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

	//filename = unhash(guid); m�ste implementeras p� n�got s�tt

	fileName = "tempor�r shit";

	if (zipFile.openEntry(fileName.c_str()))
	{
		returnData.size = zipFile.getEntrySize();
		returnData.data = zipFile.getData();
	}
	else
	{
		int potato = 0;
		potato++;
		//H�R BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER S� TAR VI PAJ BAKNING
	}

	return returnData;
}
