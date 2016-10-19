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

extern "C" __declspec(dllexport) void CompressLz77(RawData rData, CompressedData* cdata);
extern "C" __declspec(dllexport) void UncompressLz77(CompressedData cData, RawData* data);
#endif