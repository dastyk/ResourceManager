#include "DarferLoader.h"
#include "MemoryManager.h"
#include <CompressLZ77.h>
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
		drfFile.read((char*)&entry.compressionType, sizeof(uint8_t));
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
	int64_t s = -(int64_t)find->second.offset;
	drfFile.seekg(s, std::ios_base::end);
	data.size = find->second.size;
	uint64_t rawsize = find->second.size;
	
	if (find->second.compressionType)
	{
		rawsize -= sizeof(uint64_t);
		uint64_t uncompressedSize;
		drfFile.read((char*)&uncompressedSize, sizeof(uint64_t));

		char* buffer = (char*)MemoryManager::Alloc(rawsize);
		uint64_t secondUncompressionSize = 0;
		if(find->second.compressionType == 1 || find->second.compressionType == 3)
		{
			rawsize -= sizeof(uint64_t);
			drfFile.read((char*)&secondUncompressionSize, sizeof(uint64_t));
		}
		drfFile.read(buffer, rawsize);
		data.data = allocCallback(uncompressedSize);
		data.size = uncompressedSize;

		if (data.data == nullptr)
		{
			MemoryManager::Release(buffer);
			throw std::runtime_error("Chunky Pool Allocator out of memory.");
		}
		if (find->second.compressionType == 1)//LZ77 then huff
		{
			char* huffer = (char*)MemoryManager::Alloc(secondUncompressionSize);
			UncompressHuffman(huffer, secondUncompressionSize, buffer);
			UncompressLz77(data.data, secondUncompressionSize, huffer);
			MemoryManager::Release(huffer);
		}
		else if (find->second.compressionType == 2)//Only LZ77
		{
			UncompressLz77(data.data, rawsize, buffer);
		}
		else if (find->second.compressionType == 3)
		{
			char* zuffer = (char*)MemoryManager::Alloc(secondUncompressionSize);
			UncompressLz77(zuffer, rawsize, buffer);
			UncompressHuffman(data.data, uncompressedSize, zuffer);
			MemoryManager::Release(zuffer);
		}
		else if (find->second.compressionType == 4)
		{
			UncompressHuffman(data.data, uncompressedSize, buffer);
		}
		MemoryManager::Release(buffer);
	}
	else
	{
		data.data = allocCallback(data.size);
		if (data.data == nullptr)
		{
			throw std::runtime_error("Chunky Pool Allocator out of memory.");
		}
		drfFile.read((char*)data.data, data.size);
	}

	auto find2 = _fileTypes.find(find->second.extHash);
	if (find2 == _fileTypes.end())
	{
		throw std::runtime_error("Fileformat with hash " + std::to_string(find->second.extHash) + " not recognized.");
	}


	data.fType = find2->second;

	return data;
}
