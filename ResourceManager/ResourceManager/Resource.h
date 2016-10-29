#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Observer.h"
#include "flags.h"
#include "MemoryManager.h"
#include <mutex>
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
		Ptr(uint32_t index,const SM_GUID& guid, const void*& data, const  uint8_t& type, const  uint32_t& size, Observer** observerPtr) : index(index), guid(guid), data(data), type(type), size(size), _observerPtr(observerPtr)
		{

		}
	public:
		~Ptr()
		{

		}
		const SM_GUID& guid;
		const void*& data;
		const uint8_t& type;
		const uint32_t& size;
		void RegisterObserver(Observer* observer) const
		{
			*_observerPtr = observer;
		}
	private:
		const uint32_t index;
		Observer** _observerPtr;
	};

	CreateFlag(Flag, uint32_t, 4,		
		NOT_URGENT = 1 << 0,
		NEEDED_NOW = 1 << 1,
		LOAD_AND_WAIT = 1 << 2,
		PERSISTENT = 1 << 3
	);

	static const uint32_t NotFound = UINT32_MAX;

	const static size_t Size =
		sizeof(std::recursive_mutex) +
		sizeof(bool) +
		sizeof(SM_GUID) +
		sizeof(Resource::Flag) +
		sizeof(uint16_t) +
		sizeof(Observer*) +
		sizeof(void*) +
		sizeof(uint8_t) +
		sizeof(uint32_t) +
		sizeof(uint32_t) +
		sizeof(uint32_t)+
		sizeof(uint64_t);

	struct DataPointers
	{
		std::recursive_mutex* pinned;
		bool* loaded;
		SM_GUID* guid;
		Resource::Flag* flags;
		uint16_t* refCount;
		Observer** observer;
		void** rawData;
		uint8_t* type;
		uint32_t* size;
		uint32_t* startBlock;
		uint32_t* numBlocks;
		uint64_t* timeStamp;
	};

private:
	std::mutex modifyLock;
	uint32_t limit = 0;
	uint32_t count = 0;
	void* buffer = nullptr;
	Resource::DataPointers data;


	Resource() : count(0), limit(0)
	{
		
	}
	~Resource()
	{
		
	}

	/*The MakePtr function may not be called on a resource that is not pinned.*/
	Ptr MakePtr(uint32_t index)
	{
		return Ptr(index, (const SM_GUID&)data.guid[index], (const void*&)data.rawData[index], (const uint8_t&)data.type[index], (const uint32_t&)data.size[index], &data.observer[index]);
	}
	
	void DestroyPtr(const Ptr& resource)
	{
		data.pinned[resource.index].unlock();
	}
	
	uint32_t FindAndWait(const SM_GUID& guid);
	uint32_t Find(const SM_GUID& guid, uint32_t* pinned = nullptr);

	/*The Remove function may only be called after Modify has been called.*/
	void Remove(const uint32_t index);
	void Allocate(uint32_t numResources);
	void UnAllocte();

	/*The Modify function must be called before a number of functions.*/
	inline void Modify()
	{
		modifyLock.lock();
	}

	/*The SearchDone function may only be called after Modify has been called.*/
	inline void SearchDone()
	{
		modifyLock.unlock();
	}

	/*The Remove function may only be called after Modify has been called.*/
	inline void Remove()
	{
		count--;
		modifyLock.unlock();
	}

	/*The Add function may only be called after Modify has been called.*/
	inline void Add()
	{
		count++;
		modifyLock.unlock();
	}
};

#endif
