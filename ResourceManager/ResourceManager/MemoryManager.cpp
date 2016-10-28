#include "MemoryManager.h"
#include <iostream>
#include <string.h>

MemoryManager* MemoryManager::_instance = nullptr;

MemoryManager::MemoryManager()
{
}

MemoryManager::~MemoryManager()
{
	delete[] _memory;
}

void MemoryManager::CreateInstance()
{
	_instance = new MemoryManager;
}

void MemoryManager::DeleteInstance()
{
	delete _instance;
}

MemoryManager * MemoryManager::GetInstance()
{
	return _instance;
}

void MemoryManager::Init(uint64_t size)
{
	_free = _memory = new char[size];
	memset(_free, 0, size);

	_remainingMemory = size;
	_firstFree = (BlockInfo*)_free;
	_firstFree->size = size;
	_firstFree->next = nullptr;
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
		_instance->_mutexLock.lock();

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
		char* rawAddress = (char*)_instance->_Allocate(effectiveBlockSize * (nrOfObjects + 1) + sizeof(PoolAllocator));
		uint64_t mask = alignment - 1;
		uint64_t misalignment = mask & (size_t)(rawAddress + sizeof(PoolAllocator));
		uint64_t adjustment = alignment - misalignment;
		uint64_t requirement = effectiveBlockSize * (nrOfObjects + 1) + sizeof(PoolAllocator);
		if (_instance->_remainingMemory < (requirement))
		{
			throw std::runtime_error("Memory Manager ran out of memory");
		}
		_instance->_remainingMemory -= requirement;
		char* alignedAddress = rawAddress + adjustment + sizeof(PoolAllocator);
		
		PoolAllocator* pool = new(rawAddress) PoolAllocator(alignedAddress, effectiveBlockSize, nrOfObjects);
		_instance->_allocatedBlocks[rawAddress] = effectiveBlockSize * (nrOfObjects + 1) + sizeof(PoolAllocator);

		_instance->_mutexLock.unlock();

		return pool;
	}
	else
	{
		_instance->_mutexLock.lock();
		char* rawAdd = (char*)_instance->_Allocate(sizeOfObject * nrOfObjects + sizeof(PoolAllocator));
		PoolAllocator* pool = new(rawAdd) PoolAllocator(rawAdd + sizeof(PoolAllocator), sizeOfObject, nrOfObjects);
		_instance->_allocatedBlocks[rawAdd] = sizeof(PoolAllocator) + sizeOfObject * nrOfObjects;
		_instance->_mutexLock.unlock();
		return pool;
	}
}

StackAllocator * MemoryManager::CreateStackAllocator(uint64_t size)
{
	_instance->_mutexLock.lock();
	if (_instance->_remainingMemory < size + sizeof(StackAllocator))
	{
		throw std::runtime_error("Memory Manager ran out of memory");
	}
	char* rawAdd = (char*)_instance->_Allocate(size + sizeof(StackAllocator));
	StackAllocator* stack = new(rawAdd) StackAllocator(rawAdd + sizeof(StackAllocator), size);
	_instance->_allocatedBlocks[rawAdd] = sizeof(StackAllocator) + size;
	_instance->_remainingMemory -= sizeof(StackAllocator) + size;
	_instance->_mutexLock.unlock();
	return stack;
}

void * MemoryManager::Alloc(uint32_t size)
{
	_instance->_mutexLock.lock();
	uint32_t tSize = size + sizeof(uint32_t);
	if (_instance->_remainingMemory < tSize)
		throw std::runtime_error("Memory Manager ran out of memory.");

	void* ret = _instance->_Allocate(tSize);
	*(uint32_t*)ret = tSize;
	_instance->_allocatedBlocks[ret] = tSize;
	_instance->_remainingMemory -= tSize;
	_instance->_mutexLock.unlock();
	
	return (void*)((uint32_t*)ret  + 1);
}

void MemoryManager::ReleasePoolAllocator(PoolAllocator * object)
{
	_instance->_mutexLock.lock();
	_instance->_Free(object, _instance->_allocatedBlocks[(void*)object]);
	_instance->_remainingMemory += _instance->_allocatedBlocks[object];
	_instance->_allocatedBlocks.erase(object);
	_instance->_mutexLock.unlock();
}

void MemoryManager::ReleaseStackAllocator(StackAllocator * object)
{
	_instance->_mutexLock.lock();
	_instance->_Free(object, _instance->_allocatedBlocks[(void*)object]);
	_instance->_remainingMemory += _instance->_allocatedBlocks[object];
	_instance->_allocatedBlocks.erase(object);
	_instance->_mutexLock.unlock();
}

void MemoryManager::Release(void * p)
{
	if(p)
	{
		_instance->_mutexLock.lock();
		void* a = (void*)((uint32_t*)p - 1);
		uint32_t size = *(uint32_t*)a;
		_instance->_Free(a, size);
		_instance->_remainingMemory += _instance->_allocatedBlocks[a];
		_instance->_allocatedBlocks.erase(a);
		_instance->_mutexLock.unlock();
	}
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
