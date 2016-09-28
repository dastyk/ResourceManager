#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include "PoolAllocator.h"
#include "StackAllocator.h"
#include <mutex>
#include <map>
#include <string>
#define POOL_ALLOCATOR_ALIGNED 1 << 0
#define POOL_ALLOCATOR_UNALIGNED 1 << 1

class MemoryManager
{
public:
	MemoryManager(uint32_t size);
	~MemoryManager();
private:
	std::mutex _mutexLock;
	char* _memory;
	char* _free;

	uint64_t _remainingMemory;

	std::map<void*, size_t> _allocatedBlocks; //maps address to size of block

	struct BlockInfo
	{
		BlockInfo(size_t size, BlockInfo* next = nullptr) : size(size), next(next) {}
		size_t size;
		BlockInfo* next;
	};

	BlockInfo* _firstFree;

	void* _Allocate(const size_t size);


	void _Free(void* address, const size_t size);




public:

	PoolAllocator* CreatePoolAllocator(uint32_t sizeOfObject, uint32_t nrOfObjects, uint32_t alignment);
	StackAllocator* CreateStackAllocator(uint64_t size);

	void ReleasePoolAllocator(PoolAllocator* object);
	void ReleaseStackAllocator(StackAllocator* object);

	//DEBUG
	void PrintBlockInfo();

};








#endif
