//#define DEBUG

#include <feos.h>
#include <string.h>
#include "image.h"
#ifdef DEBUG
#include <stdio.h>
#endif

IMAGE image;

/* Load the required module and call the iopen_Image function */
int  openImage(char* file)
{
	instance_t mdl = NULL;
	if(!image.iModule)
		mdl = FeOS_LoadModule("png");
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
