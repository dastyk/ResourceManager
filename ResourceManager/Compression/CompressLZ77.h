#pragma once
#ifndef _COMPRESSLZ77_H_
#define _COMPRESSLZ77_H_
#include <stdint.h>



extern "C" __declspec(dllexport) void CompressLz77(void* rdata, uint64_t size, uint64_t* csize, void** cdata);
extern "C" __declspec(dllexport) void UncompressLz77(void* rdata, uint64_t csize, void* cdata);
extern "C" __declspec(dllexport) void CompressHuffman(void* rdata, uint64_t size, uint64_t* csize, void** cdata);
extern "C" __declspec(dllexport) void UncompressHuffman(void* rdata, uint64_t size, void* cdata);
#endif