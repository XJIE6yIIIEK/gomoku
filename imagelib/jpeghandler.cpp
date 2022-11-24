#include "pch.h"
#include "jpeghandler.h"
#include <jpeglib.h>

#define JPEG_BYTES_TO_CHECK 8
bool IsJPEG(const char* file_name) {
	unsigned char buf[JPEG_BYTES_TO_CHECK];
	FILE* image_file = fopen(file_name, "rb");
	if (!image_file) {
		fclose(image_file);
		return NULL;
	}

	if (fread(buf, 1, JPEG_BYTES_TO_CHECK, image_file) == NULL) {
		fclose(image_file);
		return false;
	}

	fclose(image_file);
	return jpeg_sig_cmp(buf, 0, JPEG_BYTES_TO_CHECK);
}

bool jpeg_sig_cmp(unsigned char* buf, int start, int end) {
	return (buf[0] == 255 && buf[1] == 216 && buf[2] == 255);
}

GLOBAL(unsigned char*) LoadJPEG(const char* file_name, int* width, int* height) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr pub;
	JSAMPARRAY buffer;
	jmp_buf setjmp_buffer;
	int row_stride;

	FILE* image_file = fopen(file_name, "rb");
	if (!image_file) {
		fclose(image_file);
		return NULL;
	}

	cinfo.err = jpeg_std_error(&pub);
	if (setjmp(setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(image_file);
		return NULL;
	}
		
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, image_file);

	(void)jpeg_read_header(&cinfo, TRUE);
	
	(void)jpeg_start_decompress(&cinfo);

	row_stride = cinfo.output_width * cinfo.output_components;

	*width = cinfo.output_width;
	*height = cinfo.output_height;
	int* components = new int(cinfo.output_components);

	buffer = ((cinfo).mem->alloc_sarray)((j_common_ptr)& cinfo, JPOOL_IMAGE, row_stride, 1);
	if (buffer == NULL)
		return NULL;

	unsigned char* result = new unsigned char[*width * *height * 4];

	int resIter = 0;
	int current_row = 0;
	while ((cinfo).output_scanline < (cinfo).output_height) {
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		for (int i = 0; i < *width; i++) {
			result[resIter] = (unsigned char)buffer[0][*components * i + 0];
			result[resIter + 1] = (unsigned char)buffer[0][*components * i + 1];
			result[resIter + 2] = (unsigned char)buffer[0][*components * i + 2];
			if((int)result[resIter] == 255 && (int)result[resIter + 1] == 255 && (int)result[resIter + 2] == 255)
				result[resIter + 3] = 0;
			else
				result[resIter + 3] = 1;
			resIter += 4;
		}
		current_row++;
	}

	(void)jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);
	fclose(image_file);

	return result;
}