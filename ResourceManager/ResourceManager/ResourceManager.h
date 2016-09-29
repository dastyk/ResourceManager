#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "Resource.h"
#include <vector>


// Fl�jt TODO:
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
	static void CreateInstance();
	static void DeleteInstance();
	static Resource& LoadResource(SM_GUID guid, const Resource::Flag& flag);

	static void PrintOccupancy(void);

private:
	struct FreeBlock
	{
		int32_t Previous = -1;
		int32_t Next = -1;
	};

private:
	ResourceManager();
	~ResourceManager();
	ResourceManager(const ResourceManager& other);
	ResourceManager& operator=(const ResourceManager& rhs);

	void _SetupFreeBlockList(void);

private:
	static ResourceManager* _instance;
	std::vector<Resource> _resources;

	char* _pool;
	const uint32_t _blockSize = 512 * 1024;
	uint32_t _numBlocks = 0;
	
	int32_t _firstFreeBlock = -1;
};

#endif