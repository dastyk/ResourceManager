#ifndef IASSET_LOADER_H
#define IASSET_LOADER_H

#pragma once
#include "stdafx.h"
#include <fstream>
#include "SM_GUID.h"

struct RawData
{
	uint64_t size;
	char* data;
};

class IAssetLoader
{
private:

protected:
	IAssetLoader() { compressedFile = "ERROR NO FILE DEFINED"; };
	IAssetLoader(const std::string &file) { compressedFile = file; };
	std::string compressedFile;


public:

	virtual RawData LoadResource(SM_GUID guid)=0;

};

#endif