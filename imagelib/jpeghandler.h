#pragma once

bool IsJPEG(const char*);
bool jpeg_sig_cmp(unsigned char*, int, int);
unsigned char* PrepareLoadingJPEG(FILE*, int*, int*);
unsigned char* LoadJPEG(/*jpeg_decompress_struct*, */const char*, int*, int*);