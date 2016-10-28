#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

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
#include "Resource.h"

#include "Timer.h"

// TODO:
// The LoadResource function can not return a finished loaded thing, since we are going to multithread
// stack _toLoad;

class ResourceManager
{
private:
	typedef bool(*evfunc)(uint32_t, ResourceManager*);
public:




	static ResourceManager& Instance();
	
	const SM_GUID LoadResource(SM_GUID guid, const Resource::Flag& flag);
	void UnloadResource(SM_GUID guid);
	bool IsLoaded(SM_GUID guid);

	void TestAlloc( void );

	uint32_t FreeMemory(void) const { return _allocator->FreeMemory() + ((_resource.limit - _resource.count)*Resource::Size); }
	uint32_t MaxMemory(void) const { return _allocator->MaxMemory() + _resource.limit*Resource::Size; }
	const Resource::Ptr GetResource(const SM_GUID& guid);
	void ReturnResource(const Resource::Ptr& resource);


	void SetAssetLoader(IAssetLoader* loader);
	void AddParser(const std::string& fileend,const std::function<void(const Resource::Ptr& resource)>& parseFunction);
	void SetEvictPolicy(evfunc evictPolicy);
	void Init(uint64_t maxMemory);
	void ShutDown();
	void Startup(uint32_t numLoadingThreads, uint32_t numParsingThreads);

private:
	ResourceManager();
	~ResourceManager();
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& rhs) = delete;
	
	void _Run();
	void _LoadingThread();
	void _ParserThread();

	void _UpdatePriority(uint32_t index, const Resource::Flag& flag);

	//std::function<bool(uint32_t sizeOfLoadRequest, ResourceManager* rm) > _WhatToEvict;
	
	evfunc _WhatToEvict;
	public:
		struct EvictPolicies
		{
		public:
			static bool NoEvict(uint32_t sizeOfLoadRequest, ResourceManager* rm)
			{
				return false;
			}
			static bool FirstFit(uint32_t sizeOfLoadRequest, ResourceManager* rm);
			static bool FirstCumulativeFit(uint32_t sizeOfLoadRequest, ResourceManager* rm);
			static bool InstantEvict(uint32_t sizeOfLoadRequest, ResourceManager* rm);
			static bool LRU(uint32_t sizeOfLoadRequest, ResourceManager* rm);
			static bool MRU(uint32_t sizeOfLoadRequest, ResourceManager* rm);
		};
		
			
private:
	ChunkyAllocator* _allocator = nullptr;

	Resource _resource;

	std::queue<SM_GUID> _loadingQueueHighPrio;
	std::queue<SM_GUID> _loadingQueueLowPrio;

	std::queue<SM_GUID> _parserQueueHighPrio;
	std::queue<SM_GUID> _parserQueueLowPrio;

	IAssetLoader* _assetLoader = nullptr;

	bool _running;
	AssetParser _parser;

	std::vector<std::thread> _threads;
	std::mutex _mutexLockGeneral;
	std::mutex _mutexLockLoader;
	std::mutex _mutexLockLoadingQueue;
	std::mutex _mutexLockParserQueue;

	std::vector<uint32_t> _pinned;
	std::vector<std::pair<uint32_t&, uint32_t>> _defragList;

	Timer _timer;
};

#endif