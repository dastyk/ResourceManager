#include "DarferLoader.h"


#include <string>
#include <algorithm>
void DarferLoader::ReadBinary(std::unordered_map<uint64_t, EntryData>& entries)
{

	int32_t size = 0;
	char* buffer = new char[1000];

	// Read name
	drfFile.read((char*)&size, 1);
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
	auto find = _entries.find(guid.data);
	if (find == _entries.end())
	{
		throw std::runtime_error("Could not find resource with GUID: " + std::to_string(guid));
	}



	RawData data;
	
	drfFile.seekg(find->second.offset, std::ios_base::end);
	data.size = find->second.size;
	data.data = allocCallback(data.size);
	if (data.data == nullptr)
	{
		throw std::runtime_error("Chunky Pool Allocator out of memory.");
	}	
	drfFile.read((char*)data.data, data.size);

	auto find2 = _fileTypes.find(find->second.extHash);
	if (find2 == _fileTypes.end())
	{
		throw std::runtime_error("Fileformat with hash " + std::to_string(find->second.extHash) + " not recognized.");
	}


	data.fType = find2->first;

	return data;
}
