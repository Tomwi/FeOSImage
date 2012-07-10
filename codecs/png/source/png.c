#include <feos.h>
#include <stdlib.h>
#include <png.h>
#include "png_mod.h"

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif

FILE* fp;
png_structp png_ptr;
png_infop info_ptr;
png_uint_32 width, height;
int bit_depth, color_type, interlace_type;


int iopenImage(char* name, IMAGE_INFO* inf)
{
	if(!(fp = fopen(name, "rb")))
		return IMAGE_FILE_ERR;
	png_byte keyWord[4];
	if(fread(keyWord, 1, 4, fp)!=4)
		goto error;
	if(png_sig_cmp(keyWord, (png_size_t)0, 4))
		goto error;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	                                 NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return IMAGE_NONSC_ERR;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return IMAGE_NONSC_ERR;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 4);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	             &interlace_type, NULL, NULL);
	inf->width = width;
	inf->height = height;
	inf->bpp = bit_depth;
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
	return 1;
error:
	fclose(fp);
	return IMAGE_NOT_VALID;

}

int idecodeImage(void* buffer)
{
	png_bytep row_pointer = NULL;
	int rowSz = (int)png_get_rowbytes(png_ptr, info_ptr);
	row_pointer = malloc(rowSz);
	int y;
	for (y = 0; y < height; y++) {
		png_read_rows(png_ptr, &row_pointer, NULL, 1);
		int i;
		for(i=0; i<width; i++) {
			unsigned char* byteP = (unsigned char*)(row_pointer+i*3);
			hword_t ARGB = (((byteP[2] >> 3)<<10) | ((byteP[1] >> 3)<<5) | (byteP[0] >> 3)) | BIT(15);
			*(hword_t*)(row_pointer+i*2) = ARGB;
		}
		dmaCopy(row_pointer, buffer, width*2);
		buffer+=width*2;
	}
	free(row_pointer);

	return 1;
}

int iCloseImage(void)
{
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	return 1;
}
