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
	
	Resource& LoadResource(SM_GUID guid, const Resource::Flag& flag);
	void UnloadResource(SM_GUID guid);
	void EvictResource(SM_GUID guid);
	void UpdatePriority(SM_GUID guid, const Resource::Flag& flag);
	bool IsLoaded(SM_GUID guid);

	void PrintOccupancy( void );
	void TestAlloc( void );

	uint32_t FreeMemory( void ) const { return _numFreeBlocks * _blockSize; }
	uint32_t MaxMemory( void ) const { return _numBlocks * _blockSize; }
	
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
	
	Resource* _FindResource(SM_GUID guid);

private:
	char* _pool = nullptr;
	const uint32_t _blockSize = 512 * 1024;
	uint32_t _numBlocks = 0;
	uint32_t _numFreeBlocks = 0;
	int32_t _firstFreeBlock = -1;
	
	PoolAllocator* _resourcePool;
	std::map<uint64_t, Resource*> _resources;
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