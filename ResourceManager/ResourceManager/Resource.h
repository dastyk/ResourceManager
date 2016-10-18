#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Observer.h"
#include "flags.h"
#include "MemoryManager.h"

class ResourceManager;

class Resource
{
	friend ResourceManager;
public:
	class Ptr
	{
		friend Resource;
		//friend ResourceManager;
	private:
		Ptr(uint32_t index, SM_GUID guid, const void* data, uint8_t type, uint32_t size, Observer** observerPtr, bool& pinned) : index(index), guid(guid), data(data), type(type), size(size), _observerPtr(observerPtr), pinned(pinned)
		{

		}
	public:
		~Ptr()
		{

		}
		const SM_GUID guid;
		const void* data;
		const uint8_t type;
		const uint32_t size;
		void RegisterObserver(Observer* observer)
		{
			*_observerPtr = observer;
		}
	private:
		const uint32_t index;
		bool& pinned;
		Observer** _observerPtr;
	};

	CreateFlag(Flag, uint32_t, 4,
		PERSISTENT = 1 << 0,
		NOT_URGENT = 1 << 1,
		NEEDED_NOW = 1 << 2,
		LOAD_AND_WAIT = 1 << 3
	);

	static const uint32_t NotFound = UINT32_MAX;

	const static size_t Size =
		sizeof(bool) +
		sizeof(SM_GUID) +
		sizeof(Resource::Flag) +
		sizeof(uint16_t) +
		sizeof(Observer*) +
		sizeof(void*) +
		sizeof(uint8_t) +
		sizeof(uint32_t) +
		sizeof(uint32_t) +
		sizeof(uint32_t);

	struct DataPointers
	{
		bool* pinned;
		SM_GUID* guid;
		Resource::Flag* flags;
		uint16_t* refCount;
		Observer** observer;
		void** rawData;
		uint8_t* type;
		uint32_t* size;
		uint32_t* startBlock;
		uint32_t* numBlocks;
	};

private:

	Resource(uint32_t limit) : count(0), limit(0)
	{
		Allocate(limit);
	}
	~Resource()
	{
		MemoryManager::Release(buffer);
	}
	static Resource* instance;
	static void Init(uint32_t count)
	{
		instance = new Resource(count);
	}
	static void Shutdown()
	{
		delete instance;
	}
	static const uint32_t& Limit()
	{
		return instance->limit;
	}
	static uint32_t& Count()
	{
		return instance->count;
	}
	static const Resource::DataPointers& Data()
	{
		return instance->data;
	}
	static Ptr MakePtr(uint32_t index)
	{
		auto& data = instance->data;
		return Ptr(index, data.guid[index], data.rawData[index], data.type[index], data.size[index], &data.observer[index], data.pinned[index]);
	}
	uint32_t limit = 0;
	uint32_t count = 0;
	void* buffer = nullptr;
	Resource::DataPointers data;

	static uint32_t Find(const SM_GUID & guid);
	static void Remove(const uint32_t index);
	void Allocate(uint32_t numResources);


};

#endif
