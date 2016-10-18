#include "Resource.h"


Resource* Resource::instance = nullptr;
uint32_t Resource::Find(const SM_GUID & guid)
{
	for (uint32_t i = 0; i < instance->count; i++)
	{
		uint64_t guid2 = instance->data.guid[i];
		if (guid2 == guid)
			return i;
	}
	return Resource::NotFound;
}

void Resource::Remove(const uint32_t index)
{
	Resource::DataPointers& data = instance->data;
	uint32_t& count = instance->count;
	data.pinned[index] = true;
	while (data.pinned[count]);
	data.pinned[count] = true;

	data.guid[index] = data.guid[count];
	data.flags[index] = data.flags[count];
	data.refCount[index] = data.refCount[count];
	data.observer[index] = data.observer[count];
	data.rawData[index] = data.rawData[count];
	data.type[index] = data.type[count];
	data.size[index] = data.size[count];
	data.startBlock[index] = data.startBlock[count];
	data.numBlocks[index] = data.numBlocks[count];
	data.pinned[count] = false;
	count--;

}

void Resource::Allocate(uint32_t numResources)
{
	if (numResources <= limit)
	{
		throw std::runtime_error("Can shrink the resource chunk.");
	}

	void* newC = (char*)MemoryManager::Alloc(Resource::Size*numResources);
	Resource::DataPointers newData;

	newData.pinned = (bool*)newC;
	newData.guid = (SM_GUID*)(newData.pinned + numResources);
	newData.flags = (Resource::Flag*)(newData.guid + numResources);
	newData.refCount = (uint16_t*)(newData.flags + numResources);
	newData.observer = (Observer**)(newData.refCount + numResources);
	newData.rawData = (void**)(newData.observer + numResources);
	newData.type = (uint8_t*)(newData.rawData + numResources);
	newData.size = (uint32_t*)(newData.type + numResources);
	newData.startBlock = (uint32_t*)(newData.size + numResources);
	newData.numBlocks = (uint32_t*)(newData.startBlock + numResources);

	memcpy(newData.pinned, data.pinned, count * sizeof(bool));
	memcpy(newData.guid, data.guid, count * sizeof(SM_GUID));
	memcpy(newData.flags, data.flags, count * sizeof(Resource::Flag));
	memcpy(newData.refCount, data.refCount, count * sizeof(uint16_t));
	memcpy(newData.observer, data.observer, count * sizeof(Observer*));
	memcpy(newData.rawData, data.rawData, count * sizeof(void*));
	memcpy(newData.type, data.type, count * sizeof(uint8_t));
	memcpy(newData.size, data.size, count * sizeof(uint32_t));
	memcpy(newData.startBlock, data.startBlock, count * sizeof(uint32_t));
	memcpy(newData.numBlocks, data.numBlocks, count * sizeof(uint32_t));

	limit = numResources;
	MemoryManager::Release(buffer);
	buffer = newC;
	data = newData;
}