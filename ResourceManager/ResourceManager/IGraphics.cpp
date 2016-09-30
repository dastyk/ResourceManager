#include "IGraphics.h"

void IGraphics::Notify(SM_GUID guid)
{
}

void IGraphics::CreateBuffer(Resource * resource)
{
	resource->registerObserver(this);
}
