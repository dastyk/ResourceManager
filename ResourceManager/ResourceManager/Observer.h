#ifndef OBSERVER_H
#define OBSERVER_H

#include "SM_GUID.h"

class Observer
{
public:
	Observer() {};
	virtual ~Observer() {};
	
	virtual void Notify(SM_GUID guid) = 0;
};






#endif
