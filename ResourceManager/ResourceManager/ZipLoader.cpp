#include "ZipLoader.h"
#include <iostream>
#include <fstream>

ZipLoader::ZipLoader(const std::string & file)
{
	if (!zipFile.open(file.c_str()))
	{
		// THROW ERRORS AND SHIT BROZKY, TIME TO ABANDON SHIP CAUSE THIS AIN'T MY FUCKING FAULT!
	}


	auto names = zipFile.getFilenames();
	for (auto& n : names)
	{
		uint64_t hash = std::hash<std::string>{} (n);
		hashTable[hash] = n;
	}


}

ZipLoader::~ZipLoader()
{
	// nothing to do here
}

RawData ZipLoader::LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback)
{
	RawData returnData;
	std::string fileName;

	auto find = hashTable.find(guid.data);

	if (find != hashTable.end())
	{
		fileName = find->second;
	}
	else
	{
		// THROW EXCEPTIONS EVERYONE, IT'S PARTY TIME
	}

	if (zipFile.openEntry(fileName.c_str()))
	{
		uint32_t dataSize = zipFile.getEntrySize();
		char* storageLocation = allocCallback( dataSize );
		if (storageLocation == nullptr)
		{
			throw std::runtime_error("Chunky Pool Allocator out of memory.");
		}
		zipFile.getData( dataSize, storageLocation );
		returnData.size = dataSize;
		returnData.data = storageLocation;
	}
	else
	{
		//H�R BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER S� TAR VI PAJ BAKNING
	}
	std::string end = fileName.substr(fileName.length() - 3, 3);
	uint32_t fileType = std::hash<std::string>{} (end);


	auto find2 = _fileTypes.find(fileType);
	if(find2 != _fileTypes.end())
	{
		returnData.fType = find2->second;
		// throw "NOW FOUND BITCH";
	}
	
	return returnData;
}
