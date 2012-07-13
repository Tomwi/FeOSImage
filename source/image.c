//#define DEBUG

#include <feos.h>
#include <string.h>
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#include "image.h"


IMAGE image;

char* extensions[2] = {
	"jpeg",
	"png",
};

char* supported[2] = {
	".jpg .jpeg",
	".png"
};

void* internalGfxBuf;

char* findCodec(char* file)
{
	int length = strlen(file);
	int i;

	for(i=(length-1); i>0; i--) {
		/* we probably hit the extension point */
		if(file[i] == '.') {
			int j;
			for(j=0; j<2; j++) {
				if(strstr(supported[j], &file[i])) {
					return extensions[j];
				}
			}
		}
	}
	return NULL;
}
/* Load the required module and call the iopen_Image function */
int  openImage(char* file)
{
	instance_t mdl = NULL;
	if(!image.iModule) {
		char* mod = findCodec(file);
		if(!mod)
			return 0;
		mdl = FeOS_LoadModule(mod);
	}
	if(mdl) {
#ifdef DEBUG
		printf("module loaded\n");
#endif
		image.iModule			= mdl;
		image.iopenImage		= FeOS_FindSymbol(mdl, "iopenImage");
		image.idecodeImage		= FeOS_FindSymbol(mdl, "idecodeImage");
		image.icloseImage		= FeOS_FindSymbol(mdl, "icloseImage");
		int ret = image.iopenImage(file, &image.inf);
#ifdef DEBUG
		printf("%dpx, %dpx, %d\n", image.inf.width, image.inf.height, image.inf.bpp);
#endif
		if(ret<0 || !(internalGfxBuf = malloc(3*image.inf.width*image.inf.height*(image.inf.bpp/8))))
			image.icloseImage();
		return ret;
	}
	return IMAGE_NONSC_ERR;
}

int  decodeImage(void* outBuf)
{
	int ret = image.idecodeImage(internalGfxBuf);
	if(image.inf.bpp==8 && !image.inf.alphaChannel)
		RGB24_to_ARGB16(internalGfxBuf, outBuf, image.inf.width*image.inf.height);
	return ret;
}

int  closeImage(void)
{
	int ret = image.icloseImage();
	memset(&image, 0, sizeof(IMAGE));
	free(internalGfxBuf);
	internalGfxBuf = NULL;
	return ret;
}
