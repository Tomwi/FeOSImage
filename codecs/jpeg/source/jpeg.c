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
	return 1;
}

int idecodeImage(void* outBuf){
	(void) jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer;		
	buffer = (*cinfo.mem->alloc_sarray)
	         ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		int i;
		for(i=0; i<cinfo.image_width; i++) {
			unsigned char* byteP = (unsigned char*)(buffer[0]+i*3);
			hword_t ARGB = (((byteP[2] >> 3)<<10) | ((byteP[1] >> 3)<<5) | (byteP[0] >> 3)) | BIT(15);
			*(hword_t*)(buffer[0]+i*2) = ARGB;
		}
		dmaCopy(buffer[0], outBuf, cinfo.image_width*2);
		outBuf+=cinfo.image_width*2;
	}
	jpeg_finish_decompress(&cinfo);
	return 1;
}

int icloseImage(void){
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);
	return 1;
}


/*
 * SOME FINE POINTS:
 *
 * In the above code, we ignored the return value of jpeg_read_scanlines,
 * which is the number of scanlines actually read.  We could get away with
 * this because we asked for only one line at a time and we weren't using
 * a suspending data source.  See libjpeg.txt for more info.
 *
 * We cheated a bit by calling alloc_sarray() after jpeg_start_decompress();
 * we should have done it beforehand to ensure that the space would be
 * counted against the JPEG max_memory setting.  In some systems the above
 * code would risk an out-of-memory error.  However, in general we don't
 * know the output image dimensions before jpeg_start_decompress(), unless we
 * call jpeg_calc_output_dimensions().  See libjpeg.txt for more about this.
 *
 * Scanlines are returned in the same order as they appear in the JPEG file,
 * which is standardly top-to-bottom.  If you must emit data bottom-to-top,
 * you can use one of the virtual arrays provided by the JPEG memory manager
 * to invert the data.  See wrbmp.c for an example.
 *
 * As with compression, some operating modes may require temporary files.
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.txt.
 */
