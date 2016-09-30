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

	hashTable[data[0]] = "Sphere0.arf";
	hashTable[data[1]] = "Sphere1.arf";
	hashTable[data[2]] = "Sphere2.arf";
	hashTable[data[3]] = "Sphere3.arf";
	hashTable[data[4]] = "Sphere4.arf";
	hashTable[data[5]] = "Sphere5.arf";

	compressedFile = file;

	if (!zipFile.open(file.c_str()))
	{
		// THROW ERRORS AND SHIT BROZKY, TIME TO ABANDON SHIP CAUSE THIS AIN'T MY FUCKING FAULT!
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


	auto find = _fileTypes.find(fileType);
	if(find != _fileTypes.end())
	{
		returnData.fType = find.second;
		// throw "NOW FOUND BITCH";
	}
	
	return returnData;
}
