//#define DEBUG

#include <feos.h>
#include <string.h>
#include "image.h"
#ifdef DEBUG
#include <stdio.h>
#endif

IMAGE image;

char* extensions[2] = {
	"jpeg",
	"png",
};

char* supported[2] = {
	".jpg .jpeg",
	".png"
};

char* findCodec(char* file){
	int length = strlen(file);
	int i;
	
	for(i=(length-1); i>0; i--){
		/* we probably hit the extension point */
		if(file[i] == '.'){
			int j;
			for(j=0; j<2; j++){
				if(strstr(supported[j], &file[i])){
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
	if(!image.iModule){
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
		if(ret<0)
			image.icloseImage();
		return ret;
	}
	return IMAGE_NONSC_ERR;
}

int  decodeImage(void* outBuf)
{
	return image.idecodeImage(outBuf);
}

int  closeImage(void)
{
	int ret = image.icloseImage();
	memset(&image, 0, sizeof(IMAGE));
	return ret;
}
