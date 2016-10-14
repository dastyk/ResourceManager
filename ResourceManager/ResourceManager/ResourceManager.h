#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "Resource.h"
#include <vector>
#include <thread>
#include <queue>
#include <unordered_map>
#include <mutex>
#include "AssetParser.h"
#include "IAssetLoader.h"
#include "MemoryManager.h"
#include <stack>
#include <map>
#include "ChunkyAllocator.h"

// TODO:
// The LoadResource function can not return a finished loaded thing, since we are going to multithread
// stack _toLoad;

class ResourceManager
{
public:
	static ResourceManager& Instance();
	
	const SM_GUID LoadResource(SM_GUID guid, const Resource::Flag& flag);
	void UnloadResource(SM_GUID guid);
	void EvictResource(SM_GUID guid);
	bool IsLoaded(SM_GUID guid);

	void TestAlloc( void );

	uint32_t FreeMemory(void) const { return _allocator->FreeMemory() + _resourcePool->FreeMemory(); }
	uint32_t MaxMemory(void) const { return _allocator->MaxMemory() + _resourcePool->Size(); }

	void SetAssetLoader(IAssetLoader* loader);
	void AddParser(const std::string& fileend,const std::function<void(Resource& r)>& parseFunction);

	void Init(uint64_t maxMemory);
	void ShutDown();
	void Startup();

private:
	struct ThreadControl
	{
		bool inUse = false;
		bool beenJoined = true;
	};

	class CompareResources
	{
	public:
		bool operator()(Resource *a, Resource *b) const
		{
			return a->_flags < b->_flags;
		}
	};
	
	struct KeyHasher
	{
		std::size_t operator()(const uint16_t &a) const
		{
			return (size_t)a;
		}
	};

private:
	ResourceManager();
	~ResourceManager();
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& rhs) = delete;
	
	void _Run();

	void _LoadingThread(uint16_t threadID);
	void _ParserThread(uint16_t threadID);
	void _UpdatePriority(SM_GUID guid, const Resource::Flag& flag);

	struct ResourceList
	{
		ResourceList(SM_GUID guid, const Resource::Flag& flag) : resource(guid, flag), prev(nullptr), next(nullptr)
		{

		}
		ResourceList* prev;
		Resource resource;
		ResourceList* next;
	};

	ResourceList* _CreateResource(SM_GUID guid, const Resource::Flag& flag)
	{
		if (_resources)
		{
			auto& newR = _resources->prev->next = (ResourceList*)_resourcePool->Malloc();
			new (_resources->prev->next) ResourceList(guid, flag);
			_resources->prev->next->prev = _resources->prev;
			_resources->prev = _resources->prev->next;
			return newR;
		}
		else
		{
			_resources = (ResourceList*)_resourcePool->Malloc();
			new (_resources) ResourceList(guid, flag);
			_resources->prev = _resources;
			return _resources;
		}
	}

	ResourceList* _FindResource(SM_GUID guid)
	{
		auto n = _resources;
		while (n)
		{
			if (n->resource.ID == guid)
			{
				return n;
			}
			n = n->next;
		}
		return nullptr;

	}

	void _RemoveResource(ResourceList* rm)
	{


		if (rm == _resources)
		{
			if(_resources->next)
				_resources->next->prev = _resources->prev;
			_resources = _resources->next;		
		}
		else
		{
			rm->prev->next = rm->next;

			if (rm->next)
				rm->next->prev = rm->prev;
			else
				_resources->prev = rm->prev;
		
		}

		uint32_t startBlock = rm->resource._startBlock;
		uint32_t numBlocks = rm->resource._numBlocks;
		rm->~ResourceList();

		_allocator->Free(startBlock, numBlocks);
		_resourcePool->Free(rm);
	}

	void _RemoveAllResources()
	{
		auto r = _resources;
		while (r)
		{
			r->~ResourceList();
			r = r->next;
		}
		_resources = nullptr;
	}
private:
	ChunkyAllocator* _allocator = nullptr;
	PoolAllocator* _resourcePool = nullptr;
	ResourceList* _resources = nullptr;
	//std::map<uint64_t, Resource*> _resources;
	std::priority_queue<Resource*, std::vector<Resource*>, CompareResources> _loadingQueue;
	std::priority_queue<Resource*, std::vector<Resource*>, CompareResources> _parserQueue;
	std::unordered_map<uint16_t, ThreadControl, KeyHasher> _threadRunningMap;
	std::unordered_map<uint16_t, std::thread, KeyHasher> _threadIDMap;
	IAssetLoader* _assetLoader = nullptr;

	bool _running;
	AssetParser _parser;

	std::thread _runningThread;
	std::mutex _mutexLockGeneral;
	//std::mutex _mutexLockResourceArr;
	std::mutex _mutexLockLoader;
	std::mutex _mutexLockParser;
	std::mutex _mutexLockLoadingQueue;
	std::mutex _mutexLockParserQueue;
};

#endif