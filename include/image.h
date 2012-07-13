#ifndef FEOS_IMAGE_H
#define FEOS_IMAGE_H

typedef struct{
	int width, height, bpp;
	char alphaChannel;	// alpha channel present?
}IMAGE_INFO;

typedef struct{
	IMAGE_INFO inf;
	instance_t iModule;
	int  (*iopenImage)(char*, IMAGE_INFO*);
	int  (*idecodeImage)(void* outBuf);
	int  (*icloseImage)(void);
}IMAGE;

enum IMAGE_ERROR{
	IMAGE_NONSC_ERR = -1,
	IMAGE_FILE_ERR 	= -2,
	IMAGE_NOT_VALID = -3,
};

int  openImage(char*);
int  decodeImage(void* outBuf);
int  closeImage(void);

void RGB24_to_ARGB16(void* src, void* dst, int len);

#endif
