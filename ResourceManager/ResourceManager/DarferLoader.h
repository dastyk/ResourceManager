#ifndef _DARFER_LOADER_H_
#define _DARFER_LOADER_H_

#pragma once
#include "IAssetLoader.h"
#include <fstream>
#include <unordered_map>
class DarferLoader : public IAssetLoader
{
	struct EntryData
	{
		uint8_t compressionType;
		uint32_t extHash;
		uint64_t offset;
		uint64_t size;
	};

	std::ifstream drfFile;


	void ReadBinary(std::unordered_map<uint64_t, EntryData>& entries);
	std::unordered_map<uint64_t, EntryData> _entries;
public:
	DarferLoader(const char* path);
	~DarferLoader();


	RawData LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback);
};

#endif