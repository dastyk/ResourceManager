#pragma once
#ifndef _COMPRESSLZ77_H_
#define _COMPRESSLZ77_H_
#include <stdint.h>

 struct CompressedData
{
	uint64_t sizeCompressed;
	uint64_t sizeUncompressed;
	unsigned char* data;
};

 struct RawData
{
	RawData()
	{
		size = 0;
		data = nullptr;
	}

	RawData(uint64_t s, unsigned char* d)
	{
		size = s;
		data = d;
	}

	uint64_t size;
	unsigned char* data;
};

extern "C" __declspec(dllexport) void CompressLz77(void* rdata, uint64_t size, uint64_t* csize, void** cdata);
extern "C" __declspec(dllexport) void UncompressLz77(void* rdata, uint64_t csize, void* cdata);
extern "C" __declspec(dllexport) void CompressHuffman(void* rdata, uint64_t size, uint64_t* csize, void** cdata);
extern "C" __declspec(dllexport) void UncompressHuffman(void* rdata, uint64_t size, void* cdata);
#endif