#include "pch.h"
#include "pnghandler.h"

#define PNG_BYTES_TO_CHECK 8
bool IsPNG(const char* file_name) {
	FILE* image_file = fopen(file_name, "rb");
	if (!image_file) {
		fclose(image_file);
		return NULL;
	}
	char buf[PNG_BYTES_TO_CHECK];

	if (fread(buf, 1, PNG_BYTES_TO_CHECK, image_file) == NULL) {
		fclose(image_file);
		return false;
	}

	fclose(image_file);
	return (!png_sig_cmp((png_const_bytep)buf, 0, PNG_BYTES_TO_CHECK));
}

unsigned char* LoadPNG(const char* file_name, int* width, int* height) {
	FILE* image_file = fopen(file_name, "rb");
	if (!image_file) {
		return NULL;
	}

	int bpp;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, image_file);

	png_read_info(png_ptr, info_ptr);

	*width = png_get_image_width(png_ptr, info_ptr);
	*height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	int number_of_passes = png_set_interlace_handling(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	setjmp(png_jmpbuf(png_ptr));

	png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * *height);

	for (int y = 0; y < *height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(image_file);

	switch (color_type) {
		case PNG_COLOR_TYPE_RGB:
			bpp = 3;
			break;

		case PNG_COLOR_TYPE_RGBA:
			bpp = 4;
			break;

		default:
			bpp = -1;
			break;
	}

	unsigned char* result = NULL;

	if (bpp > 0) {
		int size = *width * *height * 4;
		result = (unsigned char*)malloc(size);

		for (int y = 0; y < *height; y++) {
			png_byte* row = row_pointers[y];
			for (int x = 0; x < *width; x++) {
				result[(y * *width + x) * 4 + 0] = row[x * bpp + 0];
				result[(y * *width + x) * 4 + 1] = row[x * bpp + 1];
				result[(y * *width + x) * 4 + 2] = row[x * bpp + 2];
			}
			free(row);
		}
	}

	free(row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return result;
}