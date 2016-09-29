#include "ResourceManager.h"

ResourceManager* ResourceManager::_instance = nullptr;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

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
