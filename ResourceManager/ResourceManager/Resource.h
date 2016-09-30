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
		NOT_URGENT = 1 << 1,
		NEEDED_NOW = 1 << 2
	);

	enum ResourceType : uint32_t
	{
		MESH_PNT = 1 << 0,
		TEXTURE_DDS = 1 << 1,
		TEXTURE_PNG = 1 << 2,
		TEXTURE_JPG = 1 << 3
		
	};

	friend ResourceManager;
	friend AssetParser;
	~Resource() { observers.clear(); }
	RawData _rawData;
	void* _processedData;
	// ResourceData* data;
private:
	std::vector<Observer*> observers;
	SM_GUID ID;
	Flag _flags;
	uint16_t _refCount;
	uint16_t _callCount;
	Resource() : _refCount(0), _callCount(0) { };
	ResourceType _resourceType;
	
	void SetGUID(SM_GUID inID) { ID = inID; };
	void _NotifyObserver() { for (auto &it : observers) { it->Notify(ID); } };

public:
	void UpdateCounter(bool used = false) 
	{
		if (used && _callCount < UINT16_MAX-2)
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
	const RawData& GetRawData() const { return _rawData; };
	void* GetProcessedData() const { return _processedData; };
	const ResourceType GetResourceType() const { return _resourceType; };

	operator SM_GUID()const { return ID; }
};




#endif
