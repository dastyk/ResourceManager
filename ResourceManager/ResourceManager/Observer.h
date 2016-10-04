#ifndef OBSERVER_H
#define OBSERVER_H

#include "SM_GUID.h"

class Resource;
class Observer
{
public:
	Observer() {};
	virtual ~Observer() {};
	
	virtual void NotifyDelete(SM_GUID guid) = 0;
};






#endif
