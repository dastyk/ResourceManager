#ifndef IASSET_LOADER_H
#define IASSET_LOADER_H
#pragma warning (disable : 4267)
#pragma once
#include "stdafx.h"
#include <fstream>
#include "SM_GUID.h"
#include "Types.h"
#include <functional>
#include <map>

struct RawData
{
	uint32_t size;
	char* data;
	uint32_t fType;
};

class IAssetLoader
{
protected:
	IAssetLoader() { compressedFile = "ERROR NO FILE DEFINED"; };

	std::string compressedFile;
	std::map<uint64_t, std::string> hashTable;
	std::map<uint32_t,uint32_t> _fileTypes;

public:
	IAssetLoader(const std::string &file) { compressedFile = file; };
	virtual ~IAssetLoader() {};

	virtual RawData* LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback)=0;
	void AddType(uint32_t& type)
	{
		uint32_t t = _fileTypes.size();
		_fileTypes[type] = _fileTypes.size();//std::hash<std::string>{} (fileend)] = type;
		type = t;
	}
};

#endif