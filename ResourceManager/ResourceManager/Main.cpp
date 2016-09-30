#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <string>
#include "ResourceManager.h"
#include "MemoryManager.h"
#include <crtdbg.h>
#include "ZipLoader.h"




int main(int argc, char** argv)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);
	MemoryManager::CreateInstance();
	MemoryManager::GetInstance()->Init(512U * 1024U * 1024U);
	ResourceManager& r = ResourceManager::Instance(); // Kickstart at our will
	

	r.SetAssetLoader(new ZipLoader("Spheres.zip"));
	r.AddParser("arf",
		[](Resource& r) 
	{
		ArfData::Data* data = (ArfData::Data*)r._rawData.data;
		ArfData::DataPointers _datap;
		void* pdata = r._rawData.data + sizeof(ArfData::Data);
		_datap.positions = (ArfData::Position*)((size_t)pdata + data->PosStart);
		_datap.texCoords = (ArfData::TexCoord*)((size_t)pdata + data->TexStart);
		_datap.normals = (ArfData::Normal*)((size_t)pdata + data->NormStart);
		_datap.faces = (ArfData::Face*)((size_t)pdata + data->FaceStart);
		_datap.subMesh = (ArfData::SubMesh*)((size_t)pdata + data->SubMeshStart);

	
		// TODO: Do something with the data, so it can be rendered. // TODO: Interleave data.

		delete r._rawData.data;
	});

	Sleep(10);
	//ResourceManager::Instance().PrintOccupancy();
	ResourceManager::Instance().TestAlloc();

	Resource& mesh1 = ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::NEEDED_NOW);
	ResourceManager::Instance().ShutDown();
	MemoryManager::DeleteInstance();
	Core::ShutDown();
	
	DebugLogger::GetInstance()->Dump();
	return 0;
}