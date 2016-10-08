#include "ZipLoader.h"
#include <iostream>
#include <fstream>

ZipLoader::ZipLoader(const std::string & file)
{
	uint64_t data[6];

	data[0] = std::hash<std::string>{} ("Sphere0.arf");
	data[1] = std::hash<std::string>{} ("Sphere1.arf");
	data[2] = std::hash<std::string>{} ("Sphere2.arf");
	data[3] = std::hash<std::string>{} ("Sphere3.arf");
	data[4] = std::hash<std::string>{} ("Sphere4.arf");
	data[5] = std::hash<std::string>{} ("Sphere5.arf");
	compressedFile = file;

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

RawData* ZipLoader::LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback)
{
	RawData* preturnData = new RawData;
	RawData& returnData = *preturnData;
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
		zipFile.getData( dataSize, storageLocation );
		returnData.size = dataSize;
		returnData.data = storageLocation;
	}
	else
	{
		//HÄR BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER SÅ TAR VI PAJ BAKNING
	}
	std::string end = fileName.substr(fileName.length() - 3, 3);
	uint32_t fileType = std::hash<std::string>{} (end);


	auto find2 = _fileTypes.find(fileType);
	if(find2 != _fileTypes.end())
	{
		returnData.fType = find2->second;
		// throw "NOW FOUND BITCH";
	}
	
	return preturnData;
}
