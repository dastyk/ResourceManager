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

	// Empty
	if (_root->Next == _end)
	{
		_allocLock.unlock();
		return -1;
	}

	FreeBlock* allocSlot = _root->Next;
	FreeBlock* walker = _root->Next;
	uint32_t numContiguous = 1;

	// Keep searching until we found enough blocks. The case of not enough
	// contiguous blocks is taken care of inside the loop.
	while (numContiguous < blocks)
	{
		if (walker->Next == _end)
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
	// from the list of free blocks and fix the broken links.
	FreeBlock* lastBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(allocSlot) + (blocks - 1) * _blockSize);
	allocSlot->Previous->Next = lastBlock->Next;
	lastBlock->Next->Previous = allocSlot->Previous;

	_numFreeBlocks -= blocks;

	_allocLock.unlock();

	// Convert pointer to slot index
	return (reinterpret_cast<char*>(allocSlot) - _pool) / _blockSize;
}

// Frees certain blocks by inserting them into the free block list.
void ChunkyAllocator::Free(int32_t first, uint32_t numBlocks)
{
	_allocLock.lock();

	FreeBlock* firstBlock = reinterpret_cast<FreeBlock*>(_pool + first * _blockSize);
	FreeBlock* lastBlock = reinterpret_cast<FreeBlock*>(_pool + (first + numBlocks - 1) * _blockSize);

	// We must find where in the list to insert and properly update the blocks
	// before and after those we are returning (if existing).
	FreeBlock* before = _root;
	while (before->Next != _end && before->Next < firstBlock)
	{
		before = before->Next;
	}

	// Could assert that this pointer is indeed larger than lastBlock or _end.
	FreeBlock* after = before->Next;

	// Setup links for the first returned block
	before->Next = firstBlock;
	firstBlock->Previous = before;

	// Chain the returned blocks together
	FreeBlock* walker = firstBlock;
	while (walker != lastBlock)
	{
		walker->Next = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(walker) + _blockSize);
		walker->Next->Previous = walker;
		walker = walker->Next;
	}

	// Setup links for the last returned block (walker is lastBlock)
	walker->Next = after;
	after->Previous = walker;

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
	FreeBlock* free = _root->Next; // Should not be end

	// As long as walker hasn't reached the end
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

	_root->Next = _end;
	_root->Previous = nullptr;
	_end->Next = nullptr;
	_end->Previous = _root;

	if (_numBlocks == 0)
		return;

	// Link to first block
	FreeBlock* walker = reinterpret_cast<FreeBlock*>(_pool);
	_root->Next = walker;
	walker->Previous = _root;

	// Link blocks together
	for (uint32_t i = 1; i < _numBlocks; ++i)
	{
		walker->Next = reinterpret_cast<FreeBlock*>(_pool + i * _blockSize);
		walker->Next->Previous = walker;
		walker = walker->Next;
	}

	// Links to last block
	walker->Next = _end;
	_end->Previous = walker;

	_allocLock.unlock();
}