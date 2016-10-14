#include "ChunkyAllocator.h"

#include <stdio.h>
#include "MemoryManager.h"

ChunkyAllocator::ChunkyAllocator(uint32_t blocks)
{
	_numFreeBlocks = _numBlocks = blocks;
	_pool = (char*)MemoryManager::Alloc(_numBlocks * _blockSize);

	// Make blocks form a linked list (all of them at startup)
	_SetupFreeBlockList();
}

ChunkyAllocator::~ChunkyAllocator()
{
	MemoryManager::Release(_pool);
}

// Returns the slot that was allocated at. If no suitable slot was found, -1 is returned.
int32_t ChunkyAllocator::Allocate(uint32_t blocks)
{
	// This first part finds a suitable allocation slot

	_allocLock.lock();

	if (!_firstFreeBlock)
	{
		_allocLock.unlock();
		return -1;
	}

	FreeBlock* allocSlot = _firstFreeBlock;
	FreeBlock* walker = _firstFreeBlock;
	uint32_t numContiguous = 1;

	// Keep searching until we found enough blocks. The case of not enough
	// contiguous blocks is taken care of inside the loop.
	while (numContiguous < blocks)
	{
		if (!walker->Next)
		{
			_allocLock.unlock();
			return -1;
		}

		// Not next contiguous block; reset and keep trying
		if (reinterpret_cast<char*>(walker->Next) != reinterpret_cast<char*>(walker) + _blockSize)
		{
			allocSlot = walker = walker->Next;
			numContiguous = 1;
		}
		// Contiguous
		else
		{
			walker = walker->Next;
			numContiguous++;
		}
	}

	// Given a valid allocation slot and number of blocks -- extract those blocks
	// from the list of free blocks.

	if (!allocSlot)
	{
		_allocLock.unlock();
		return -1;
	}

	if (allocSlot == _firstFreeBlock)
	{
		// The last of the blocks to allocate (offset allocSlot correct number of
		// bytes and treat as FreeBlock*).
		FreeBlock* lastBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(allocSlot) + (blocks - 1) * _blockSize);

		// Index of the block after the last one to allocate
		_firstFreeBlock = lastBlock->Next;

		if (_firstFreeBlock)
			_firstFreeBlock->Previous = nullptr;
	}
	else
	{
		FreeBlock* firstToAlloc = allocSlot;
		FreeBlock* lastToAlloc = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(allocSlot) + (blocks - 1) * _blockSize);
		FreeBlock* nextFree = lastToAlloc->Next;
		firstToAlloc->Previous->Next = nextFree;

		if (nextFree)
			nextFree->Previous = firstToAlloc->Previous;
	}

	_numFreeBlocks -= blocks;

	_allocLock.unlock();

	// Convert pointer to slot index
	return (reinterpret_cast<char*>(allocSlot) - _pool) / _blockSize;
}

// Frees certain blocks by inserting them into the free block list.
void ChunkyAllocator::Free(int32_t firstBlock, uint32_t numBlocks)
{
	_allocLock.lock();

	// If there is no list to insert into, just make the current ones the new list.
	if (!_firstFreeBlock)
	{
		// Set up previous link on the first block
		_firstFreeBlock = reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize);
		FreeBlock* b = _firstFreeBlock;
		b->Previous = nullptr;

		// For any other blocks we correctly set up the next link BEFORE actually
		// getting the next block, after which we set up the previous link.
		for (uint32_t block = firstBlock + 1; block < firstBlock + numBlocks; ++block)
		{
			b->Next = reinterpret_cast<FreeBlock*>(_pool + block * _blockSize);
			b->Next->Previous = b;
			b = b->Next;
		}

		// The last block will now indicate the end of the list
		b->Next = nullptr;
	}
	// First free block is after the ones we are returning
	else if (_firstFreeBlock > reinterpret_cast<FreeBlock*>(_pool + (firstBlock + numBlocks - 1) * _blockSize))
	{
		FreeBlock* oldFirst = _firstFreeBlock;

		// Insert at head of list (like above), with the exception that the last
		// block points to the former first free block.
		_firstFreeBlock = reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize);

		// Set up previous link on the first block
		FreeBlock* b = _firstFreeBlock;
		b->Previous = nullptr;

		// For any other blocks we correctly set up the next link BEFORE actually
		// getting the next block, after which we set up the previous link.
		for (uint32_t block = firstBlock + 1; block < firstBlock + numBlocks; ++block)
		{
			b->Next = reinterpret_cast<FreeBlock*>(_pool + block * _blockSize);
			b->Next->Previous = b;
			b = b->Next;
		}

		// The last block will now point to the former head of the list
		b->Next = oldFirst;

		// Old first block has its previous pointer set to the last freed block
		oldFirst->Previous = reinterpret_cast<FreeBlock*>(_pool + (firstBlock + numBlocks - 1) * _blockSize);
	}
	// Existing free blocks before the ones we are returning. This means that
	// we must find where in the list to insert and properly update the blocks
	// before and after (if existing).
	else
	{
		FreeBlock* lastFreeBlock = _firstFreeBlock;
		FreeBlock* currFreeBlock = _firstFreeBlock;

		// Keep trying until we found correct spot (where the slot changed from immediately before to after)
		while (currFreeBlock < reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize)) // Less than comparison should be valid for entire range due to allocation list housekeeping
		{
			lastFreeBlock = currFreeBlock;

			currFreeBlock = lastFreeBlock->Next;

			if (!currFreeBlock)
				break;
		}

		lastFreeBlock->Next = reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize);

		// Set up previous link on the first block
		FreeBlock* b = lastFreeBlock->Next;
		b->Previous = lastFreeBlock;

		// For any other blocks we correctly set up the next link BEFORE actually
		// getting the next block, after which we set up the previous link.
		for (uint32_t block = firstBlock + 1; block < firstBlock + numBlocks; ++block)
		{
			b->Next = reinterpret_cast<FreeBlock*>(_pool + block * _blockSize);
			b->Next->Previous = b;
			b = b->Next;
		}

		// Link to the next block (will be nullptr as appropriate if end of pool)
		b->Next = currFreeBlock;

		// If we have another block, get it to link to the last returned block
		if (currFreeBlock)
			currFreeBlock->Previous = reinterpret_cast<FreeBlock*>(_pool + (firstBlock + numBlocks - 1) * _blockSize);
	}

	_numFreeBlocks += numBlocks;

	_allocLock.unlock();
}

// Little debug function that outputs occupancy of blocks where O indicates open
// and X indicates occupied.
void ChunkyAllocator::PrintOccupancy(void)
{
	if (_numBlocks == 0)
	{
		return;
	}

	char* walker = _pool;
	FreeBlock* free = _firstFreeBlock;

	// As long as walker hasn't reached the ned
	while (walker < (_pool + _numBlocks * _blockSize))
	{
		if (reinterpret_cast<FreeBlock*>(walker) == free)
		{
			free = free->Next;

			//OutputDebugStringA( "[O]" );
			printf("[O]");
		}
		else
		{
			//OutputDebugStringA( "[X]" );
			printf("[X]");
		}

		walker += _blockSize;
	}

	//OutputDebugStringA( "\n" );
	printf("\n");
}

void ChunkyAllocator::_SetupFreeBlockList(void)
{
	_allocLock.lock();

	// Iterate through blocks (all are free at first) and reinterpret them
	// as the FreeBlock struct in order to set their previous and next ref-
	// erences correctly in order to create a doubly linked list.

	if (_numBlocks == 0)
	{
		_firstFreeBlock = nullptr;
	}
	// Special case because both previous and next are non-existant.
	else if (_numBlocks == 1)
	{
		FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool);
		b->Previous = nullptr;
		b->Next = nullptr;
		_firstFreeBlock = b;
	}
	else
	{
		_firstFreeBlock = reinterpret_cast<FreeBlock*>(_pool);
		FreeBlock* b = _firstFreeBlock;
		b->Previous = nullptr;

		for (uint32_t i = 1; i < _numBlocks; ++i)
		{
			b->Next = reinterpret_cast<FreeBlock*>(_pool + i * _blockSize);
			b->Next->Previous = b;
			b = b->Next;
		}

		b->Next = nullptr;
	}

	_allocLock.unlock();
}