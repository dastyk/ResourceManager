// ZipExample.cpp : Defines the entry point for the console application.
//

#include "IAssetLoader.h"
#include "ZipLoader.h"

//#include "stdafx.h"
//#include "zipper.h"
//#include "unzipper.h"
//#include <fstream>
//
//struct LoadedData
//{
//	uint64_t size;
//	char* data;
//};
//
//using namespace std;
//using namespace ziputils;
//
//void zip()
//{
//	//const char* Filenames[] = {"unzipper.cpp", "unzipper.h", "zipper.cpp", "zipper.h"};
//	const char* Filenames[] = { "Test1.txt", "Test2.txt" };
//	unsigned int nCount = sizeof(Filenames) / sizeof(char*);
//
//	zipper zipFile;
//	zipFile.open("test.zip");
//
//	// add files to the zip file
//	for (unsigned int i = 0; i < nCount; i++)
//	{
//		ifstream file(Filenames[i], ios::in | ios::binary);
//		if (file.is_open())
//		{
//			zipFile.addEntry(Filenames[i]);
//			zipFile << file;
//		}
//	}
//	zipFile.close();
//}
//
//void zipFolder()
//{
//	// append the existing file
//	zipper zipFile;
//	zipFile.open("test.zip", true);
//
//	// add file into a folder
//	ifstream file("unzipper.cpp", ios::in | ios::binary);
//	if (file.is_open())
//	{
//		zipFile.addEntry("/Folder/unzipper.cpp");
//		zipFile << file;
//	}
//	zipFile.close();
//}
//
//void unzip(std::string filenameToLookFor)
//{
//	unzipper zipFile;
//	LoadedData temp;
//	zipFile.open("Test.zip");
//	//auto filenames = zipFile.getFilenames();
//
//	if (zipFile.openEntry(filenameToLookFor.c_str()))
//	{
//		temp.size = zipFile.getEntrySize();
//		//temp.data = new char[temp.size];
//		//zipFile.closeEntry();
//		temp.data = zipFile.getData();
//	}
//	else
//	{
//		int potato = 0;
//		potato++;
//		//HÄR BORDE NOG ETT EXCEPTION KASTAS SENARE, ELLER SÅ TAR VI PAJ BAKNING
//	}
//
//		//zipFile.openEntry((*it).c_str());
//		//zipFile >> std::cout;
//}

int main()
{
	//unzip("Test1.txt");

	IAssetLoader* ziploader;
	ziploader = new ZipLoader("Test.zip");

	RawData check = ziploader->LoadResource("Test1.txt");

	getchar();
	return 0;
}

