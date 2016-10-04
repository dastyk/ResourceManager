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
	CreateFlag(Flag, uint32_t, 3,
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
	Resource() : _refCount(0), _callCount(0), observers(std::vector<Observer*>())
	{

	};
	ResourceType _resourceType;
	void* _data;
	void SetGUID(SM_GUID inID) { ID = inID; };
	void _NotifyObserver() { for (auto &it : (observers)) { it->NotifyDelete(ID); } };
	std::mutex CounterLock;
	std::mutex UnRefLock;
public:
	void Unload()
	{
		UnRefLock.lock();
		_refCount = (_refCount == 0) ? 0 : _refCount - 1;
		UnRefLock.unlock();
	}
	void UpdateCounter(int16_t inc) 
	{
		CounterLock.lock();
		int32_t temp = (int32_t)_callCount + (int32_t)inc;
		if (temp < INT16_MIN)
			_callCount = INT16_MIN;
		else if (temp > INT16_MAX)
			_callCount = INT16_MAX;
		else
			_callCount += inc;
		CounterLock.unlock();
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
	void Destroy() { if (_destroyFunction) _destroyFunction(_data); };
	void SetData(void* data, const std::function<void(void*)>& dfunc) { _data = data; _destroyFunction = dfunc; };
	const ResourceType GetResourceType() const { return _resourceType; };

	operator SM_GUID()const { return ID; }
};




#endif
