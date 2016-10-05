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
#include <map>
// Flöjt TODO:
// Chunked allocation, i.e. make this a pool allocator with doubly linked list of
// free blocks. Allocation is a matter if going to first empty (ordered) and see if
// we can find enough blocks in a row to store a file.
// Use asset loaders and asset parsers with GUID to load.

// TODO:
// The LoadResource function can not return a finished loaded thing, since we are going to multithread
// stack _toLoad;




class ResourceManager
{
public:
	static ResourceManager& Instance();
	
	Resource& LoadResource(SM_GUID guid, const Resource::Flag& flag);

	void PrintOccupancy(void);
	void TestAlloc(void);
	
	void SetAssetLoader(IAssetLoader* loader);
	void AddParser(const std::string& fileend,const std::function<void(Resource& r)>& parseFunction);

	void ShutDown();

	

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
			a->_flags > b->_flags;
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


	void _Startup();
	void _LoadingThread(uint16_t threadID);
	void _ParserThread(uint16_t threadID);
	void _SetupFreeBlockList(void);
	int _FindSuitableAllocationSlot(uint32_t blocks);
	void _Allocate(int32_t allocSlot, uint32_t blocks);
	void _Free(int32_t firstBlock, uint32_t numBlocks);
	Resource* _FindResource(SM_GUID guid);
	
	PoolAllocator* _resourcePool;
	std::priority_queue<Resource*, std::vector<Resource*>, CompareResources> _loadingQueue;
	std::priority_queue<Resource*, std::vector<Resource*>, CompareResources> _parserQueue;
	std::unordered_map<uint16_t, ThreadControl, KeyHasher> _threadRunningMap;
	std::unordered_map<uint16_t, std::thread, KeyHasher> _threadIDMap;
	IAssetLoader* _assetLoader = nullptr;
	

	bool _running;
	AssetParser _parser;
	char* _pool = nullptr;
	const uint32_t _blockSize = 512 * 1024;
	uint32_t _numBlocks = 0;
	std::thread _runningThread;
	int32_t _firstFreeBlock = -1;
	std::mutex _mutexLockGeneral;
	std::mutex _mutexLockResourceArr;
	std::mutex _mutexLockLoader;
	std::mutex _mutexLockParser;
	std::mutex _mutexLockLoadingQueue;
	std::mutex _mutexLockParserQueue;
};

#endif