#ifndef _CHUNKY_ALLOCATOR_H_
#define _CHUNKY_ALLOCATOR_H_

#include <stdint.h>
#include <mutex>

class ChunkyAllocator
{
public:
	ChunkyAllocator(uint32_t numBlocks);
	~ChunkyAllocator();

	int32_t Allocate(uint32_t blocks);
	void Free(int32_t firstBlock, uint32_t numBlocks);

	static uint32_t BlockSize(void) { return _blockSize; }
	char* Data(uint32_t block) { return  _pool + block * _blockSize; }

	uint32_t FreeMemory(void) const { return _numFreeBlocks * _blockSize; }
	uint32_t MaxMemory(void) const { return _numBlocks * _blockSize; }

	void PrintOccupancy(void);

private:
	struct FreeBlock
	{
		FreeBlock* Previous = nullptr;
		FreeBlock* Next = nullptr;
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

	FreeBlock _rootBlock, _endBlock; // Really just for stack storage, I actually use the pointers instead
	FreeBlock* _root = &_rootBlock;
	FreeBlock* _end = &_endBlock;

	std::mutex _allocLock;
};

#endif // !_CHUNKY_ALLOCATOR_H_