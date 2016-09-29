#include "ZipLoader.h"

ZipLoader::ZipLoader(const std::string & file)
{
	compressedFile = file;

	if (!zipFile.open(file.c_str()))
	{
		// THROW ERRORS AND SHIT BROZKY, TIME TO ABANDON SHIP CAUSE THIS AIN'T MY FUCKING FAULT!
	}

}

RawData ZipLoader::LoadResource(SM_GUID guid)
{
	RawData returnData;
	std::string fileName;

	//filename = unhash(guid); måste implementeras på något sätt

	fileName = "temporär shit";

	if (zipFile.openEntry(fileName.c_str()))
	{
		returnData.size = zipFile.getEntrySize();
		returnData.data = zipFile.getData();
	}
	else
	{
		int potato = 0;
		potato++;
		//HÄR BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER SÅ TAR VI PAJ BAKNING
	}

	return returnData;
}
