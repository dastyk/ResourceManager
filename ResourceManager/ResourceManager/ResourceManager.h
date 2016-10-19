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
// TODO:
// The LoadResource function can not return a finished loaded thing, since we are going to multithread
// stack _toLoad;

class ResourceManager
{
public:




	static ResourceManager& Instance();
	
	const SM_GUID LoadResource(SM_GUID guid, const Resource::Flag& flag);
	void UnloadResource(SM_GUID guid);
	bool IsLoaded(SM_GUID guid);

	void TestAlloc( void );

	uint32_t FreeMemory(void) const { return _allocator->FreeMemory() + ((_resource.limit - _resource.count)*Resource::Size); }
	uint32_t MaxMemory(void) const { return _allocator->MaxMemory() + _resource.limit*Resource::Size; }

	void SetAssetLoader(IAssetLoader* loader);
	void AddParser(const std::string& fileend,const std::function<void(Resource::Ptr& resource)>& parseFunction);
	void SetEvictPolicy(const std::function<bool(uint32_t sizeOfLoadRequest, ResourceManager* rm)>& evictPolicy);
	void Init(uint64_t maxMemory);
	void ShutDown();
	void Startup();

private:

	struct ThreadControl
	{
		bool inUse = false;
		bool beenJoined = true;
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
	//void _UpdatePriority(SM_GUID guid, const Resource::Flag& flag);

	std::function<bool(uint32_t sizeOfLoadRequest, ResourceManager* rm) > _WhatToEvict;

	public:
		struct EvictPolicies
		{
		public:
			static bool NoEvict(uint32_t sizeOfLoadRequest, ResourceManager* rm)
			{
				return false;
			}
			static bool FirstFit(uint32_t sizeOfLoadRequest, ResourceManager* rm)
			{
				uint32_t count = rm->_resource.count;
				auto& data = rm->_resource.data;
				for (uint32_t i = 0; i < count; i++)
				{
					bool pinned = data.pinned[i].try_lock();
					if (pinned && data.refCount[i] == 0 && data.numBlocks[i] >= sizeOfLoadRequest)
					{
						rm->_resource.Modify();
						rm->_allocator->Free(data.startBlock[i], data.numBlocks[i]);
						rm->_resource.Remove(i);		
						return true;
					}
				}
				return false;
			}
			//static bool FirstCumulativeFit(uint32_t sizeOfLoadRequest, ResourceManager* rm)
			//{
			//	uint32_t foundTot = 0;
			//	std::vector<ResourceList*> found;
			//	auto r = rm->_resources;
			//	while (r)
			//	{
			//		if (r->resource._flags != Resource::Flag::PERSISTENT && r->resource._refCount == 0 && r->resource.GetState() == Resource::ResourceState::Loaded)
			//		{
			//			if (r->resource._numBlocks >= sizeOfLoadRequest)
			//			{
			//				printf("\tEvicting resource, GUID: %llu.\n\n", r->resource.ID.data);
			//				rm->_RemoveResource(r, rm->_resources);
			//				return true;
			//			}
			//			else
			//			{
			//			found.push_back(r);
			//			foundTot += r->resource.GetData().size;
			//			}
			//		}

			//		if (foundTot >= sizeOfLoadRequest)
			//		{
			//			for (auto re : found)
			//				rm->_RemoveResource(re, rm->_resources);
			//			// IF THIS HAPPENS YOU MUST DEFRAG!!!!
			//			//rm->_allocator->Defrag()
			//			return true;
			//		}
			//		r = r->next;
			//	}

			//	return false;
			//}
			//static bool LRU(uint32_t sizeOfLoadRequest, ResourceManager* rm)
			//{
			//	auto found = std::pair<uint64_t, ResourceList*>(UINT64_MAX, nullptr);
			//	auto r = rm->_resources;
			//	while (r)
			//	{
			//		if (r->resource._flags != Resource::Flag::PERSISTENT && r->resource._refCount == 0 && r->resource.GetState() == Resource::ResourceState::Loaded )
			//		{
			//			//if (found.first < r->resource.TimeStamp)
			//			{
			//				//found.first = r->resource.TimeStamp;
			//				found.second = r;
			//			}
			//		}
			//		r = r->next;
			//	}

			//	if (found.second)
			//	{
			//		printf("\tEvicting resource, GUID: %llu.\n\n", r->resource.ID.data);
			//		rm->_RemoveResource(found.second, rm->_resources);
			//		return true;
			//	}

			//	return false;
			//}
			//static bool MRU(uint32_t sizeOfLoadRequest, ResourceManager* rm)
			//{
			//	auto found = std::pair<uint64_t, ResourceList*>(0, nullptr);
			//	auto r = rm->_resources;
			//	while (r)
			//	{
			//		if (r->resource._flags != Resource::Flag::PERSISTENT && r->resource._refCount == 0 && r->resource.GetState() == Resource::ResourceState::Loaded)
			//		{
			//			//if (found.first > r->resource.TimeStamp)
			//			{
			//				//found.first = r->resource.TimeStamp;
			//				found.second = r;
			//			}
			//		}
			//		r = r->next;
			//	}

			//	if (found.second)
			//	{
			//		printf("\tEvicting resource, GUID: %llu.\n\n", r->resource.ID.data);
			//		rm->_RemoveResource(found.second, rm->_resources);
			//		return true;
			//	}

			//	return false;
			//}
		};
		
			
private:
	ChunkyAllocator* _allocator = nullptr;

	Resource _resource;

	std::priority_queue<uint32_t, std::vector<uint32_t>, std::less<uint32_t>> _loadingQueue;
	std::priority_queue<uint32_t, std::vector<uint32_t>, std::less<uint32_t>> _parserQueue;
	std::unordered_map<uint16_t, ThreadControl, KeyHasher> _threadRunningMap;
	std::unordered_map<uint16_t, std::thread, KeyHasher> _threadIDMap;
	IAssetLoader* _assetLoader = nullptr;

	bool _running;
	AssetParser _parser;

	std::thread _runningThread;
	std::mutex _mutexLockGeneral;
	std::mutex _mutexLockLoader;
	std::mutex _mutexLockLoadingQueue;
	std::mutex _mutexLockParserQueue;
};

#endif