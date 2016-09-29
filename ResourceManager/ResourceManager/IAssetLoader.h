#ifndef IASSET_LOADER_H
#define IASSET_LOADER_H

#pragma once
#include "stdafx.h"
#include <fstream>

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

	virtual RawData LoadResource(std::string fileName)=0;

};

#endif