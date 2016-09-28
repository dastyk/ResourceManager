#ifndef ZIP_LOADER_H
#define ZIP_LOADER_H

#pragma once
#include "IAssetLoader.h"
#include "zipper.h"
#include "unzipper.h"

class ZipLoader: public IAssetLoader
{

private:
	
	ZipLoader() { compressedFile = "ERROR NO FILE DEFINED"; };

	ziputils::unzipper zipFile;

public:

	ZipLoader(const std::string &file);
	RawData LoadResource(std::string fileName);

};

#endif