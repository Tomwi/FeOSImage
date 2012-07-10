#ifndef JPEG_MOD_H
#define JPEG_MOD_H

#include <feos.h>
#include "../../../include/image.h"

FEOS_EXPORT	int  iopenImage(char*, IMAGE_INFO*);
FEOS_EXPORT	int  idecodeImage(void* outBuf);
FEOS_EXPORT int  icloseImage(void);;


#endif /* JPEG_MOD_H */
