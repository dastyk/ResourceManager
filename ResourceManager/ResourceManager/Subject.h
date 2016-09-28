#ifndef SUBJECT_H
#define SUBJECT_H

#include "Observer.h"
#include <vector>

class Resource
{
protected:
	std::vector<Observer*> observers;
	GUID ID;

	void _NotifyObserver() { for (auto &it : observers) { it->Notify(ID); } };

public:
	Resource() { };
	~Resource() { observers.clear(); }

	void registerObserver(Observer* observer) { observers.push_back(observer); }
	void unregisterObserver(Observer* observer)
	{ 
		for (auto it = observers.begin(); it != observers.end(); ++it)
		{ 
			if (*it == observer) 
				observers.erase(it);
		} 
	}
};




#endif
