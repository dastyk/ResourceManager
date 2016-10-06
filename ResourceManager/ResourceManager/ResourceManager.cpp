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
	_resourcePool = MemoryManager::CreatePoolAllocator(sizeof(Resource), 1337, 0);

}

ResourceManager::~ResourceManager()
{

	delete _assetLoader;
	_assetLoader = nullptr;
	
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
		find->IncRefCount();
		_mutexLockGeneral.unlock();
		return *find;
	}
		
	
	auto temp = (Resource*)_resourcePool->Malloc();
	new (temp) Resource();
	
	_resources[guid.data] = temp;
	Resource& r = *temp;
	r._refCount = 1;
	r.ID = guid;
	r._flags = flag;

	if (flag & Resource::Flag::LOAD_AND_WAIT)
	{
		_mutexLockLoader.lock();
		printf("Resource loading. GUID: %llu\n", r.GetGUID().data);
		r.SetData(_assetLoader->LoadResource(guid), [](void* data) 
		{
			operator delete(((RawData*)data)->data);
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
		printf("Unreferencing resource. GUID: %llu. RefCount: %d\n", guid.data, found->_refCount);
		found->Unload();
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
		found->_refCount = 0;
	}


}

void ResourceManager::UpdatePriority(SM_GUID guid,const Resource::Flag& flag)
{
	_mutexLockGeneral.lock();
	auto find = _FindResource(guid);
	if (find)
	{
		find->_flags = flag;


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
		bool ret = find->GetState() == Resource::ResourceState::Loaded;
		_mutexLockGeneral.unlock();
		return ret;
	}
	_mutexLockGeneral.unlock();
	return false;
}

// Little debug function that outputs occupancy of blocks where O indicates open
// and X indicates occupied.


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


Resource* ResourceManager::_FindResource(SM_GUID guid)
{
	auto& find = _resources.find(guid.data);
	if (find != _resources.end())
	{
		return find->second;
	}
	return nullptr;
}

void ResourceManager::_Run()
{
	_mutexLockGeneral.lock();
	

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
		for (auto &it : _resources)
		{
			if (it.second->_refCount == 0)
			{
				printf("Unloading resource. GUID: %llu.\n", it.second->ID.data);
				it.second->~Resource();
				_resourcePool->Free((char*)it.second);
				_resources.erase(it.second->ID.data);
				break;
			}


			
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
			void* temp = _assetLoader->LoadResource(job->GetGUID());
			_mutexLockLoader.unlock();

			job->SetState(Resource::ResourceState::Waiting);
			printf("Finished loading resource. GUID: %llu\n", job->GetGUID().data);
			//Lock so we can insert the data to the resources

			job->SetData(temp, [](void* data)
			{
				operator delete(((RawData*)data)->data);
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

	for (auto& r : _resources)
	{		
		r.second->~Resource();
	}
	_resources.clear();
	
}