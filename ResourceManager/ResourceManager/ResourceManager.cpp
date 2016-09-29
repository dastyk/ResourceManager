#include "ResourceManager.h"

#include <stdexcept>
#include <Windows.h> // Because fuck of VS. Start following standards
#include <iostream>

using namespace std;

ResourceManager& ResourceManager::Instance()
{
	static ResourceManager instance;
	return instance;
}

ResourceManager::ResourceManager()
{
	_runningThread = thread(&ResourceManager::_Run, this);
}

ResourceManager::~ResourceManager()
{
}

Resource & ResourceManager::LoadResource(SM_GUID guid, const Resource::Flag& flag)
{
	// TODO: finish return statement
	// rawData = AssetLoader->Load(SM_GUID);
	// resources.push_back(AssetParser->Parse(rawData))

	// Mutex lock
	// if(loaded)
	//		Resource& r = _instance->Find(guid);
	//		r._refCount++;
	//		return r;
	// else
	_resources.push_back(Resource());
	Resource& r = _resources.back();
	r._refCount++;
	r.ID = guid;
	r._flags = flag;
	// Start thread
		// AssetLoader.LoadResource(guid);
		_parser.ParseResource(r);
	// Mutex unlock
	return r;
}

// Little debug function that outputs occupancy of blocks where O indicates open
// and X indicates occupied.
void ResourceManager::PrintOccupancy(void)
{
	if (_numBlocks == 0)
	{
		return;
	}

	int32_t walker = 0;
	int32_t free = _firstFreeBlock;
	
	while (walker < _numBlocks)
	{
		if (walker == free)
		{
			FreeBlock* f = reinterpret_cast<FreeBlock*>(_pool + free * _blockSize);
			free = f->Next;

			OutputDebugStringA("[O]");
		}
		else
		{
			OutputDebugStringA("[X]");
		}

		walker++;
	}

	OutputDebugStringA("\n");
}

void ResourceManager::TestAlloc(void)
{
	PrintOccupancy();
	_Allocate(3);
	PrintOccupancy();
	_Allocate(2);
	PrintOccupancy();
}

void ResourceManager::_Startup()
{
}

void ResourceManager::_SetupFreeBlockList(void)
{
	// Iterate through blocks (all are free at first) and reinterpret them
	// as the FreeBlock struct in order to set their previous and next ref-
	// erences correctly in order to create a doubly linked list.

	if (_numBlocks == 0)
	{
		_firstFreeBlock = -1;
	}
	// Special case because both previous and next are non-existant.
	else if (_numBlocks == 1)
	{
		_firstFreeBlock = 0;
		FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool);
		b->Previous = -1;
		b->Next = -1;
	}
	else
	{
		_firstFreeBlock = 0;

		for (uint32_t i = 1; i < _numBlocks - 1; ++i)
		{
			FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool + i * _blockSize);
			b->Previous = i - 1;
			b->Next = i + 1;
		}

		FreeBlock* first = reinterpret_cast<FreeBlock*>(_pool);
		first->Previous = -1;
		first->Next = 1;

		FreeBlock* last = reinterpret_cast<FreeBlock*>(_pool + (_numBlocks - 1) * _blockSize);
		last->Previous = _numBlocks - 2; // Zero based and one before this
		last->Next = -1;
	}
}

int ResourceManager::_Allocate(uint32_t blocks)
{
	if (_firstFreeBlock == -1)
	{
		throw runtime_error("No free blocks remaining!");
	}

	int32_t allocSlot = _firstFreeBlock;
	int32_t walker = _firstFreeBlock;
	uint32_t numContiguous = 1;

	// Keep searching until we found enough blocks. The case of not enough
	// contiguous blocks is taken care of inside the loop.
	while (numContiguous < blocks)
	{
		FreeBlock* lastFree = reinterpret_cast<FreeBlock*>(_pool + walker * _blockSize);

		if (lastFree->Next == -1)
		{
			throw runtime_error("Not enough contiguous free blocks to accomodate the allocation!");
		}

		// Not next contigious block; reset and keep trying
		if (lastFree->Next != walker + 1)
		{
			allocSlot = walker = lastFree->Next;
			numContiguous = 1;
		}
		// Contiguous
		else
		{
			walker = lastFree->Next;
			numContiguous++;
		}
	}

	// If we reached here it means we found enough contiguous blocks, with walker
	// being the last one of our range.

	if (allocSlot == _firstFreeBlock)
	{
		_firstFreeBlock = reinterpret_cast<FreeBlock*>(_pool + walker * _blockSize)->Next;

		if (_firstFreeBlock != -1)
			reinterpret_cast<FreeBlock*>(_pool + _firstFreeBlock * _blockSize)->Previous = -1;
	}
	else
	{
		FreeBlock* firstToAlloc = reinterpret_cast<FreeBlock*>(_pool + allocSlot * _blockSize);
		int32_t nextFree = reinterpret_cast<FreeBlock*>(_pool + walker * _blockSize)->Next;
		reinterpret_cast<FreeBlock*>(_pool + firstToAlloc->Previous * _blockSize)->Next = nextFree;

		if (nextFree != -1)
			reinterpret_cast<FreeBlock*>(_pool + nextFree * _blockSize)->Previous = firstToAlloc->Previous;
	}

	return allocSlot;
}

void ResourceManager::_Free(int32_t firstBlock, uint32_t numBlocks)
{

}

void ResourceManager::_Run()
{
	_numBlocks = 1000;
	_pool = new char[_numBlocks * _blockSize];

	// Make blocks form a linked list (all of them at startup)
	_SetupFreeBlockList();

	_running = true;
	while (_running)
	{
		//Loop through all resources, ticking them down
		for (auto &it : _resources)
			it._callCount--;


		this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	

	delete[] _pool;

}

void ResourceManager::ShutDown()
{
	_running = false;
	_runningThread.join();
}