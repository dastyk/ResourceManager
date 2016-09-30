#ifndef RESOURCE_H
#define RESOURCE_H

#include "Observer.h"
#include <vector>
#include "flags.h"
#include "Types.h"
#include "IAssetLoader.h"

class ResourceManager;
class AssetParser;



class Resource
{
public:
	CreateFlag(Flag, uint32_t, 3,
		PERSISTENT = 1 << 0,
		FUZZY = 1 << 1,
		LOAD_RIGHT_THE_FUCK_NOW = 1 << 2
	);

	friend ResourceManager;
	friend AssetParser;
	~Resource() { observers.clear(); }

	// ResourceData* data;
private:
	
	std::vector<Observer*> observers;
	SM_GUID ID;
	Flag _flags;
	uint16_t _refCount;
	uint16_t _callCount;
	Resource() : _refCount(0), _callCount(0) { };
	RawData _rawData;

	void SetGUID(SM_GUID inID) { ID = inID; };
	void _NotifyObserver() { for (auto &it : observers) { it->Notify(ID); } };

public:
	void UpdateCounter(bool used) {
		if (used)
			_callCount+=2;
		else
		{
			if (_callCount != 0)
			{
				_callCount--;
			}
		}
	};
	
	void registerObserver(Observer* observer) { observers.push_back(observer); }
	void unregisterObserver(Observer* observer)
	{
		for (auto it = observers.begin(); it != observers.end(); ++it)
		{
			if (*it == observer)
				observers.erase(it);
		}
	}
	SM_GUID GetGUID()const { return ID; };

	operator SM_GUID()const { return ID; }
};




#endif
