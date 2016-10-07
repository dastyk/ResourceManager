#include "ResourceManager.h"
#pragma warning (disable : 4267)
#pragma warning (disable : 4018) //Signed/unsigned missmatch
#include <stdexcept>
#include <Windows.h> // Because fuck off VS. Start following standards
#include <iostream>
#include "DebugLogger.h"
#include <sstream>

#define NR_OF_LOADING_THREADS 1
#define NR_OF_PARSING_THREADS 2

using namespace std;

ResourceManager& ResourceManager::Instance()
{
	static ResourceManager instance;
	return instance;
}

ResourceManager::ResourceManager()
{
	_resourcePool = MemoryManager::CreatePoolAllocator(sizeof(ResourceList), 1337, 0);
}

ResourceManager::~ResourceManager()
{
	delete _assetLoader;
	_assetLoader = nullptr;
	
	delete[] _pool;
	_pool = nullptr;
}

Resource & ResourceManager::LoadResource(SM_GUID guid, const Resource::Flag& flag)
{
	// Idea of this method:
	// * If the resource already exists, don't do anything because we're done
	// * If not, we have to load it, which means that we need to reserve memory.
	// * We need the size to reserve, which means that we need to consult the asset loader for the file.
	// * Either we store the compressed size or the parsed data. The latter case involved a trip to the asset parser.
	// * When we have the final size it's time to allocate. If we find a suitable slot we can use it, otherwise something must be evicted.

	_mutexLockGeneral.lock();
	auto find = _FindResource(guid);
	if (find)
	{
		//printf("Resource already loaded. GUID: %llu \n", guid.data);
		_mutexLockGeneral.unlock();
		UpdatePriority(guid, flag);
		_mutexLockGeneral.lock();
		find->resource.IncRefCount();
		_mutexLockGeneral.unlock();
		return find->resource;
	}
	
	auto& r = _CreateResource(guid, flag)->resource;
	r.IncRefCount();
	if (flag & Resource::Flag::LOAD_AND_WAIT)
	{
		_mutexLockLoader.lock();
		printf("Resource loading. GUID: %llu\n", r.GetGUID().data);

		// TODO: Query asset loader for data size -> reserve memory here -> give pointer where asset data can be stored to the asset loader

		uint32_t startBlock = 0;
		uint32_t numBlocks = 0;

		RawData* rawData = _assetLoader->LoadResource( guid, [this, &startBlock, &numBlocks](uint32_t dataSize) -> char*
		{
			char* data = nullptr;

			numBlocks = static_cast<uint32_t>(ceilf( static_cast<float>(dataSize) / _blockSize ));
			int32_t allocSlot = _FindSuitableAllocationSlot( numBlocks );

			if ( allocSlot == -1 )
			{
				numBlocks = 0;
			}
			else
			{
				startBlock = allocSlot;
				_Allocate( allocSlot, numBlocks );
				data = _pool + numBlocks * _blockSize;
			}

			return data;
		} );

		r.SetData(rawData, startBlock, numBlocks, [this](RawData* data, uint32_t startBlock, uint32_t numBlocks) 
		{
			//operator delete(((RawData*)data)->data);
			_Free( startBlock, numBlocks );
			delete data;
		});
		_mutexLockLoader.unlock();

		_mutexLockParser.lock();
		_parser.ParseResource(r);
		_mutexLockParser.unlock();
		printf("Resource finished loading. GUID: %llu\n", r.GetGUID().data);
		r.SetState(Resource::ResourceState::Loaded);
	}
	else
	{
		_mutexLockLoadingQueue.lock();
		r.SetState(Resource::ResourceState::Waiting);
		printf("Adding resource to toLoad stack. GUID: %llu\n", r.GetGUID().data);
		_loadingQueue.push(&r);
		_mutexLockLoadingQueue.unlock();
	}
	_mutexLockGeneral.unlock();
	return r;
}

void ResourceManager::UnloadResource(SM_GUID guid)
{
	_mutexLockGeneral.lock();
	auto found = _FindResource(guid);

	if (found)
	{
		printf("Unreferencing resource. GUID: %llu. RefCount: %d\n", guid.data, found->resource._refCount);
		found->resource.Unload();
	}
	_mutexLockGeneral.unlock();
}

//Should maybe never be called
void ResourceManager::EvictResource(SM_GUID guid)
{
	printf("Setting resource to be evicted. GUID: %llu\n", guid.data);
	auto found = _FindResource(guid);

	if (found)
	{
		found->resource._refCount = 0;
	}


}

void ResourceManager::UpdatePriority(SM_GUID guid,const Resource::Flag& flag)
{
	_mutexLockGeneral.lock();
	auto find = _FindResource(guid);
	if (find)
	{
		find->resource._flags = flag;


		std::priority_queue<Resource*, std::vector<Resource*>, CompareResources> newQ;

		_mutexLockLoadingQueue.lock();
		_mutexLockParserQueue.lock();
		size_t size = _loadingQueue.size();
		for (size_t i = 0; i < size; i++)
		{
			auto r = _loadingQueue.top();
			_loadingQueue.pop();
			newQ.push(r);
		}

		_loadingQueue = std::move(newQ);



		size = _parserQueue.size();
		for (size_t i = 0; i < size; i++)
		{
			auto r = _parserQueue.top();
			_parserQueue.pop();
			newQ.push(r);
		}

		_parserQueue = std::move(newQ);
		_mutexLockParserQueue.unlock();
		_mutexLockLoadingQueue.unlock();
	}
	_mutexLockGeneral.unlock();

}

bool ResourceManager::IsLoaded(SM_GUID guid)
{
	_mutexLockGeneral.lock();
	auto find = _FindResource(guid);
	if (find)
	{
		bool ret = find->resource.GetState() == Resource::ResourceState::Loaded;
		_mutexLockGeneral.unlock();
		return ret;
	}
	_mutexLockGeneral.unlock();
	return false;
}

// Little debug function that outputs occupancy of blocks where O indicates open
// and X indicates occupied.
void ResourceManager::PrintOccupancy( void )
{
	if ( _numBlocks == 0 )
	{
		return;
	}

	int32_t walker = 0;
	int32_t free = _firstFreeBlock;

	while ( walker < _numBlocks )
	{
		if ( walker == free )
		{
			FreeBlock* f = reinterpret_cast<FreeBlock*>(_pool + free * _blockSize);
			free = f->Next;

			OutputDebugStringA( "[O]" );
		}
		else
		{
			OutputDebugStringA( "[X]" );
		}

		walker++;
	}

	OutputDebugStringA( "\n" );
}

void ResourceManager::TestAlloc( void )
{
	int32_t allocSlot = -1;
	//_mutexLock.lock();
	//_mutexLock.unlock();
	PrintOccupancy();
	allocSlot = _FindSuitableAllocationSlot(3);
	_Allocate(allocSlot, 3);
	PrintOccupancy();
	allocSlot = _FindSuitableAllocationSlot(2);
	_Allocate(allocSlot, 2);
	PrintOccupancy();
	allocSlot = _FindSuitableAllocationSlot(15);
	_Allocate(allocSlot, 15);
	PrintOccupancy();
	_Free(2, 3);
	PrintOccupancy();
	_Free(7, 1);
	PrintOccupancy();
	_Free(5, 1);
	PrintOccupancy();
	_Free(0, 1);
	PrintOccupancy();
	_Free(19, 1);
	PrintOccupancy();
	allocSlot = _FindSuitableAllocationSlot(2);
	_Allocate(allocSlot, 2);
	PrintOccupancy();
	_Free(9, 3);
	PrintOccupancy();
	_Free(14, 3);
	PrintOccupancy();
	allocSlot = _FindSuitableAllocationSlot(3);
	_Allocate(allocSlot, 3);
	PrintOccupancy();
}

void ResourceManager::Startup()
{
	_runningThread = thread(&ResourceManager::_Run, this);
}

void ResourceManager::SetAssetLoader(IAssetLoader * loader)
{
	_assetLoader = loader;
}

void ResourceManager::AddParser(const std::string& fileend, const std::function<void(Resource& r)>& parseFunction)
{
	uint32_t type = std::hash<std::string>{} (fileend);
	_assetLoader->AddType(type);
	_parser.AddParser(type, parseFunction);
	
}

void ResourceManager::_SetupFreeBlockList( void )
{
	// Iterate through blocks (all are free at first) and reinterpret them
	// as the FreeBlock struct in order to set their previous and next ref-
	// erences correctly in order to create a doubly linked list.

	if ( _numBlocks == 0 )
	{
		_firstFreeBlock = -1;
	}
	// Special case because both previous and next are non-existant.
	else if ( _numBlocks == 1 )
	{
		_firstFreeBlock = 0;
		FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool);
		b->Previous = -1;
		b->Next = -1;
	}
	else
	{
		_firstFreeBlock = 0;

		for ( uint32_t i = 1; i < _numBlocks - 1; ++i )
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

int32_t ResourceManager::_FindSuitableAllocationSlot( uint32_t blocks )
{
	if ( _firstFreeBlock == -1 )
	{
		return -1;
		//throw runtime_error("No free blocks remaining!");
	}

	int32_t allocSlot = _firstFreeBlock;
	int32_t walker = _firstFreeBlock;
	uint32_t numContiguous = 1;

	// Keep searching until we found enough blocks. The case of not enough
	// contiguous blocks is taken care of inside the loop.
	while ( numContiguous < blocks )
	{
		FreeBlock* lastFree = reinterpret_cast<FreeBlock*>(_pool + walker * _blockSize);

		if ( lastFree->Next == -1 )
		{
			return -1;
			//throw runtime_error("Not enough contiguous free blocks to accomodate the allocation!");
		}

		// Not next contigious block; reset and keep trying
		if ( lastFree->Next != walker + 1 )
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

	return allocSlot;
}

// Given a valid allocation slot and number of blocks -- extract those blocks
// from the list of free blocks.
void ResourceManager::_Allocate( int32_t allocSlot, uint32_t blocks )
{
	if ( allocSlot == -1 )
	{
		throw runtime_error( "Invalid allocation slot!" );
	}

	if ( allocSlot == _firstFreeBlock )
	{
		// Index of the block after the last one to allocate
		_firstFreeBlock = reinterpret_cast<FreeBlock*>(_pool + (allocSlot + blocks - 1) * _blockSize)->Next;

		if ( _firstFreeBlock != -1 )
			reinterpret_cast<FreeBlock*>(_pool + _firstFreeBlock * _blockSize)->Previous = -1;
	}
	else
	{
		FreeBlock* firstToAlloc = reinterpret_cast<FreeBlock*>(_pool + allocSlot * _blockSize);
		int32_t nextFree = reinterpret_cast<FreeBlock*>(_pool + (allocSlot + blocks - 1) * _blockSize)->Next;
		reinterpret_cast<FreeBlock*>(_pool + firstToAlloc->Previous * _blockSize)->Next = nextFree;

		if ( nextFree != -1 )
			reinterpret_cast<FreeBlock*>(_pool + nextFree * _blockSize)->Previous = firstToAlloc->Previous;
	}

	_numFreeBlocks -= blocks;
}

// Frees certain blocks by inserting them into the free block list.
void ResourceManager::_Free( int32_t firstBlock, uint32_t numBlocks )
{
	// If there is no list to insert into, just make the current ones the new list.
	if ( _firstFreeBlock == -1 )
	{
		_firstFreeBlock = firstBlock;

		// Set up previous link on the first block
		FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize);
		b->Previous = -1;

		// For any other blocks we correctly set up the next link BEFORE actually
		// getting the next block, after which we set up the previous link.
		for ( uint32_t block = firstBlock + 1; block < firstBlock + numBlocks; ++block )
		{
			b->Next = block;
			b = reinterpret_cast<FreeBlock*>(_pool + block * _blockSize);
			b->Previous = block - 1;
		}

		// The last block will now indicate the end of the list
		b->Next = -1;
	}
	// First free block is after the ones we are returning
	else if ( _firstFreeBlock > firstBlock + numBlocks )
	{
		int32_t oldFirst = _firstFreeBlock;

		// Insert at head of list (like above), with the exception that the last
		// block points to the former first free block.
		_firstFreeBlock = firstBlock;

		// Set up previous link on the first block
		FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize);
		b->Previous = -1;

		// For any other blocks we correctly set up the next link BEFORE actually
		// getting the next block, after which we set up the previous link.
		for ( uint32_t block = firstBlock + 1; block < firstBlock + numBlocks; ++block )
		{
			b->Next = block;
			b = reinterpret_cast<FreeBlock*>(_pool + block * _blockSize);
			b->Previous = block - 1;
		}

		// The last block will now point to the former head of the list
		b->Next = oldFirst;
		reinterpret_cast<FreeBlock*>(_pool + oldFirst * _blockSize)->Previous = firstBlock + numBlocks - 1;
	}
	// Existing free blocks before the ones we are returning. This means that
	// we must find where in the list to insert and properly update the blocks
	// before and after (if existing).
	else
	{
		int32_t lastFree = _firstFreeBlock;
		FreeBlock* lastFreeBlock = reinterpret_cast<FreeBlock*>(_pool + lastFree * _blockSize);
		int32_t currFree = _firstFreeBlock;
		FreeBlock* currFreeBlock = reinterpret_cast<FreeBlock*>(_pool + currFree * _blockSize);

		// Keep trying until we found correct spot (where the slot changed from immediately before to after)
		while ( currFree < firstBlock ) // Less than comparison should be valid for entire range due to allocation list housekeeping
		{
			lastFree = currFree;
			lastFreeBlock = currFreeBlock;

			currFree = lastFreeBlock->Next;
			if ( currFree == -1 )
				break;

			currFreeBlock = reinterpret_cast<FreeBlock*>(_pool + currFree * _blockSize);
		}

		lastFreeBlock->Next = firstBlock;

		// Set up previous link on the first block
		FreeBlock* b = reinterpret_cast<FreeBlock*>(_pool + firstBlock * _blockSize);
		b->Previous = lastFree;

		// For any other blocks we correctly set up the next link BEFORE actually
		// getting the next block, after which we set up the previous link.
		for ( uint32_t block = firstBlock + 1; block < firstBlock + numBlocks; ++block )
		{
			b->Next = block;
			b = reinterpret_cast<FreeBlock*>(_pool + block * _blockSize);
			b->Previous = block - 1;
		}

		// Link to the next block (will be -1 as appropriate if end of pool)
		b->Next = currFree;

		// If we have another block, get it as link to the last returned block
		if ( currFree != -1 )
			reinterpret_cast<FreeBlock*>(_pool + currFree * _blockSize)->Previous = firstBlock + numBlocks - 1;
	}

	_numFreeBlocks += numBlocks;
}


void ResourceManager::_Run()
{
	_mutexLockGeneral.lock();
	
	_numFreeBlocks = _numBlocks = 20;
	_pool = new char[_numBlocks * _blockSize];

	// Make blocks form a linked list (all of them at startup)
	_SetupFreeBlockList();

	//Create threads and initialize them as "free"
	for (uint16_t i = 0; i < NR_OF_LOADING_THREADS; i++)
	{
		_threadIDMap.insert({ i, thread() });

		_threadIDMap.find(i)->second = thread(&ResourceManager::_LoadingThread, this, i);

		_threadRunningMap.insert({ i, ThreadControl() });
	}

	for (uint16_t i = NR_OF_LOADING_THREADS; i < NR_OF_PARSING_THREADS + NR_OF_LOADING_THREADS; i++)
	{
		_threadIDMap.insert({ i, thread() });

		_threadIDMap.find(i)->second = thread(&ResourceManager::_ParserThread, this, i);

		_threadRunningMap.insert({ i, ThreadControl() });

	}
	_running = true;
	_mutexLockGeneral.unlock();

	while (_running)
	{
		_mutexLockGeneral.lock();
		//Loop through all resources, ticking them down
		uint64_t erased = 0;
		auto r = _resources;
		while (r)
		{
			if (r->resource._refCount == 0)
			{
				printf("Unloading resource. GUID: %llu.\n", r->resource.ID.data);
				_RemoveResource(r);
				break;
			}
			r = r->next;
		}
		_mutexLockGeneral.unlock();
		this_thread::sleep_for(std::chrono::milliseconds(17));
	}
	bool allThreadsJoined = false;
	while (!allThreadsJoined)
	{
		allThreadsJoined = true;
		_mutexLockGeneral.lock();
		for (auto &it : _threadRunningMap)
		{
			if (it.second.inUse)
			{
				allThreadsJoined = false;
			}
			else
			{
				if (!it.second.beenJoined)
				{
					_threadIDMap.find(it.first)->second.join();
					it.second.beenJoined = true;
				}
			}
		}
		_mutexLockGeneral.unlock();
	}



}

	
void ResourceManager::_LoadingThread(uint16_t threadID)
{
	_mutexLockGeneral.lock();

	_threadRunningMap.find(threadID)->second.inUse = true;
	_threadRunningMap.find(threadID)->second.beenJoined = false;
	_mutexLockGeneral.unlock();
	while (_running)
	{
		//_mutexLockGeneral.lock();
		_mutexLockLoadingQueue.lock();
		if(_loadingQueue.size())
		{
			Resource* job = _loadingQueue.top();
			_loadingQueue.pop();
			_mutexLockLoadingQueue.unlock();
			ostringstream dataStream;
			dataStream << job->GetGUID().data;

			job->SetState(Resource::ResourceState::Loading);
			printf("Started loading resource. GUID: %llu\n", job->GetGUID().data);
			//DebugLogger::GetInstance()->AddMsg("Started Job: " + dataStream.str());



			_mutexLockLoader.lock();
			//Call asset loader to load the data we want
			uint32_t startBlock = 0;
			uint32_t numBlocks = 0;
			RawData* rawData = _assetLoader->LoadResource(job->GetGUID(), [this, &startBlock, &numBlocks]( uint32_t dataSize ) -> char*
			{
				char* data = nullptr;

				numBlocks = static_cast<uint32_t>(ceilf( static_cast<float>(dataSize) / _blockSize ));
				int32_t allocSlot = _FindSuitableAllocationSlot( numBlocks );

				if ( allocSlot == -1 )
				{
					numBlocks = 0;
				}
				else
				{
					startBlock = allocSlot;
					_Allocate( allocSlot, numBlocks );
					data = _pool + numBlocks * _blockSize;
				}

				return data;
			} );
			_mutexLockLoader.unlock();

			job->SetState(Resource::ResourceState::Waiting);
			printf("Finished loading resource. GUID: %llu\n", job->GetGUID().data);
			//Lock so we can insert the data to the resources

			job->SetData(rawData, startBlock, numBlocks, [this]( RawData* data, uint32_t startBlock, uint32_t numBlocks )
			{
				//operator delete(((RawData*)data)->data);
				_Free( startBlock, numBlocks );
				delete data;
			});

			_mutexLockParserQueue.lock();
			_parserQueue.push(job);
			_mutexLockParserQueue.unlock();

		}
		else
			_mutexLockLoadingQueue.unlock();
	//	_mutexLockGeneral.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}

	_mutexLockGeneral.lock();
	_threadRunningMap.find(threadID)->second.inUse = false;
	_mutexLockGeneral.unlock();
}

void ResourceManager::_ParserThread(uint16_t threadID)
{
	_mutexLockGeneral.lock();

	_threadRunningMap[threadID].inUse = true;
	_threadRunningMap[threadID].beenJoined = false;
	_mutexLockGeneral.unlock();
	while (_running)
	{
		//_mutexLockGeneral.lock();
		_mutexLockParserQueue.lock();
		if (_parserQueue.size())
		{
			Resource* workingResource = _parserQueue.top();
			_parserQueue.pop();
			_mutexLockParserQueue.unlock();
			// TODO: Create one thread for loading the resource and one, or multiple threads for parsing the data.

			workingResource->SetState(Resource::ResourceState::Parsing);
			printf("Starting parsing resource. GUID: %llu\n", workingResource->GetGUID().data);
			//_mutexLockParser.lock();
			//Let the parser make their magic. Should implement a "dummy" GUID at this point in time, that we "use as resource" for the frame that the parser might work
			_parser.ParseResource(*workingResource);
			//_mutexLockParser.unlock();

			workingResource->SetState(Resource::ResourceState::Loaded);
			printf("Finished parsing resource. GUID: %llu\n", workingResource->GetGUID().data);

		}
		else
			_mutexLockParserQueue.unlock();
	//	_mutexLockGeneral.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}
	_mutexLockGeneral.lock();
	_threadRunningMap[threadID].inUse = false;
	_mutexLockGeneral.unlock();
}

void ResourceManager::ShutDown()
{
	_running = false;
	_runningThread.join();
	delete _assetLoader;
	_assetLoader = nullptr;

	delete[] _pool;
	_pool = nullptr;

	auto r = _resources;
	while(r)
	{		
		_RemoveResource(r);
		r = _resources;
	}
}