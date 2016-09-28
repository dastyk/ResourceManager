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

ResourceManager * ResourceManager::GetInstance()
{
	return _instance;
}


Resource & ResourceManager::LoadResource(SM_GUID guid, const Flags& flag)
{
	// TODO: insert return statement here
	// rawData = AssetLoader->Load(SM_GUID);
	// resources.push_back(AssetParser->Parse(rawData))
	return _resources.back();
}
