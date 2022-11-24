#include "pch.h"
#include "LoadImage.h"
#include "pnghandler.h"
#include "jpeghandler.h"



extern "C" __declspec(dllexport) unsigned char* LoadImageS(const char* file_name, int* width, int* height) {	
	unsigned char* result = NULL;
	if (IsPNG(file_name)) {
		result = LoadPNG(file_name, width, height);
	} else if (IsJPEG(file_name)) {
		result = LoadJPEG(file_name, width, height);
	}

	return result;
}