#ifndef OBSERVER_H
#define OBSERVER_H

#include "SM_GUID.h"

class Resource;
class Observer
{
public:
	Observer() {};
	virtual ~Observer() {};
	
	virtual void NotifyDelete(Resource& r) = 0;
};






#endif
