#ifndef IASSET_LOADER_H
#define IASSET_LOADER_H

#pragma once
#include "stdafx.h"
#include <fstream>
#include "SM_GUID.h"
#include "Types.h"

#include <unordered_map>

struct RawData
{
	uint64_t size;
	char* data;
	FileType fType;
};

class IAssetLoader
{
private:

protected:
	IAssetLoader() { compressedFile = "ERROR NO FILE DEFINED"; };

	std::string compressedFile;
	std::unordered_map<uint64_t, std::string> hashTable;


public:
	IAssetLoader(const std::string &file) { compressedFile = file; };
	virtual ~IAssetLoader() {};

	virtual RawData LoadResource(SM_GUID guid)=0;

};

#endif