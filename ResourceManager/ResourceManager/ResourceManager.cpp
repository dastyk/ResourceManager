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
	
}

ResourceManager::~ResourceManager()
{

}

const SM_GUID ResourceManager::LoadResource(SM_GUID guid, const Resource::Flag& flag)
{
	//Lock the "general" lock so that we won't be able to change a resource before we're done with it. Unlocked right before a return
	_mutexLockGeneral.lock();

	//Check to see if the GUID is already loaded, in that case see if we can update the priority and then return the Load Resource
	// (Simply put: if already loaded or in queue to be loaded, don't push it into the queue to be loaded.
	auto find = _FindResource(guid);
	if (find)
	{
		_UpdatePriority(guid, flag);
		find->resource.IncRefCount();
		_mutexLockGeneral.unlock();
		return guid;
	}
	
	//Create the resource
	auto& r = _CreateResource(guid, flag)->resource;
	r.IncRefCount();

	//If we don't want to push it onto a thread, and instead want to have it loaded right the fuck now, we do so. 
	//Someone else feel free to comment what's going on here. I have no idea, really.
	if (flag & Resource::Flag::LOAD_AND_WAIT)
	{
		//TODO: Move this so we don't "find" a GUID that early have another flag, and now is LOAD_AND_WAIT, but not loaded and just in queue.
		_mutexLockLoader.lock();
		printf("Resource loading. GUID: %llu\n", r.GetGUID().data);

		// TODO: Query asset loader for data size -> reserve memory here -> give pointer where asset data can be stored to the asset loader

		uint32_t startBlock = 0;
		uint32_t numBlocks = 0;

		RawData rawData = _assetLoader->LoadResource( guid, [this, &startBlock, &numBlocks](uint32_t dataSize) -> char*
		{
			char* data = nullptr;

			numBlocks = static_cast<uint32_t>(ceilf( static_cast<float>(dataSize) / _blockSize ));
			startBlock = _Allocate( numBlocks );

			if (startBlock == -1 )
			{
				numBlocks = 0;
			}
			else
			{
				data = _pool + startBlock * _blockSize;
			}

			return data;
		} );

		r.SetData(rawData, startBlock, numBlocks);
		_mutexLockLoader.unlock();

		_mutexLockParser.lock();
		_parser.ParseResource(r);
		_mutexLockParser.unlock();
		printf("Resource finished loading. GUID: %llu\n", r.GetGUID().data);
		r.SetState(Resource::ResourceState::Loaded);
	}
	else
	{
		//Lock the loading queue so we can push our new resource to it.
		_mutexLockLoadingQueue.lock();
		
		//Set the state of the resource to waiting (to be loaded), print out that it is on the stack and push it to the stack
		r.SetState(Resource::ResourceState::Waiting);
		printf("Adding resource to toLoad stack. GUID: %llu\n", r.GetGUID().data);
		_loadingQueue.push(&r);

		//Unlock the loading queue so we don't cause a deadlock
		_mutexLockLoadingQueue.unlock();
	}

	//Unlock the general lock and return the function with the GUID.
	_mutexLockGeneral.unlock();
	return guid;
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

void ResourceManager::_UpdatePriority(SM_GUID guid,const Resource::Flag& flag)
{
	auto find = _FindResource(guid);
	if (find)
	{
		//Only update the resource if it gets a HIGHER priorty.
		//Pop the entire queue and reorder it. A lot of work, is it actually worth?
		if (find->resource._flags < flag)
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
	}
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

			//OutputDebugStringA( "[O]" );
			printf("[O]");
		}
		else
		{
			//OutputDebugStringA( "[X]" );
			printf("[X]");
		}

		walker++;
	}

	//OutputDebugStringA( "\n" );
	printf("\n");
}

void ResourceManager::TestAlloc( void )
{
	//int32_t allocSlot = -1;
	////_mutexLock.lock();
	////_mutexLock.unlock();
	//PrintOccupancy();
	//allocSlot = _FindSuitableAllocationSlot(3);
	//_Allocate(allocSlot, 3);
	//PrintOccupancy();
	//allocSlot = _FindSuitableAllocationSlot(2);
	//_Allocate(allocSlot, 2);
	//PrintOccupancy();
	//allocSlot = _FindSuitableAllocationSlot(15);
	//_Allocate(allocSlot, 15);
	//PrintOccupancy();
	//_Free(2, 3);
	//PrintOccupancy();
	//_Free(7, 1);
	//PrintOccupancy();
	//_Free(5, 1);
	//PrintOccupancy();
	//_Free(0, 1);
	//PrintOccupancy();
	//_Free(19, 1);
	//PrintOccupancy();
	//allocSlot = _FindSuitableAllocationSlot(2);
	//_Allocate(allocSlot, 2);
	//PrintOccupancy();
	//_Free(9, 3);
	//PrintOccupancy();
	//_Free(14, 3);
	//PrintOccupancy();
	//allocSlot = _FindSuitableAllocationSlot(3);
	//_Allocate(allocSlot, 3);
	//PrintOccupancy();

	//int32_t allocSlot = _Allocate(6);
	//PrintOccupancy();

	//allocSlot = _Allocate(14);
	//PrintOccupancy();

	//_Free(6, 1);
	//PrintOccupancy();

	//allocSlot = _Allocate(1);
	//PrintOccupancy();

	//_Free(18, 2);
	//PrintOccupancy();

	//_Free(19, 1);
	//PrintOccupancy();

	//_Free(9, 2);
	//PrintOccupancy();

	//int32_t allocSlot = _Allocate(18);
	//allocSlot = _Allocate(1);
	//allocSlot = _Allocate(1);
	//allocSlot = _Allocate(1);


	//int32_t a1 = _Allocate(1);
	//int32_t a2 = _Allocate(1);
	//int32_t a3 = _Allocate(1);
	//int32_t a4 = _Allocate(1);
	//int32_t a5 = _Allocate(1);

	//_Free(a4, 1);
	//_Free(a5, 1);

	//int i = 0;
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
	_mutexAllocLock.lock();
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
	_mutexAllocLock.unlock();
}

// Returns the slot that was allocated at. If no suitable slot was found, -1 is returned.
int32_t ResourceManager::_Allocate( uint32_t blocks )
{
	// This first part finds a suitable allocation slot

	_mutexAllocLock.lock();

	if ( _firstFreeBlock == -1 )
	{
		_mutexAllocLock.unlock();
		return -1;
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
			_mutexAllocLock.unlock();
			return -1;
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

	// Given a valid allocation slot and number of blocks -- extract those blocks
	// from the list of free blocks.

	if ( allocSlot == -1 )
	{
		_mutexAllocLock.unlock();
		return -1;
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

	_mutexAllocLock.unlock();

	return allocSlot;
}

// Frees certain blocks by inserting them into the free block list.
void ResourceManager::_Free( int32_t firstBlock, uint32_t numBlocks )
{
	_mutexAllocLock.lock();

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
	else if ( _firstFreeBlock > firstBlock + numBlocks - 1)
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

	_mutexAllocLock.unlock();
}


/*===============================================================
/*		The main "Run" thread for the Resource Manager
/*	From here, we start the LoadingThread and the two ParserThreads
/*	For as long as we haven't shut down this thread, it'll loop through
/*	the resources and remove resources not "currently in use".
/*	(Read: Loaded, non-persistent and without a _refCount)
/*	When this function is closed, it makes sure that it's child
/*	threads are terminated as well.
/*
/*	Between each loop, we'll wait for 17 ms before going again,
/*	about 1 "frame".
/*	=============================================================
*/

void ResourceManager::_Run()
{
	//Lock so we can initialize everything without any intereference.
	_mutexLockGeneral.lock();

	//Create threads and initialize them as "free"
	//Both the loadingThread and the parserThread.
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
		//Loop through all resources, removing the first, and only one, resource available to be removed.
		uint64_t erased = 0;
		auto r = _resources;
		while (r)
		{
			if (r->resource._refCount == 0 && r->resource.GetState() == Resource::ResourceState::Loaded && !(r->resource._flags & Resource::Flag::PERSISTENT))
			{
				printf("Unloading resource. GUID: %llu.\n", r->resource.ID.data);
				_RemoveResource(r);
				break;
			}
			r = r->next;
		}
		_mutexLockGeneral.unlock();

		//Taking a nap.
		this_thread::sleep_for(std::chrono::milliseconds(17));
	}

	//When we're shutting down, we wait for our child-threads and join them in.
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


/*===============================================================
/*		The "Loading" thread for the Resource Manager
/*	This thread is quite simple. Each iteration of the loop,
/*	it checks if there's any job for it in the loading queue.
/*	If so, it'll mark this job as "loading" and start loading it. 
/*	Once done, it'll mark the job "waiting" again and push it onto
/*	the parser queue.
/*
/*	Between each loop, we'll wait for 17 ms before going again,
/*	about 1 "frame".
/*	=============================================================
*/
void ResourceManager::_LoadingThread(uint16_t threadID)
{

	//Lock the general so we can change ourselves to "in use" and "not joined"
	_mutexLockGeneral.lock();

	_threadRunningMap.find(threadID)->second.inUse = true;
	_threadRunningMap.find(threadID)->second.beenJoined = false;
	_mutexLockGeneral.unlock();

	while (_running)
	{
		//We don't wish for nasty surprises.
		_mutexLockLoadingQueue.lock();
		if(_loadingQueue.size())
		{
			//Get the job
			Resource* job = _loadingQueue.top();
			_loadingQueue.pop();
			_mutexLockLoadingQueue.unlock();

			//Proudly write out what GUID we have started working on.
			ostringstream dataStream;
			dataStream << job->GetGUID().data;

			job->SetState(Resource::ResourceState::Loading);
			printf("Started loading resource. GUID: %llu\n", job->GetGUID().data);


			//Lock the loader so we can work in peace and quiet.
			_mutexLockLoader.lock();

			//Call asset loader to load the data we want
			uint32_t startBlock = 0;
			uint32_t numBlocks = 0;

			try
			{
				RawData rawData = _assetLoader->LoadResource(job->GetGUID(), [this, &startBlock, &numBlocks](uint32_t dataSize) -> char*
				{
					char* data = nullptr;

					numBlocks = static_cast<uint32_t>(ceilf(static_cast<float>(dataSize) / _blockSize));
					startBlock = _Allocate(numBlocks);

					if (startBlock == -1)
					{
						numBlocks = 0;
					}
					else
					{
						data = _pool + startBlock * _blockSize;
					}

					return data;
				});


				_mutexLockLoader.unlock();

				printf("Finished loading resource. GUID: %llu\n", job->GetGUID().data);
				job->SetState(Resource::ResourceState::Waiting);
				job->SetData(rawData, startBlock, numBlocks);

				//Lock so we can insert the data to the resources
				_mutexLockParserQueue.lock();
				_parserQueue.push(job);
				_mutexLockParserQueue.unlock();
			}
			catch (std::runtime_error& e)
			{
				//We don't have enough memory. Wait one "sleep", but push the job back onto the queue for a new try.
				_mutexLockLoadingQueue.lock();
				job->SetState(Resource::ResourceState::Waiting);
				_loadingQueue.push(job);
				_mutexLockLoadingQueue.unlock();

				_mutexLockLoader.unlock();
			}
			

		}
		else
			_mutexLockLoadingQueue.unlock();
	
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}

	//Mark us "done", that is "not in use".
	_mutexLockGeneral.lock();
	_threadRunningMap.find(threadID)->second.inUse = false;
	_mutexLockGeneral.unlock();
}


/*===============================================================
/*		The "Parsing" thread for the Resource Manager
/*	This thread is quite simple. Each iteration of the loop,
/*	it checks if there's any job for it in the parsing queue.
/*	If so, it'll mark this job as "Parsing" and start parsing it.
/*	Once done, it'll mark the job "Loaded".
/*
/*	Between each loop, we'll wait for 17 ms before going again,
/*	about 1 "frame".
/*	=============================================================
*/
void ResourceManager::_ParserThread(uint16_t threadID)
{
	//Lock general so we can mark that we're in use.
	_mutexLockGeneral.lock();

	_threadRunningMap[threadID].inUse = true;
	_threadRunningMap[threadID].beenJoined = false;
	_mutexLockGeneral.unlock();


	while (_running)
	{
		//Lock the parser queue so we can check for jobs and take them (no walls here)	
		_mutexLockParserQueue.lock();
		if (_parserQueue.size())
		{

			Resource* workingResource = _parserQueue.top();
			_parserQueue.pop();
			_mutexLockParserQueue.unlock();

			//Mark it as parsed, notify the user and start parsing it.
			workingResource->SetState(Resource::ResourceState::Parsing);
			printf("Starting parsing resource. GUID: %llu\n", workingResource->GetGUID().data);
			
			_parser.ParseResource(*workingResource);
			
			//The resource is now loaded and marked as such, the used is notified.
			workingResource->SetState(Resource::ResourceState::Loaded);
			printf("Finished parsing resource. GUID: %llu\n", workingResource->GetGUID().data);

		}
		else
			_mutexLockParserQueue.unlock();
	
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}

	//Let us join in the thread, mark us as "no longer in use"
	_mutexLockGeneral.lock();
	_threadRunningMap[threadID].inUse = false;
	_mutexLockGeneral.unlock();
}

void ResourceManager::Init(uint64_t maxMemory)
{
	uint32_t nrb = maxMemory / (sizeof(ResourceList) + _blockSize);

	_resourcePool = MemoryManager::CreatePoolAllocator(sizeof(ResourceList), nrb, 0);

	_numFreeBlocks = _numBlocks = nrb;
	_pool = (char*)MemoryManager::Alloc(_numBlocks * _blockSize);

	// Make blocks form a linked list (all of them at startup)
	_SetupFreeBlockList();
}

void ResourceManager::ShutDown()
{
	_running = false;
	_runningThread.join();

	delete _assetLoader;
	_assetLoader = nullptr;


	_RemoveAllResources();
	MemoryManager::ReleasePoolAllocator(_resourcePool);
	MemoryManager::Release(_pool);


}