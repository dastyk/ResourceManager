#ifndef _IGRAPHICS_H_
#define _IGRAPHICS_H_

#include "Observer.h"
#include "SM_GUID.h"
#include "Resource.h"

class IGraphics : public Observer
{
public:
	IGraphics() {};
	virtual ~IGraphics() {};

	virtual void Notify(SM_GUID guid) = 0;

	virtual void Draw() = 0;
	virtual void CreateBuffer(Resource* resource) = 0;

};

#endif
