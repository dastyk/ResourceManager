#include "DarferLoader.h"


#include <string>
#include <algorithm>
void DarferLoader::ReadBinary(std::unordered_map<uint64_t, EntryData>& entries)
{

	uint32_t size = 0;
	char* buffer = new char[1000];

	// Read name
	drfFile.read((char*)&size, 4);
	drfFile.read(buffer, size);
	std::string name(buffer, size);
	delete[] buffer;
	uint64_t nameHash = std::hash<std::string>{}(name);

	// File or folder
	bool file = false;
	drfFile.read((char*)&file, 1);
	if (file)
	{
		EntryData entry;
		entry.extHash = std::hash<std::string>{}(name.substr(name.find_last_of(".") + 1));
		bool compressed = false;
		drfFile.read((char*)&compressed, 1);
		drfFile.read((char*)&entry.offset, 8);
		drfFile.read((char*)&entry.size, 8);

		entries[nameHash] = std::move(entry);
	}

	uint32_t chilren = 0;
	drfFile.read((char*)&chilren, 4);

	for (uint32_t i = 0; i < chilren; i++)
	{
		ReadBinary(entries);
	}


}

DarferLoader::DarferLoader(const char* path)
{

	drfFile.open(path, std::ios::binary);
	if (!drfFile.is_open())
	{
		throw std::runtime_error("Could not find .drf file.");
	}

	ReadBinary(_entries);

}



DarferLoader::~DarferLoader()
{
	drfFile.close();
}

RawData DarferLoader::LoadResource(SM_GUID guid, std::function<char*(uint32_t dataSize)> allocCallback)
{
	return RawData();
}