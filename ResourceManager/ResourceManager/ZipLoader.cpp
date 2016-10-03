#include "ZipLoader.h"
#include <iostream>
#include <fstream>

ZipLoader::ZipLoader(const std::string & file)
{


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

void* ZipLoader::LoadResource(SM_GUID guid)
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

	//filename = unhash(guid); måste implementeras på något sätt

	//fileName = "temporär shit";

	if (zipFile.openEntry(fileName.c_str()))
	{
		returnData.size = zipFile.getEntrySize();
		returnData.data = zipFile.getData();
	}
	else
	{
		//HÄR BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER SÅ TAR VI PAJ BAKNING
	}

	uint32_t fileType = std::hash<std::string>{} (fileName.substr(fileName.length() - 3, 3));


	auto find2 = _fileTypes.find(fileType);
	if(find2 != _fileTypes.end())
	{
		returnData.fType = find2->second;
		// throw "NOW FOUND BITCH";
	}
	
	return preturnData;
}
