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
	void* data;
	uint8_t fType;
};

class IAssetLoader
{
protected:
	IAssetLoader() { compressedFile = "ERROR NO FILE DEFINED"; };

	std::string compressedFile;
	std::map<uint64_t, std::string> hashTable;
	std::map<uint32_t,uint8_t> _fileTypes;

public:
	IAssetLoader(const std::string &file) { compressedFile = file; };
	virtual ~IAssetLoader() {};

	virtual RawData LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback)=0;
	uint8_t AddType(uint32_t& type)
	{
		uint8_t t = _fileTypes.size();
		_fileTypes[type] = t;//std::hash<std::string>{} (fileend)] = type;
		return t;
	}
};

#endif