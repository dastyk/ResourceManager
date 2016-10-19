#ifndef _CHUNKY_ALLOCATOR_H_
#define _CHUNKY_ALLOCATOR_H_

#include <stdint.h>
#include <mutex>
#include <list>

class ChunkyAllocator
{
public:
	ChunkyAllocator(uint32_t numBlocks);
	~ChunkyAllocator();

	int32_t Allocate(uint32_t blocks);
	void Free(int32_t firstBlock, uint32_t numBlocks);
	bool Defrag(std::list<std::pair<uint32_t&, uint32_t>>& allocs);

	static uint32_t BlockSize(void) { return _blockSize; }
	char* Data(uint32_t block) { return  _pool + block * _blockSize; }

	uint32_t FreeMemory(void) const { return _numFreeBlocks * _blockSize; }
	uint32_t MaxMemory(void) const { return _numBlocks * _blockSize; }

	void PrintOccupancy(void);

private:
	struct FreeChunk
	{
		FreeChunk* Previous = nullptr;
		FreeChunk* Next = nullptr;
		uint32_t Blocks = 0;
	};

private:
	ChunkyAllocator(const ChunkyAllocator& other) = delete;
	ChunkyAllocator& operator=(const ChunkyAllocator& rhs) = delete;

	void _SetupFreeBlockList(void);

private:
	char* _pool = nullptr;
	static const uint32_t _blockSize = 512 * 1024;
	uint32_t _numBlocks = 0;
	uint32_t _numFreeBlocks = 0;

	FreeChunk _rootChunk, _endChunk; // Really just for stack storage, I actually use the pointers instead
	FreeChunk* _root = &_rootChunk;
	FreeChunk* _end = &_endChunk;

	std::mutex _allocLock;
};

#endif // !_CHUNKY_ALLOCATOR_H_
