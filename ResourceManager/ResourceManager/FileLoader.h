#ifndef _FILE_LOADER_H_
#define _FILE_LOADER_H_

#include "IAssetLoader.h"

class FileLoader : public IAssetLoader
{
public:
	FileLoader();
	FileLoader(const std::string& filelistfile);
	virtual ~FileLoader();

	virtual RawData LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback);

};




#endif

