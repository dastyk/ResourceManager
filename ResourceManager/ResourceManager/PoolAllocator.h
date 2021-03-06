#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#pragma warning (disable : 4267) //conversion from size_t to uint32_t
#pragma warning (disable : 4244) //conversion from __int64 to int32_t

#include <memory>
#include <stdexcept>
#include <string>
class PoolAllocator
{
private:
	struct List
	{
		int32_t next;
	};

public:
	PoolAllocator(char* poolStart, size_t blocksize, size_t numBlocks);
	~PoolAllocator();

	inline void* Malloc()
	{
		if (!_freeBlockList) throw std::runtime_error("Pool allocator out of memory.");

      // Set the next free block to be the second one in the list and return
      // the original free block (effectively consuming the first block).
		char* ret = _freeBlockList;
		if (((List*)_freeBlockList)->next >= 0)
			_freeBlockList = _pool + ((List*)_freeBlockList)->next;
		else
			_freeBlockList = nullptr;
		_freeBlocks--;
		return ret;
	}

	inline void Free(void*p)
	{
		if (!p) throw std::runtime_error("Free on nullptr");

      // When freeing a block, we put it first in the list of empty blocks.
      // To not sever the link we make it point to the previous head.
		if (_freeBlockList)
		{
			char* prev = _freeBlockList;
			_freeBlockList = (char*)p;
			((List*)_freeBlockList)->next = prev - _pool;
		}
		else
		{
			_freeBlockList = (char*)p;
			((List*)_freeBlockList)->next = -1;
		}
		_freeBlocks++;
	}

	inline size_t Size()const
	{
		return _numBlocks * _blockSize;
	}

	uint32_t FreeMemory()const
	{
		return _freeBlocks*_blockSize;
	}

	inline size_t BlockSize()const
	{
		return _blockSize;
	}

private:
	void _SetupFreeBlocks();

private:
	char* _pool;
   char* _freeBlockList;
	size_t _numBlocks;
	size_t _blockSize;
	size_t _freeBlocks;
};

#endif //POOL_ALLOCATOR_H
