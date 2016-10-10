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
	void UpdatePriority(SM_GUID guid, const Resource::Flag& flag);
	bool IsLoaded(SM_GUID guid);

	void PrintOccupancy( void );
	void TestAlloc( void );

	uint32_t FreeMemory( void ) const { return _numFreeBlocks * _blockSize + _resourcePool->FreeMemory(); }
	uint32_t MaxMemory( void ) const { return _numBlocks * _blockSize + _resourcePool->Size(); }

	void SetAssetLoader(IAssetLoader* loader);
	void AddParser(const std::string& fileend,const std::function<void(Resource& r)>& parseFunction);

	void ShutDown();
	void Startup();

private:
	struct FreeBlock
	{
		int32_t Previous = -1;
		int32_t Next = -1;
	};

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
	void _SetupFreeBlockList( void );
	int32_t _FindSuitableAllocationSlot( uint32_t blocks );
	void _Allocate( int32_t allocSlot, uint32_t blocks );
	void _Free( int32_t firstBlock, uint32_t numBlocks );

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
			rm->~ResourceList();

		}
		else
		{
			rm->prev->next = rm->next;

			if (rm->next)
				rm->next->prev = rm->prev;
			else
				_resources->prev = rm->prev;
			rm->~ResourceList();
		}

		for (auto &it : (rm->resource.observers))
		{
			it->NotifyDelete(rm->resource.ID);
		}
		
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
	char* _pool = nullptr;
	const uint32_t _blockSize = 512 * 1024;
	uint32_t _numBlocks = 0;
	uint32_t _numFreeBlocks = 0;
	int32_t _firstFreeBlock = -1;

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
	std::mutex _mutexAllocLock;
	//std::mutex _mutexLockResourceArr;
	std::mutex _mutexLockLoader;
	std::mutex _mutexLockParser;
	std::mutex _mutexLockLoadingQueue;
	std::mutex _mutexLockParserQueue;
};

#endif