#include "Resource.h"


uint32_t Resource::FindAndWait(const SM_GUID & guid)
{
	Modify();
	for (uint32_t i = 0; i < count; i++)
	{
		if (data.guid[i] == guid)
		{
			if (data.pinned[i].try_lock())
			{

				modifyLock.unlock();
				return i;
			}
			else
			{
				data.pinned[i].lock();
				modifyLock.unlock();
				return i;
			}
		}
	}
	SearchDone();
	return Resource::NotFound;
}

uint32_t Resource::Find(const SM_GUID & guid, uint32_t* pinned)
{
	Modify();
	for (uint32_t i = 0; i < count; i++)
	{
		if (data.guid[i] == guid)
		{
			if (data.pinned[i].try_lock())
			{

				SearchDone();
				return i;
			}
			else
			{
				if (pinned)
					*pinned = i;
				SearchDone();
				return Resource::NotFound;
			}
		}
	}
	SearchDone();
	return Resource::NotFound;
}


void Resource::Remove(const uint32_t index)
{
	uint32_t last = count - 1;
	if (last == index)
	{
	//	data.pinned[index].unlock();
		Remove();
		return;
	}
	data.pinned[last].lock();
	for (uint32_t i = 0; i < data.observers[index].numObservers; i++)
	{
		data.observers[index].observers[i]->NotifyDelete(data.guid[index]);
	}
		
	data.loaded[index] = data.loaded[last];
	data.guid[index] = data.guid[last];
	data.flags[index] = data.flags[last];
	data.refCount[index] = data.refCount[last];
	data.observers[index] = data.observers[last];
	data.rawData[index] = data.rawData[last];
	data.type[index] = data.type[last];
	data.size[index] = data.size[last];
	data.startBlock[index] = data.startBlock[last];
	data.numBlocks[index] = data.numBlocks[last];
	data.timeStamp[index] = data.timeStamp[last];
	data.pinned[last].unlock();
	//data.pinned[index].unlock();
	Remove();

}

void Resource::Allocate(uint32_t numResources)
{
	modifyLock.lock();
	if (numResources <= limit)
	{
		throw std::runtime_error("Can shrink the resource chunk.");
	}

	void* newC = (char*)MemoryManager::Alloc(Resource::Size*numResources);
	Resource::DataPointers newData;

	newData.pinned = (std::recursive_mutex*)newC;
	newData.loaded = (bool*)(newData.pinned + numResources);
	newData.guid = (SM_GUID*)(newData.loaded + numResources);
	newData.flags = (Resource::Flag*)(newData.guid + numResources);
	newData.refCount = (uint16_t*)(newData.flags + numResources);
	newData.observers = (ObserverData*)(newData.refCount + numResources);
	newData.rawData = (void**)(newData.observers + numResources);
	newData.type = (uint8_t*)(newData.rawData + numResources);
	newData.size = (uint32_t*)(newData.type + numResources);
	newData.startBlock = (uint32_t*)(newData.size + numResources);
	newData.numBlocks = (uint32_t*)(newData.startBlock + numResources);
	newData.timeStamp = (uint64_t*)(newData.numBlocks + numResources);

	memcpy(newData.pinned, data.pinned, count * sizeof(std::recursive_mutex));
	memcpy(newData.loaded, data.loaded, count * sizeof(bool));
	memcpy(newData.guid, data.guid, count * sizeof(SM_GUID));
	memcpy(newData.flags, data.flags, count * sizeof(Resource::Flag));
	memcpy(newData.refCount, data.refCount, count * sizeof(uint16_t));
	memcpy(newData.observers, data.observers, count * sizeof(ObserverData));
	memcpy(newData.rawData, data.rawData, count * sizeof(void*));
	memcpy(newData.type, data.type, count * sizeof(uint8_t));
	memcpy(newData.size, data.size, count * sizeof(uint32_t));
	memcpy(newData.startBlock, data.startBlock, count * sizeof(uint32_t));
	memcpy(newData.numBlocks, data.numBlocks, count * sizeof(uint32_t));
	memcpy(newData.timeStamp, data.timeStamp, count * sizeof(uint64_t));

	for (uint32_t i = 0; i < numResources; i++)
	{
		new (&newData.pinned[i]) std::recursive_mutex();
	}

	limit = numResources;
	MemoryManager::Release(buffer);
	buffer = newC;
	data = newData;
	modifyLock.unlock();
}

void Resource::UnAllocte()
{
	
	/*for (uint32_t i = 0; i < count; i++)
	{
		data.pinned[i].lock();
			data.pinned[i].unlock();
		data.pinned[i].~mutex();
	}*/
	MemoryManager::Release(buffer);
}

void Resource::Ptr::RegisterObserver(Observer * observer)const
{
	if (_observers->numObservers < Resource::MAX_OBSERVERS)
	{
		_observers->observers[_observers->numObservers] = observer;
		_observers->numObservers++;
	}
}
