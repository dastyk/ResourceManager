#include "ChunkyAllocator.h"

#include <stdio.h>
#include "MemoryManager.h"

using namespace std;

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

	FreeChunk* walker = _root->Next;

	// Keep trying until we find a free chunk with enough blocks in.
	while (walker != _end)
	{
		if (walker->Blocks >= blocks)
			break;

		walker = walker->Next;
	}

	if (walker == _end)
	{
		_allocLock.unlock();
		return -1;
	}

	// Given a valid allocation slot and number of blocks -- extract those blocks
	// from the list of free blocks and fix the broken links. In case the chunk
	// has more blocks than required, it's split to keep the remaining ones.
	FreeChunk* nextChunk;
	if (walker->Blocks > blocks)
	{
		nextChunk = reinterpret_cast<FreeChunk*>(reinterpret_cast<char*>(walker) + blocks * _blockSize);
		nextChunk->Blocks = walker->Blocks - blocks;
		nextChunk->Previous = walker->Previous;
		nextChunk->Next = walker->Next;
		walker->Previous->Next = nextChunk;
		walker->Next->Previous = nextChunk;
	}
	else
	{
		nextChunk = walker->Next;
		nextChunk->Previous = walker->Previous;
		walker->Previous->Next = nextChunk;
	}

	_numFreeBlocks -= blocks;

	_allocLock.unlock();

	// Convert pointer to slot index
	return (reinterpret_cast<char*>(walker) - _pool) / _blockSize;
}

// Frees certain blocks by inserting them into the free block list.
void ChunkyAllocator::Free(int32_t first, uint32_t numBlocks)
{
	_allocLock.lock();

	FreeChunk* returnChunk = reinterpret_cast<FreeChunk*>(_pool + first * _blockSize);
	returnChunk->Blocks = numBlocks;

	// We must find where in the list to insert and properly update the blocks
	// before and after those we are returning (if existing).
	FreeChunk* before = _root;
	while (before->Next != _end && before->Next < returnChunk)
	{
		before = before->Next;
	}
	FreeChunk* after = before->Next;

	// If we return blocks at the end of a chunk we merge it right away.
	if ((reinterpret_cast<char*>(before) + before->Blocks * _blockSize) == reinterpret_cast<char*>(returnChunk))
	{
		before->Blocks += numBlocks;
		returnChunk = before; // We do this to simplify potential two-way merge later
	}
	// Otherwise they must be linked together
	else
	{
		before->Next = returnChunk;
		returnChunk->Previous = before;
	}

	// If the end of the returned chunk coincides with the next free chunk we
	// can merge that way as well.
	if (after != _end && (reinterpret_cast<char*>(returnChunk) + returnChunk->Blocks * _blockSize) == reinterpret_cast<char*>(after))
	{
		returnChunk->Blocks += after->Blocks;
		returnChunk->Next = after->Next;
		returnChunk->Next->Previous = returnChunk;
	}
	// Otherwise they must be linked together
	else
	{
		returnChunk->Next = after;
		returnChunk->Next->Previous = returnChunk;
	}

	_numFreeBlocks += numBlocks;

	_allocLock.unlock();
}

// Performs one defragmentation iteration. Returns which index of the provided
// list was moved or -1 if none.
int32_t ChunkyAllocator::Defrag(list<pair<uint32_t, uint32_t>>& allocs)
{
	_allocLock.lock();

	FreeChunk* free = _root->Next;

	// No free chunks, nothing to defrag
	if (free == _end)
	{
		_allocLock.unlock();
		return -1;
	}

	int32_t changedIndex = 0;

	// Naïve defragmentation that just stuffs upcoming occupied blocks into the free one.
	for (auto& alloc : allocs)
	{
		// Found an allocation that comes after the free block (defrag left)
		if (reinterpret_cast<char*>(free) + free->Blocks * _blockSize == _pool + alloc.first * _blockSize)
		{
			// Make a copy of the old free block before overwriting data
			FreeChunk oldFree;
			oldFree = *free;

			// Copy data into the free chunk (memmove supports overlaps, so even
			// if the free chunk is just one block, we can still move 2 blocks)
			memmove(free, _pool + alloc.first * _blockSize, alloc.second * _blockSize);

			// Move start of free chunk to accomodate the newly inserted data and
			// incorporate the newly freed slots when we're at it
			FreeChunk* newFree = reinterpret_cast<FreeChunk*>(reinterpret_cast<char*>(free) + alloc.second * _blockSize);
			newFree->Blocks = oldFree.Blocks;
			newFree->Previous = oldFree.Previous;
			newFree->Next = oldFree.Next;
			newFree->Previous->Next = newFree;
			newFree->Next->Previous = newFree;

			// Allocation was moved to the start of the free chunk before, so its
			// alloc slot is shifted by the number of blocks of the free chunk.
			alloc.first -= newFree->Blocks;

			// If the next free block comes immediately after we merge
			if (newFree->Next == reinterpret_cast<FreeChunk*>(reinterpret_cast<char*>(newFree) + newFree->Blocks * _blockSize))
			{
				newFree->Blocks += newFree->Next->Blocks;
				newFree->Next = newFree->Next->Next;
				newFree->Next->Previous = newFree;
			}

			_allocLock.unlock();
			return changedIndex;
		}

		++changedIndex;
	}

	_allocLock.unlock();
	return -1;
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
	FreeChunk* free = _root->Next; // Should not be end

	// As long as walker hasn't reached the end
	while (walker < (_pool + _numBlocks * _blockSize))
	{
		if (reinterpret_cast<FreeChunk*>(walker) == free)
		{
			for (uint32_t i = 0; i < free->Blocks; ++i)
			{
				//OutputDebugStringA( "[O]" );
				printf("[O]");
			}

			walker += free->Blocks * _blockSize;
			free = free->Next;
		}
		else
		{
			//OutputDebugStringA( "[X]" );
			printf("[X]");
			walker += _blockSize;
		}
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

	FreeChunk* walker = reinterpret_cast<FreeChunk*>(_pool);
	walker->Blocks = _numBlocks;
	_root->Next = walker;
	walker->Previous = _root;
	walker->Next = _end;
	_end->Previous = walker;

	_allocLock.unlock();
}