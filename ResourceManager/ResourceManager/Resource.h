#ifndef RESOURCE_H
#define RESOURCE_H

#include "Observer.h"
#include <vector>

class Resource
{
protected:
	std::vector<Observer*> observers;
	SM_GUID ID;

	
	
	Resource() { };
public:
	~Resource() { observers.clear(); }

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
	void SetID(SM_GUID inID) { ID = inID; };
	SM_GUID GetGUID()const { return ID; };

	operator SM_GUID()const { return ID; }
};




#endif
