#ifndef RESOURCE_H
#define RESOURCE_H

#include "Observer.h"
#include <vector>
#include "flags.h"
#include "Types.h"
#include "IAssetLoader.h"
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


	enum ResourceState : uint32_t
	{
		Waiting,
		Loading,
		Parsing,
		Loaded
	};

	friend ResourceManager;
	
private:
	std::vector<Observer*> observers;
	SM_GUID ID;
	Flag _flags;
	uint16_t _refCount;
	uint16_t _callCount;
	ResourceState _state;
	Resource() : _refCount(0), _callCount(0), observers(std::vector<Observer*>())
	{

	};
	Resource(SM_GUID id, const Flag& flag)
	{
		ID = id;
		_flags = flag;
	}
	~Resource() 
	{
		for (auto &it : observers)
		{
			it->NotifyDelete(ID);
		}
	}
	RawData _rawData;
	uint32_t _startBlock;
	uint32_t _numBlocks;
	void SetGUID(SM_GUID inID) { ID = inID; };
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
	RawData GetData() 
	{
		_SetDataLock.lock();
		RawData returnd = _rawData;
		_SetDataLock.unlock();
		return returnd;
	};
	void SetState(ResourceState state)
	{
		_StateLock.lock();
		_state = state;
		_StateLock.unlock();
	}
	ResourceState GetState()
	{
		_StateLock.lock();
		ResourceState s = _state;
		_StateLock.unlock();
		return s;
	};
	void SetData(RawData data, uint32_t startBlock, uint32_t numBlocks) 
	{
		_SetDataLock.lock();
		_rawData = data;
		_startBlock = startBlock;
		_numBlocks = numBlocks;
		_SetDataLock.unlock();
	};

	operator SM_GUID()const { return ID; }
};




#endif
