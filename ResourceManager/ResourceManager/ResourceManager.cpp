#include "ResourceManager.h"

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
	_numBlocks = 1000;
	_pool = new char[_numBlocks * _blockSize];

	// Make blocks form a linked list (all of them at startup)
	_SetupFreeBlockList();
}

ResourceManager::~ResourceManager()
{
	delete[] _pool;
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
