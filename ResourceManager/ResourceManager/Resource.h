#ifndef RESOURCE_H
#define RESOURCE_H

#include "Observer.h"
#include <vector>
#include "flags.h"
#include "Types.h"

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
	ObjectType _objectType;
	FileType _fileType;
	Resource() : _refCount(0) { };
	char* _rawData;

	void SetGUID(SM_GUID inID) { ID = inID; };

public:
	void _NotifyObserver() { for (auto &it : observers) { it->Notify(ID); } };
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