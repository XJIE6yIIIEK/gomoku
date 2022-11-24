#pragma once

#ifdef LOADIMAGE_EXPORTS
#define LOADIMAGE_API __declspec(dllexport)
#else
#define LOADIMAGE_API __declspec(dllimport)
#endif


extern "C" __declspec(dllexport) unsigned char* LoadImageS(const char*, int*, int*);
