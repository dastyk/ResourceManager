#ifndef OBSERVER_H
#define OBSERVER_H

#include "GUID.h"

class Observer
{
public:
	Observer() {};
	virtual ~Observer() {};
	
	virtual void Notify(GUID guid) = 0;
};






#endif
