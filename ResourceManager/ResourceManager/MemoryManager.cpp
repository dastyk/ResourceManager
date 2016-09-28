#include "MemoryManager.h"
#include <iostream>
#include <string.h>

MemoryManager::MemoryManager(uint32_t size)
{

	_free = _memory = new char[size];
	memset(_free, 0, size);
	
	_remainingMemory = size;
	_firstFree = (BlockInfo*)_free;
	_firstFree->size = size;
	_firstFree->next = nullptr;
}

MemoryManager::~MemoryManager()
{
	delete[] _memory;
}

void * MemoryManager::_Allocate(const size_t size)
{
	BlockInfo* freeBlock = _firstFree;
	BlockInfo* previous = nullptr;
	while (freeBlock->size < size + sizeof(BlockInfo) && freeBlock->next != nullptr)
	{
		previous = freeBlock;
		freeBlock = freeBlock->next;
	}
	if (freeBlock->size > size + sizeof(BlockInfo))
	{
		if (freeBlock == _firstFree)
		{
			BlockInfo old = *_firstFree;
			_firstFree = (BlockInfo*)((char*)_firstFree + size);
			_firstFree->size = old.size - size;
			_firstFree->next = old.next;
		}
		else
		{
			previous->next = (BlockInfo*)((char*)freeBlock + size);
			previous->next->size = freeBlock->size - size;
			previous->next->next = freeBlock->next;
		}
		return freeBlock;
	}
	throw std::runtime_error("Out of memory");
}

void MemoryManager::_Free(void * address, const size_t size)
{
	//Check if block to be freed has any free "neighbours"
	BlockInfo* left = nullptr;
	BlockInfo* right = nullptr;
	BlockInfo* free = _firstFree;
	while (free)
	{
		if ((char*)free + free->size == address)
			left = free;
		else if ((char*)free == (char*)address + size)
			right = free;
		free = free->next;
	}
	if (left && right)
	{
		left->size += size + right->size;
		if (left->next == right)
			left->next = right->next;
		if (right->next == left)
		{
			BlockInfo* b = _firstFree;
			if (right == _firstFree)
			{
				_firstFree = left;
			}
			else
			{
				while (b->next && b->next != right)
					b = b->next;
				b->next = left;
			}
		}
	}
	else if (left)
	{
		left->size += size;
	}
	else if (right)
	{
		//Find which block points to "right" and adjust it
		BlockInfo* b = _firstFree;
		if (right == _firstFree)
		{
			_firstFree = (BlockInfo*)((char*)_firstFree - size);
			_firstFree->size = b->size + size;
			_firstFree->next = b->next;
		}
		else
		{
			while (b->next && b->next != right)
			{
				b = b->next;
			}
			b->next = (BlockInfo*)((char*)right - size);
			b->next->next = right->next;
			b->next->size = right->size + size;

		}
	}
	else
	{
		//Find last free block and point it to this address
		BlockInfo* here = (BlockInfo*)address;
		here->next = nullptr;
		here->size = size;
		BlockInfo* iter = _firstFree;
		while (iter->next != nullptr)
			iter = iter->next;
		iter->next = here;
	}
}

// alignment is the desired alignment, i.e. blocks will start on a multiple of this.
// For example, one might want to store structs of size 136 (perhaps two matrices
// and two floats) with alignment 64 for the matrices. A value of zero means that
// we don't care about alignment.
PoolAllocator * MemoryManager::CreatePoolAllocator(uint32_t sizeOfObject, uint32_t nrOfObjects, uint32_t alignment)
{
	// Always use minimum size of 8 due to pointers in pool :/
	if (sizeOfObject < 4)
		sizeOfObject = 4;

	if (alignment)
	{
		_mutexLock.lock();

		// Use a block size that is the smallest multiple of the desired alignment
		// greater than or equal to the block size.
		uint32_t trailing = sizeOfObject % alignment;
		uint32_t effectiveBlockSize = sizeOfObject;
		if (trailing != 0)
		{
			// Enlarge to next multiple of alignment
			effectiveBlockSize += (alignment - trailing);
		}

		//char* rawAddress = _free + sizeof(PoolAllocator);
		char* rawAddress = (char*)_Allocate(effectiveBlockSize * (nrOfObjects + 1) + sizeof(PoolAllocator));
		uint64_t mask = alignment - 1;
		uint64_t misalignment = mask & (size_t)(rawAddress + sizeof(PoolAllocator));
		uint64_t adjustment = alignment - misalignment;
		uint64_t requirement = effectiveBlockSize * (nrOfObjects + 1) + sizeof(PoolAllocator);
		if (_remainingMemory < (requirement))
		{
			throw std::runtime_error("Not enough memory");
		}
		_remainingMemory -= requirement;
		char* alignedAddress = rawAddress + adjustment + sizeof(PoolAllocator);
		
		PoolAllocator* pool = new(rawAddress) PoolAllocator(alignedAddress, effectiveBlockSize, nrOfObjects);
		_allocatedBlocks[rawAddress] = effectiveBlockSize * (nrOfObjects + 1) + sizeof(PoolAllocator);

		_mutexLock.unlock();

		return pool;
	}
	else
	{
		_mutexLock.lock();
		char* rawAdd = (char*)_Allocate(sizeOfObject * nrOfObjects + sizeof(PoolAllocator));
		PoolAllocator* pool = new(rawAdd) PoolAllocator(rawAdd + sizeof(PoolAllocator), sizeOfObject, nrOfObjects);
		_allocatedBlocks[rawAdd] = sizeof(PoolAllocator) + sizeOfObject * nrOfObjects;
		_mutexLock.unlock();
		return pool;
	}
}

StackAllocator * MemoryManager::CreateStackAllocator(uint64_t size)
{
	_mutexLock.lock();
	if (_remainingMemory < size + sizeof(StackAllocator))
	{
		throw std::runtime_error("Not enough memory");
	}
	char* rawAdd = (char*)_Allocate(size + sizeof(StackAllocator));
	StackAllocator* stack = new(rawAdd) StackAllocator(rawAdd + sizeof(StackAllocator), size);
	_allocatedBlocks[rawAdd] = sizeof(StackAllocator) + size;
	_remainingMemory -= sizeof(StackAllocator) + size;
	_mutexLock.unlock();
	return stack;
}

void MemoryManager::ReleasePoolAllocator(PoolAllocator * object)
{
	_Free(object, _allocatedBlocks[(void*)object]);
	_remainingMemory += _allocatedBlocks[object];
	_allocatedBlocks.erase(object);
}

void MemoryManager::ReleaseStackAllocator(StackAllocator * object)
{
	_Free(object, _allocatedBlocks[(void*)object]);
	_remainingMemory += _allocatedBlocks[object];
	_allocatedBlocks.erase(object);
}

void MemoryManager::PrintBlockInfo()
{
	BlockInfo* b = _firstFree;
	std::cout << "Starting address: " << reinterpret_cast<uintptr_t>(_memory) << "\n";
	do {
		std::cout << "Free Block @ " << reinterpret_cast<uintptr_t>(b) << ".   Size: " << b->size << std::endl;
		b = b->next;
	} while (b);

	for (auto& i : _allocatedBlocks)
	{
		std::cout << "Allocated Block @ " << reinterpret_cast<uintptr_t>(i.first) << ".    Size: " << i.second << std::endl;
	}
	std::cout << "Remaining memory: " << _remainingMemory << "\n";
}
