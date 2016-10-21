#include "FileLoader.h"
#include <string>
FileLoader::FileLoader()
{
	//nothing
}

/*
filelistfile is a textfile in the format:
filename.jpg
filename2.jpg
mesh.arf
mesh2.arf

and so on
*/
FileLoader::FileLoader(const std::string& filelistfile)
{
	std::ifstream in(filelistfile);
	if (!in.is_open())
		throw std::exception(std::string("Unable to open file ").append(filelistfile).c_str());
	std::string filename;
	while (std::getline(in, filename))
	{
		std::ifstream testexist(filename);
		if (!testexist.fail())
		{
			//If we dont get here, file probably doesnt exist
			//It might though, but have restricted read access
			hashTable[SM_GUID(filename.c_str())] = filename;	
		}
	}
}

FileLoader::~FileLoader()
{
}

RawData FileLoader::LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback)
{
	auto& find = hashTable.find(guid);
	if(find == hashTable.end())
		throw std::exception("Tried to load nonexistent file");



	//Get file ending too
	std::string filename = find->second;
	std::string fileend;
	for (std::string::const_reverse_iterator i = filename.crend(); i != filename.crbegin(); i++)
	{
		if (*i != '.')
			fileend.append(*i, 1);
		else
			i = filename.crbegin();
	}
	uint32_t endhash = std::hash<std::string>{} (fileend);
	auto& find2 = _fileTypes.find(endhash);
	if (find2 == _fileTypes.end())
		throw std::exception("Unknown filetype");

	std::ifstream in(find->second);
	if (in.fail())
		throw std::exception("Could not open file to load");

	std::streampos filesize = in.tellg();
	in.seekg(0, std::ios::end);
	filesize = in.tellg() - filesize;
	in.seekg(0);
	size_t size = filesize;

	RawData rd;
	rd.size = size;
	rd.data = allocCallback(size);
	if (rd.data == nullptr)
		throw std::exception("Chunky allocator out of memory");
	in.read((char*)rd.data, filesize);

	return rd;
}
