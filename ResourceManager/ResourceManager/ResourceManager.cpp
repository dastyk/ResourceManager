#include "ResourceManager.h"
#pragma warning (disable : 4267)
#pragma warning (disable : 4018) //Signed/unsigned missmatch
#include <stdexcept>
#include <Windows.h> // Because fuck off VS. Start following standards
#include <iostream>
#include "DebugLog.h"
#include <sstream>
#include "Core.h"

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
	//_mutexLockGeneral.lock();

	//Check to see if the GUID is already loaded, in that case see if we can update the priority and then return the Load Resource
	// (Simply put: if already loaded or in queue to be loaded, don't push it into the queue to be loaded.

	auto find = _resource.FindLock(guid);
	if (find != Resource::NotFound)
	{
		_resource.data.refCount[find]++;
		_UpdatePriority(find, flag);
		_resource.data.pinned[find].unlock();
		//_mutexLockGeneral.unlock();
		return guid;
	}
	
	//Create the resource
	_resource.Modify();
	const Resource::DataPointers& data = _resource.data;
	uint32_t count = _resource.count;
	//new(&data.pinned[count]) std::mutex();
	data.pinned[count].lock();
	data.loaded[count] = false;
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
		PrintDebugString("Resource loading. GUID: %llu\n", guid.data);

		// TODO: Query asset loader for data size -> reserve memory here -> give pointer where asset data can be stored to the asset loader

		uint32_t startBlock = 0;
		uint32_t numBlocks = 0;


		uint64_t timestamp = Core::GetInstance()->GetTimer()->GetTimeStamp();
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
		timestamp = Core::GetInstance()->GetTimer()->GetTimeStamp() - timestamp;
		PrintDebugString("Loaded in %ld time units\n", timestamp);
		data.rawData[count] = rawData.data;
		data.type[count] = rawData.fType;
		data.size[count] = rawData.size;

		_mutexLockLoader.unlock();

		_parser.ParseResource(_resource.MakePtrNoLock(count));

		data.loaded[count] = true;
		PrintDebugString("Resource finished loading. GUID: %llu\n", guid.data);
		
	}
	else
	{
		//Lock the loading queue so we can push our new resource to it.
		_mutexLockLoadingQueue.lock();
		
		//Set the state of the resource to waiting (to be loaded), print out that it is on the stack and push it to the stack
		PrintDebugString("Adding resource to toLoad stack. GUID: %llu\n", guid.data);
		if (data.flags[count] & Resource::Flag::NEEDED_NOW)
			_loadingQueueHighPrio.push(guid);
		else
			_loadingQueueLowPrio.push(guid);

		//Unlock the loading queue so we don't cause a deadlock
		_mutexLockLoadingQueue.unlock();
	}

	//Unlock the general lock and return the function with the GUID.
	data.pinned[count].unlock();
	_resource.Add();
	return guid;
}

void ResourceManager::UnloadResource(SM_GUID guid)
{

	auto found = _resource.FindLock(guid);

	if (found != Resource::NotFound)
	{
		//_resource.data.pinned[found].lock();
		auto& refCount = _resource.data.refCount[found];
		refCount = (refCount > 0) ? refCount-1 : 0;
		if(refCount == 0)
			_resource.data.timeStamp[found] = _timer.GetTimeStamp();

		_resource.data.pinned[found].unlock();
		PrintDebugString("Unreferencing resource. GUID: %llu. RefCount: %d\n", guid.data, refCount);
		
	}
	
}


void ResourceManager::_UpdatePriority(uint32_t index,const Resource::Flag& flag)
{
	auto& data = _resource.data;

	if (data.flags[index] != flag )
	{
		if (flag & (Resource::Flag::NEEDED_NOW | Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT))
		{
			data.flags[index] &= ~(Resource::Flag::NOT_URGENT);
			data.flags[index] |= flag;
			if (flag & Resource::Flag::NEEDED_NOW && data.flags[index] & Resource::Flag::NOT_URGENT)
			{
				_mutexLockLoadingQueue.lock();
				std::queue<SM_GUID> newQ;
				while (_loadingQueueLowPrio.size())
				{
					SM_GUID guid = _loadingQueueLowPrio.front();
					_loadingQueueLowPrio.pop();
					if (guid == data.guid[index])
						_loadingQueueHighPrio.push(guid);
					else
						newQ.push(guid);
				}
				_loadingQueueLowPrio = newQ;
				_mutexLockLoadingQueue.unlock();

				_mutexLockParserQueue.lock();
				std::queue<SM_GUID> newQ2;
				while (_parserQueueLowPrio.size())
				{
					SM_GUID guid = _parserQueueLowPrio.front();
					_parserQueueLowPrio.pop();
					if (guid == data.guid[index])
						_parserQueueHighPrio.push(guid);
					else
						newQ2.push(guid);
				}
				_parserQueueLowPrio = newQ2;
				_mutexLockParserQueue.unlock();
			}
		}
		else if (data.refCount[index] == 1)
		{
			data.flags[index] |= flag;
			if (data.flags[index] & Resource::Flag::NEEDED_NOW && flag & Resource::Flag::NOT_URGENT)
			{
				_mutexLockLoadingQueue.lock();
				std::queue<SM_GUID> newQ;
				while (_loadingQueueHighPrio.size())
				{
					SM_GUID guid = _loadingQueueHighPrio.front();
					_loadingQueueHighPrio.pop();
					if (guid == data.guid[index])
						_loadingQueueLowPrio.push(guid);
					else
						newQ.push(guid);
				}
				_loadingQueueHighPrio = newQ;
				_mutexLockLoadingQueue.unlock();

				_mutexLockParserQueue.lock();
				std::queue<SM_GUID> newQ2;
				while (_parserQueueHighPrio.size())
				{
					SM_GUID guid = _parserQueueHighPrio.front();
					_parserQueueHighPrio.pop();
					if (guid == data.guid[index])
						_parserQueueLowPrio.push(guid);
					else
						newQ2.push(guid);
				}
				_parserQueueHighPrio = newQ2;
				_mutexLockParserQueue.unlock();
			}
		}
	}
}

bool ResourceManager::IsLoaded(SM_GUID guid)
{
	//_mutexLockGeneral.lock();
	auto find = _resource.Find(guid);
	if (find != Resource::NotFound)
	{	
		return _resource.data.loaded[find];
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

	//int32_t slot = _allocator->Allocate(20);
	//memset(_allocator->Data(slot), 0, _allocator->BlockSize() * 20);
	//_allocator->PrintOccupancy();

	//_allocator->Free(1, 2);
	//_allocator->Free(5, 1);
	//_allocator->Free(13, 4);

	//list<pair<uint32_t, uint32_t>> defrags;

	//PrintDebugString("Before defragmenting 3 blocks at index 3:\n");
	//_allocator->PrintOccupancy();

	//defrags.push_back({ 4, 1 });
	//_allocator->Defrag(defrags);

	//PrintDebugString("After defragmenting:\n");
	//_allocator->PrintOccupancy();

	//_allocator->Allocate(3);
	//_allocator->PrintOccupancy();

	//int hej = 0;
}

void ResourceManager::Startup(uint32_t numLoadingThreads, uint32_t numParsingThreads)
{
	_timer.Update();
	_running = true;
	
	//Lock so we can initialize everything without any intereference.
	_mutexLockGeneral.lock();
	_threads.push_back(std::thread(&ResourceManager::_Run, this));

	//Create threads and initialize them as "free"
	//Both the loadingThread and the parserThread.
	for (uint16_t i = 0; i < numLoadingThreads; i++)
	{
		_threads.push_back(std::thread(&ResourceManager::_LoadingThread, this));
	}

	for (uint16_t i = 0; i < numParsingThreads; i++)
	{
		_threads.push_back(std::thread(&ResourceManager::_ParserThread, this));

	}
	
	_mutexLockGeneral.unlock();
}


const Resource::Ptr ResourceManager::GetResource(const SM_GUID & guid)
{
	uint32_t find = _resource.FindLock(guid);
	if (find != Resource::NotFound)
	{
		return _resource.MakePtrNoLock(find);
	}

	throw std::runtime_error("Tried to get a non-existing resource. GUID: " + std::to_string(guid.data));
}

void ResourceManager::ReturnResource(const Resource::Ptr & resource)
{
	_resource.DestroyPtr(resource);
}

void ResourceManager::SetAssetLoader(IAssetLoader * loader)
{
	_assetLoader = loader;
}

void ResourceManager::AddParser(const std::string& fileend, const std::function<void(const Resource::Ptr& resource)>& parseFunction)
{
	uint32_t type = std::hash<std::string>{} (fileend);
	uint8_t index = _assetLoader->AddType(type);
	_parser.AddParser(index, parseFunction);
	
}

void ResourceManager::SetEvictPolicy(evfunc evictPolicy)
{
	_mutexLockGeneral.lock();
	_WhatToEvict = evictPolicy;
	_mutexLockGeneral.unlock();
}

/*===============================================================
/*		The main "Run" thread for the Resource Manager
/*	From here, it'll loop through
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
	

	while (_running)
	{
		if (_WhatToEvict == EvictPolicies::InstantEvict)
			_WhatToEvict(0, this);

		_defragList.clear();
		_pinned.clear();

		// Find all resources with flag 'Loaded' and add them to a list of
		// allocations that are allowed to be defragmented

		auto& data = _resource.data;
		uint32_t count = _resource.count;
		for (uint32_t i = 0; i < count; i++)
		{
			if (data.pinned[i].try_lock())
			{
				if (data.numBlocks[i])
				{
					_defragList.push_back({ data.startBlock[i], data.numBlocks[i] });
					_pinned.push_back(i);
				}
				else
					data.pinned[i].unlock();
			}
		}
		uint32_t index = _allocator->Defrag(_defragList);
		if (index != UINT32_MAX)
		{
			data.rawData[_pinned[index]] = _allocator->Data(_defragList[index].first);
			//_allocator->PrintOccupancy();
			//PrintDebugString("\n\n");
		}

		for (auto& m : _pinned)
		{
			data.pinned[m].unlock();
		}

		//Taking a nap.
		this_thread::sleep_for(std::chrono::milliseconds(17));
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
void ResourceManager::_LoadingThread()
{

	//Lock the general so we can change ourselves to "in use" and "not joined"
	bool pinned = false;
	while (_running)
	{
		//We don't wish for nasty surprises.
		_mutexLockLoadingQueue.lock();

		uint32_t job = Resource::NotFound;
		if (_loadingQueueHighPrio.size())
		{
			//Get the job
			SM_GUID guid = _loadingQueueHighPrio.front();

			_loadingQueueHighPrio.pop();



			pinned = false;
			job = _resource.FindLock(guid, &pinned);

			// We found the resource but it was pinned, so add it back into the queue and try later.
			if (pinned)
				_loadingQueueHighPrio.push(guid);


		}
		if (job == Resource::NotFound && _loadingQueueLowPrio.size())
		{
			//Get the job
			SM_GUID guid = _loadingQueueLowPrio.front();

			_loadingQueueLowPrio.pop();


			pinned = false;
			job = _resource.FindLock(guid, &pinned);

			// We found the resource but it was pinned, so add it back into the queue and try later.
			if (pinned)
				_loadingQueueLowPrio.push(guid);
		}

		_mutexLockLoadingQueue.unlock();
		if (job != Resource::NotFound)
		{
			const auto& data = _resource.data;
			//Proudly write out what GUID we have started working on.
			SM_GUID guid = data.guid[job];
			PrintDebugString("Started loading resource. GUID: %llu\n", guid.data);


			//Lock the loader so we can work in peace and quiet.
			_mutexLockLoader.lock();

			//Call asset loader to load the data we want
			uint32_t startBlock = 0;
			uint32_t numBlocks = 0;

			try
			{
				//uint64_t timestamp = Core::GetInstance()->GetTimer()->GetTimeStamp();
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
				//timestamp = Core::GetInstance()->GetTimer()->GetTimeStamp() - timestamp;

				_mutexLockLoader.unlock();

				PrintDebugString("Finished loading resource. GUID: %llu\n", guid.data);
				//	PrintDebugString("Loaded in %ld time units\n", timestamp);

				data.rawData[job] = rawData.data;
				data.type[job] = rawData.fType;
				data.size[job] = rawData.size;
				data.startBlock[job] = startBlock;
				data.numBlocks[job] = numBlocks;

				//Lock so we can insert the data to the resources
				_mutexLockParserQueue.lock();
				if (data.flags[job] & Resource::Flag::NEEDED_NOW)
					_parserQueueHighPrio.push(guid);
				else
					_parserQueueLowPrio.push(guid);
				_mutexLockParserQueue.unlock();

				data.pinned[job].unlock();
			}
			catch (std::runtime_error& e)
			{
				//We don't have enough memory. Wait one "sleep", but push the job back onto the queue for a new try.
				PrintDebugString("Error: %s\n", e.what());

				
				if (_WhatToEvict(numBlocks, this))
				{
					_mutexLockLoadingQueue.lock();
					if (data.flags[job] & Resource::Flag::NEEDED_NOW)
						_loadingQueueHighPrio.push(guid);
					else
						_loadingQueueLowPrio.push(guid);
					PrintDebugString("\tAdding resource to toLoad stack. GUID: %llu\n", guid.data);
					_mutexLockLoadingQueue.unlock();
				}
				else
				{
					PrintDebugString("\tCould not find a resource to evict.\n\n");

					_resource.Modify();
					_resource.Remove(job);
				}

				data.pinned[job].unlock();
				_mutexLockLoader.unlock();
			}

		}
		std::this_thread::sleep_for(std::chrono::milliseconds(17));

	}

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
void ResourceManager::_ParserThread()
{

	bool pinned = false;
	while (_running)
	{
		//We don't wish for nasty surprises.
		_mutexLockParserQueue.lock();

		uint32_t job = Resource::NotFound;
		if (_parserQueueHighPrio.size())
		{
			//Get the job
			SM_GUID guid = _parserQueueHighPrio.front();

			_parserQueueHighPrio.pop();



			pinned = false;
			job = _resource.FindLock(guid, &pinned);

			// We found the resource but it was pinned, so add it back into the queue and try later.
			if (pinned)
				_parserQueueHighPrio.push(guid);


		}
		if (job == Resource::NotFound && _parserQueueLowPrio.size())
		{
			//Get the job
			SM_GUID guid = _parserQueueLowPrio.front();

			_parserQueueLowPrio.pop();
			

			pinned = false;
			job = _resource.FindLock(guid, &pinned);

			// We found the resource but it was pinned, so add it back into the queue and try later.
			if (pinned)
				_parserQueueLowPrio.push(guid);
		}

		_mutexLockParserQueue.unlock();

		if (job != Resource::NotFound)
		{
			
			const Resource::Ptr& resource = _resource.MakePtrNoLock(job);


			auto& data = _resource.data;
			SM_GUID guid = data.guid[job];
			PrintDebugString("Started parsing. GUID: %llu\n", guid.data);

			//Mark it as parsed, notify the user and start parsing it.
			//PrintDebugString("Starting parsing resource. GUID: %llu\n", guid.data);

			_parser.ParseResource(resource);
			data.loaded[job] = true;



			//The resource is now loaded and marked as such, the user is notified.
			//PrintDebugString("Finished parsing resource. GUID: %llu\n", guid.data);
			PrintDebugString("Finished parsing. GUID: %llu\n", guid.data);
			_resource.DestroyPtr(resource);


		}

		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}

	
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


	//When we're shutting down, we wait for our child-threads and join them in.
	for (auto& t : _threads)
		t.join();

	delete _assetLoader;
	_assetLoader = nullptr;

	if (_resource.modifyLock.try_lock())
		_resource.modifyLock.unlock();

	_resource.UnAllocte();
	delete _allocator;
}

bool ResourceManager::EvictPolicies::FirstFit(uint32_t sizeOfLoadRequest, ResourceManager * rm)
{
	uint32_t num = rm->_allocator->FreeBlocks();

	uint32_t count = rm->_resource.count;
	auto& data = rm->_resource.data;
	for (uint32_t i = 0; i < count; i++)
	{
		bool pinned = data.pinned[i].try_lock();
		if (pinned)
		{
			if (data.refCount[i] == 0 && !(data.flags[i] & Resource::Flag::PERSISTENT) && data.numBlocks[i] + num >= sizeOfLoadRequest)
			{
				PrintDebugString("\tEvicting resource. GUID; %llu\n\n", data.guid[i].data);
				rm->_resource.Modify();
				rm->_allocator->Free(data.startBlock[i], data.numBlocks[i]);
				rm->_resource.Remove(i);
				data.pinned[i].unlock();
				return true;
			}
			else
				data.pinned[i].unlock();

		}

	}
	return false;
}

bool ResourceManager::EvictPolicies::FirstCumulativeFit(uint32_t sizeOfLoadRequest, ResourceManager * rm)
{
	uint32_t num = rm->_allocator->FreeBlocks();


	uint32_t count = rm->_resource.count;
	auto& data = rm->_resource.data;
	for (uint32_t i = 0; i < count; i++)
	{
		bool pinned = data.pinned[i].try_lock();
		if (pinned)
		{
			if (data.refCount[i] == 0 && !(data.flags[i] & Resource::Flag::PERSISTENT))
			{
				PrintDebugString("\tEvicting resource. GUID; %llu\n\n", data.guid[i].data);
				num += data.numBlocks[i];
				rm->_resource.Modify();
				rm->_allocator->Free(data.startBlock[i], data.numBlocks[i]);
				rm->_resource.Remove(i);
				data.pinned[i].unlock();
				if (num >= sizeOfLoadRequest)
				{
					return true;
				}
			}
			else
				data.pinned[i].unlock();

		}

	}
	return false;
}

bool ResourceManager::EvictPolicies::InstantEvict(uint32_t sizeOfLoadRequest, ResourceManager * rm)
{
	uint32_t count = rm->_resource.count;
	auto& data = rm->_resource.data;
	for (uint32_t i = 0; i < count; i++)
	{
		if (data.pinned[i].try_lock())
		{
			if (data.refCount[i] == 0 && !(data.flags[i] & Resource::Flag::PERSISTENT))
			{
				PrintDebugString("\tEvicting resource. GUID; %llu\n\n", data.guid[i].data);

				rm->_resource.Modify();
				rm->_allocator->Free(data.startBlock[i], data.numBlocks[i]);
				rm->_resource.Remove(i);

			}
			data.pinned[i].unlock();
		}

	}
	return true;
}


bool ResourceManager::EvictPolicies::LRU(uint32_t sizeOfLoadRequest, ResourceManager * rm)
{
	std::stack<std::pair<uint64_t, uint32_t>> lrus;
	uint32_t count = rm->_resource.count;
	auto& data = rm->_resource.data;
	uint32_t num = rm->_allocator->FreeBlocks();

	for (uint32_t i = 0; i < count; i++)
	{
		if (data.pinned[i].try_lock())
		{
			if (data.refCount[i] == 0 && !(data.flags[i] & Resource::Flag::PERSISTENT) && data.numBlocks[i])
			{
				if (lrus.size())
				{
					auto plru = lrus.top();
					if (plru.first < data.timeStamp[i])
					{
						lrus.pop();
						lrus.push({ data.timeStamp[i] ,i });
						lrus.push(plru);
					}
					else
					{
						lrus.push({ data.timeStamp[i] ,i });
					}

				}
				else
				{
					lrus.push({ data.timeStamp[i] ,i });
				}
			}
			else
				data.pinned[i].unlock();
		}
	}

	bool ret = false;
	while(lrus.size() && num < sizeOfLoadRequest)
	{
		auto& lru = lrus.top();
		PrintDebugString("\tEvicting resource. GUID; %llu\n", data.guid[lru.second].data);
		rm->_resource.Modify();
		rm->_allocator->Free(data.startBlock[lru.second], data.numBlocks[lru.second]);
		num += data.numBlocks[lru.second];
		rm->_resource.Remove(lru.second);
		data.pinned[lru.second].unlock();
		lrus.pop();
		ret = true;
	}
	if(ret)
		PrintDebugString("\n");


	return ret;
}

bool ResourceManager::EvictPolicies::MRU(uint32_t sizeOfLoadRequest, ResourceManager * rm)
{

	std::stack<std::pair<uint64_t, uint32_t>> mrus;
	uint32_t count = rm->_resource.count;
	auto& data = rm->_resource.data;
	uint32_t num = rm->_allocator->FreeBlocks();

	for (uint32_t i = 0; i < count; i++)
	{
		if (data.pinned[i].try_lock())
		{
			if (data.refCount[i] == 0 && !(data.flags[i] & Resource::Flag::PERSISTENT) && data.numBlocks[i])
			{
				if (mrus.size())
				{
					auto pmru = mrus.top();
					if (pmru.first > data.timeStamp[i])
					{
						mrus.pop();
						mrus.push({ data.timeStamp[i] ,i });
						mrus.push(pmru);
					}
					else
					{
						mrus.push({ data.timeStamp[i] ,i });
					}

				}
				else
				{
					mrus.push({ data.timeStamp[i] ,i });
				}
			}
			else
				data.pinned[i].unlock();
		}
	}

	bool ret = false;
	while (mrus.size() && num < sizeOfLoadRequest)
	{
		auto& mru = mrus.top();
		PrintDebugString("\tEvicting resource. GUID; %llu\n", data.guid[mru.second].data);
		rm->_resource.Modify();
		rm->_allocator->Free(data.startBlock[mru.second], data.numBlocks[mru.second]);
		num += data.numBlocks[mru.second];
		rm->_resource.Remove(mru.second);
		data.pinned[mru.second].unlock();
		mrus.pop();
		ret = true;
	}
	if (ret)
		PrintDebugString("\n");


	return ret;
}
