#ifndef RESOURCE_H
#define RESOURCE_H

#include "Observer.h"
#include <vector>
#include "flags.h"
#include "Types.h"
#include "IAssetLoader.h"
#include <functional>
#include <mutex>

class ResourceManager;
class AssetParser;



class Resource
{
public:
	CreateFlag(Flag, uint32_t, 4,
		PERSISTENT = 1 << 0,
		NOT_URGENT = 1 << 1,
		NEEDED_NOW = 1 << 2,
		LOAD_AND_WAIT = 1 << 3
	);

	enum ResourceType : uint32_t
	{
		MESH,
		TEXTURE
		
	};

	enum ResourceState : uint32_t
	{
		Waiting,
		Loading,
		Parsing,
		Loaded
	};

	friend ResourceManager;
	friend AssetParser;
	~Resource() 
	{
		_NotifyObserver(); 
		if (_destroyFunction)
			_destroyFunction(_data);
	}
	
private:
	std::vector<Observer*> observers;
	std::function<void(void*data)> _destroyFunction;
	SM_GUID ID;
	Flag _flags;
	uint16_t _refCount;
	uint16_t _callCount;
	ResourceState _state;
	Resource() : _refCount(0), _callCount(0), observers(std::vector<Observer*>())
	{

	};
	ResourceType _resourceType;
	void* _data;
	void SetGUID(SM_GUID inID) { ID = inID; };
	void _NotifyObserver() { for (auto &it : (observers)) { it->NotifyDelete(ID); } };
	std::mutex _SetDataLock;
	std::mutex _StateLock;
	std::mutex _UnRefLock;
public:
	void IncRefCount()
	{
		_UnRefLock.lock();
		_refCount++;
		_UnRefLock.unlock();
	}
	void Unload()
	{
		_UnRefLock.lock();
		_refCount = (_refCount == 0) ? 0 : _refCount - 1;
		_UnRefLock.unlock();
	}
	void UpdateCounter(int16_t inc) 
	{
		int32_t temp = (int32_t)_callCount + (int32_t)inc;
		if (temp < INT16_MIN)
			_callCount = INT16_MIN;
		else if (temp > INT16_MAX)
			_callCount = INT16_MAX;
		else
			_callCount += inc;
	};
	void registerObserver(Observer* observer) 
	{ 
		observers.push_back(observer);
	}
	void unregisterObserver(Observer* observer)
	{
		for (auto it = observers.begin(); it != observers.end(); ++it)
		{
			if (*it == observer)
				observers.erase(it);
		}
	}
	SM_GUID GetGUID()const { return ID; };
	void* GetData() const { return _data; };
	void Destroy() { _SetDataLock.lock(); if (_destroyFunction) _destroyFunction(_data); _SetDataLock.unlock(); };
	void SetState(ResourceState state)
	{
		_StateLock.lock();
		_state = state;
		_StateLock.unlock();
	}
	ResourceState GetState()
	{
		return _state;
	};
	void SetData(void* data, const std::function<void(void*)>& dfunc) 
	{
		_SetDataLock.lock();
		_data = data; _destroyFunction = dfunc; 
		_SetDataLock.unlock();
	};
	const ResourceType GetResourceType() const { return _resourceType; };

	operator SM_GUID()const { return ID; }
};




#endif
