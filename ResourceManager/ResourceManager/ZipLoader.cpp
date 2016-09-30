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

RawData ZipLoader::LoadResource(SM_GUID guid)
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

	//filename = unhash(guid); mÂste implementeras pÅEnÂgot s‰tt

	//fileName = "tempor‰r shit";

	if (zipFile.openEntry(fileName.c_str()))
	{
		returnData.size = zipFile.getEntrySize();
		returnData.data = zipFile.getData();
	}
	else
	{
		//HƒR BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER S≈ TAR VI PAJ BAKNING
	}

	uint32_t fileType = std::hash<std::string>{} (fileName.substr(fileName.length() - 3, 3));


	auto find2 = _fileTypes.find(fileType);
	if(find2 != _fileTypes.end())
	{
		returnData.fType = find2->second;
		// throw "NOW FOUND BITCH";
	}
	
	return returnData;
}
