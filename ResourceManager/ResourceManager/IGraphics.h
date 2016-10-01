#ifndef _IGRAPHICS_H_
#define _IGRAPHICS_H_

#include "Observer.h"
#include "SM_GUID.h"
#include "Resource.h"
#include "Structs.h"

class IGraphics : public Observer
{
public:
	IGraphics() {};
	virtual ~IGraphics() {};

	virtual void Notify(SM_GUID guid) = 0;

	virtual void Draw() = 0;
	//CreateBuffer(Resource* ) is too generic to work. Depending on what kind of buffers/shader resource views need to be created
	//"Resource" needs to be able to hold a lot of different data structures which makes a fucking mess.
//	virtual void CreateBuffer(Resource* resource) = 0;
	virtual void CreateMeshBuffers(SM_GUID guid, PNTMeshData& meshdata) = 0;
	virtual void CreateShaderResource(Resource* resource) = 0;

};

#endif
