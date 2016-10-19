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
	auto find = _resource.Find(guid);
	if (find != Resource::NotFound)
	{
		//_UpdatePriority(guid, flag);
		_resource.data.refCount[_resource.count]++;
		_mutexLockGeneral.unlock();
		return guid;
	}
	
	//Create the resource
	const Resource::DataPointers& data = _resource.data;
	uint32_t count = _resource.count;
	data.pinned[count] = true;
	data.flags[count] = flag;
	data.guid[count] = guid;
	data.refCount[count] = 1;
	data.numBlocks[count] = 0;
	data.startBlock[count] = 0;

	//If we don't want to push it onto a thread, and instead want to have it loaded right the fuck now, we do so. 
	//Someone else feel free to comment what's going on here. I have no idea, really.
	if (flag & Resource::Flag::LOAD_AND_WAIT)
	{
		//TODO: Move this so we don't "find" a GUID that early have another flag, and now is LOAD_AND_WAIT, but not loaded and just in queue.
		_mutexLockLoader.lock();
		printf("Resource loading. GUID: %llu\n", guid.data);

		// TODO: Query asset loader for data size -> reserve memory here -> give pointer where asset data can be stored to the asset loader

		uint32_t startBlock = 0;
		uint32_t numBlocks = 0;

		RawData rawData = _assetLoader->LoadResource( guid, [this, &startBlock, &numBlocks](uint32_t dataSize) -> char*
		{
			char* data = nullptr;

			numBlocks = static_cast<uint32_t>(ceilf(static_cast<float>(dataSize) / _allocator->BlockSize()));
			startBlock = _allocator->Allocate( numBlocks );

			if (startBlock == -1 )
			{
				numBlocks = 0;
			}
			else
			{
				data = _allocator->Data(startBlock);
			}

			return data;
		} );

		data.rawData[count] = rawData.data;
		data.type[count] = rawData.fType;
		data.size[count] = rawData.size;

		_mutexLockLoader.unlock();

		_parser.ParseResource(_resource.MakePtr(count));


		data.pinned[count] = false;
		printf("Resource finished loading. GUID: %llu\n", guid.data);
		
	}
	else
	{
		//Lock the loading queue so we can push our new resource to it.
		_mutexLockLoadingQueue.lock();
		
		//Set the state of the resource to waiting (to be loaded), print out that it is on the stack and push it to the stack
		printf("Adding resource to toLoad stack. GUID: %llu\n", guid.data);
		_loadingQueue.push(count);

		//Unlock the loading queue so we don't cause a deadlock
		_mutexLockLoadingQueue.unlock();
	}

	//Unlock the general lock and return the function with the GUID.
	_resource.count++;
	_mutexLockGeneral.unlock();
	return guid;
}

void ResourceManager::UnloadResource(SM_GUID guid)
{

	auto found = _resource.Find(guid);

	if (found != Resource::NotFound)
	{
		_mutexLockGeneral.lock();
		auto& refCount = _resource.data.refCount[found];
		refCount = (refCount > 0) ? refCount-1 : 0;
		_mutexLockGeneral.unlock();
		printf("Unreferencing resource. GUID: %llu. RefCount: %d\n", guid.data, refCount);
		
	}
	
}

//
//void ResourceManager::_UpdatePriority(SM_GUID guid,const Resource::Flag& flag)
//{
//	auto find = _FindResource(guid, _resources);
//	if (find)
//	{
//		//Only update the resource if it gets a HIGHER priorty.
//		//Pop the entire queue and reorder it. A lot of work, is it actually worth?
//		if (find->resource._flags < flag)
//		{
//			find->resource._flags = flag;
//
//
//			std::priority_queue<Resource*, std::vector<Resource*>, CompareResources> newQ;
//
//			_mutexLockLoadingQueue.lock();
//			_mutexLockParserQueue.lock();
//			size_t size = _loadingQueue.size();
//			for (size_t i = 0; i < size; i++)
//			{
//				auto r = _loadingQueue.top();
//				_loadingQueue.pop();
//				newQ.push(r);
//			}
//
//			_loadingQueue = std::move(newQ);
//
//
//
//			size = _parserQueue.size();
//			for (size_t i = 0; i < size; i++)
//			{
//				auto r = _parserQueue.top();
//				_parserQueue.pop();
//				newQ.push(r);
//			}
//
//			_parserQueue = std::move(newQ);
//			_mutexLockParserQueue.unlock();
//			_mutexLockLoadingQueue.unlock();
//		}
//	}
//}

bool ResourceManager::IsLoaded(SM_GUID guid)
{
	//_mutexLockGeneral.lock();
	auto find = _resource.Find(guid);
	if (find != Resource::NotFound)
	{
		bool ret = !_resource.data.pinned[find];
	//	_mutexLockGeneral.unlock();
		return ret;
	}
	//_mutexLockGeneral.unlock();
	return false;
}

void ResourceManager::TestAlloc( void )
{
	//_allocator->PrintOccupancy();

	//int32_t slot = _allocator->Allocate(3);
	//memset(_allocator->Data(slot), 0, _allocator->BlockSize() * 3);
	//_allocator->PrintOccupancy();

	//slot = _allocator->Allocate(2);
	//memset(_allocator->Data(slot), 0, _allocator->BlockSize() * 2);
	//_allocator->PrintOccupancy();

	//slot = _allocator->Allocate(15);
	//memset(_allocator->Data(slot), 0, _allocator->BlockSize() * 15);
	//_allocator->PrintOccupancy();

	//_allocator->Free(2, 3);
	//_allocator->PrintOccupancy();

	//_allocator->Free(7, 1);
	//_allocator->PrintOccupancy();

	//_allocator->Free(5, 1);
	//_allocator->PrintOccupancy();

	//_allocator->Free(0, 1);
	//_allocator->PrintOccupancy();

	//_allocator->Free(19, 1);
	//_allocator->PrintOccupancy();

	//slot = _allocator->Allocate(2);
	//memset(_allocator->Data(slot), 0, _allocator->BlockSize() * 2);
	//_allocator->PrintOccupancy();

	//_allocator->Free(9, 3);
	//_allocator->PrintOccupancy();

	//_allocator->Free(14, 3);
	//_allocator->PrintOccupancy();

	//slot = _allocator->Allocate(3);
	//memset(_allocator->Data(slot), 0, _allocator->BlockSize() * 3);
	//_allocator->PrintOccupancy();
}

void ResourceManager::Startup()
{
	_runningThread = thread(&ResourceManager::_Run, this);
}


void ResourceManager::SetAssetLoader(IAssetLoader * loader)
{
	_assetLoader = loader;
}

void ResourceManager::AddParser(const std::string& fileend, const std::function<void(Resource::Ptr& resource)>& parseFunction)
{
	uint32_t type = std::hash<std::string>{} (fileend);
	uint8_t index = _assetLoader->AddType(type);
	_parser.AddParser(index, parseFunction);
	
}

void ResourceManager::SetEvictPolicy(const std::function<bool(uint32_t sizeOfLoadRequest, ResourceManager*rm)>& evictPolicy)
{
	_mutexLockGeneral.lock();
	_WhatToEvict = evictPolicy;
	_mutexLockGeneral.unlock();
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
		//_mutexLockGeneral.lock();
		////Loop through all resources, removing the first, and only one, resource available to be removed.
		//uint64_t erased = 0;
		//auto r = _resources;
		//while (r)
		//{
		//	if (r->resource._refCount == 0 && r->resource.GetState() == Resource::ResourceState::Loaded && !(r->resource._flags & Resource::Flag::PERSISTENT))
		//	{
		//		printf("Unloading resource. GUID: %llu.\n", r->resource.ID.data);
		//		_RemoveResource(r);
		//		break;
		//	}
		//	r = r->next;
		//}
		//_mutexLockGeneral.unlock();

		//Taking a nap.
		//this_thread::sleep_for(std::chrono::milliseconds(17));
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
			uint32_t job = _loadingQueue.top();
			_loadingQueue.pop();
			_mutexLockLoadingQueue.unlock();

			const auto& data = _resource.data;

			//Proudly write out what GUID we have started working on.
			SM_GUID guid = data.guid[job];
			printf("Started loading resource. GUID: %llu\n", guid.data);


			//Lock the loader so we can work in peace and quiet.
			_mutexLockLoader.lock();

			//Call asset loader to load the data we want
			uint32_t startBlock = 0;
			uint32_t numBlocks = 0;

			try
			{
				RawData rawData = _assetLoader->LoadResource(guid, [this, &startBlock, &numBlocks](uint32_t dataSize) -> char*
				{
					char* data = nullptr;

					numBlocks = static_cast<uint32_t>(ceilf(static_cast<float>(dataSize) / _allocator->BlockSize()));
					startBlock = _allocator->Allocate(numBlocks);

					if (startBlock != -1)
					{
						data = _allocator->Data(startBlock);
					}

					return data;
				});


				_mutexLockLoader.unlock();

				printf("Finished loading resource. GUID: %llu\n", guid.data);

				data.rawData[job] = rawData.data;
				data.type[job] = rawData.fType;
				data.size[job] = rawData.size;
				data.startBlock[job] = startBlock;
				data.numBlocks[job] = numBlocks;

				//Lock so we can insert the data to the resources
				_mutexLockParserQueue.lock();
				_parserQueue.push(job);
				_mutexLockParserQueue.unlock();
			}
			catch (std::runtime_error& e)
			{
				//We don't have enough memory. Wait one "sleep", but push the job back onto the queue for a new try.
				printf("Resource Manager out of memory...\n");


				if (_WhatToEvict(numBlocks, this))
				{
					_mutexLockLoadingQueue.lock();
					_loadingQueue.push(job);
					_mutexLockLoadingQueue.unlock();
				}
				else
				{
					_resource.Remove(job);
					printf("\tCould not find a resource to evict.\n\n");
				}
					
								
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

			uint32_t job = _parserQueue.top();
			_parserQueue.pop();
			_mutexLockParserQueue.unlock();

			auto& data = _resource.data;
			SM_GUID guid = data.guid[job];

			//Mark it as parsed, notify the user and start parsing it.
			printf("Starting parsing resource. GUID: %llu\n", guid.data);
			
			_parser.ParseResource(_resource.MakePtr(job));
			
			data.pinned[job] = false;

			//The resource is now loaded and marked as such, the used is notified.
			printf("Finished parsing resource. GUID: %llu\n", guid.data);

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
	uint32_t nrb = maxMemory / (Resource::Size + ChunkyAllocator::BlockSize());

	_resource.Allocate(nrb);
	_allocator = new ChunkyAllocator(nrb);

	_WhatToEvict = EvictPolicies::NoEvict;
}

void ResourceManager::ShutDown()
{
	_running = false;
	_runningThread.join();

	delete _assetLoader;
	_assetLoader = nullptr;

	_resource.UnAllocte();
	delete _allocator;
}