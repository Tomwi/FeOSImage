#include <stdio.h>
#include <string.h>
#include <jpeglib.h>
#include <jerror.h>

#include "jpeg_mod.h"

static struct jpeg_error_mgr jerr;
extern JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
extern int image_height;	/* Number of rows in image */
extern int image_width;		/* Number of columns in image */

FILE* fp;
struct jpeg_decompress_struct cinfo;

int iopenImage(char* name, IMAGE_INFO* inf)
{
	fp = fopen(name, "rb");
	if(!fp)
		return IMAGE_FILE_ERR;
	memset(&cinfo, 0, sizeof(cinfo));
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, true);
	inf->width = cinfo.image_width;
	inf->height = cinfo.image_height;
	inf->bpp = 8;
	inf->alphaChannel = 0;
	return 1;
}

int idecodeImage(void* outBuf)
{
	(void) jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer;
	buffer = (*cinfo.mem->alloc_sarray)
	         ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(outBuf, buffer[0], row_stride);
		outBuf+=row_stride;
	}
	jpeg_finish_decompress(&cinfo);
	return 1;
}

int icloseImage(void)
{
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);
	return 1;
}
