#ifndef _CHUNKY_POOL_ALLOCATOR_H_
#define _CHUNKY_POOL_ALLOCATOR_H_

#pragma once
#include <stdint.h>

class ChunkyPoolAllocator
{
private:
	struct FreeBlock
	{
		int32_t Previous = -1;
		int32_t Next = -1;
	};

	char* _pool = nullptr;
	const uint32_t _blockSize = 512 * 1024;
	uint32_t _numBlocks = 0;
	int32_t _firstFreeBlock = -1;

	void _SetupFreeBlockList(void);
public:
	ChunkyPoolAllocator();
	~ChunkyPoolAllocator();



	void PrintOccupancy(void);
	void TestAlloc(void);

	
	int32_t FindSuitableAllocationSlot(uint32_t size);
	void Allocate(int32_t allocSlot, uint32_t size);
	void Free(int32_t firstBlock, uint32_t size);
};

#endif