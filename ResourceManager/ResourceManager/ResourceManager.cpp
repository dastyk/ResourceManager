#include "ResourceManager.h"

ResourceManager* ResourceManager::_instance = nullptr;

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

void ResourceManager::CreateInstance()
{
	if (_instance)
		DeleteInstance();

	_instance = new ResourceManager();

}

void ResourceManager::DeleteInstance()
{
	delete _instance;
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
	_instance->_resources.push_back(Resource());
	Resource& r = _instance->_resources.back();
	r._refCount++;
	r.ID = guid;
	r._flags = flag;
	// Mutex unlock
	return r;
}

void ResourceManager::_SetupFreeBlockList(void)
{
	if (_numBlocks == 0)
	{
		_firstFreeBlock = -1;
	}
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

		// Iterate through blocks (all are free at first) and set the first bytes
		// to the pointer of the next block, thereby creating a linked list.
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
