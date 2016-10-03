#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <string>
#include "ResourceManager.h"
#include "MemoryManager.h"
#include <crtdbg.h>
#include "ZipLoader.h"
#include "MeshData.h"



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
		// Setup pointers
		RawData* rdata = (RawData*)r.GetData();
		ArfData::Data* data = (ArfData::Data*)rdata->data;
		ArfData::DataPointers _datap;
		void* pdata = (void*)((size_t)data + sizeof(ArfData::Data));
		_datap.positions = (ArfData::Position*)((size_t)pdata + data->PosStart);
		_datap.texCoords = (ArfData::TexCoord*)((size_t)pdata + data->TexStart);
		_datap.normals = (ArfData::Normal*)((size_t)pdata + data->NormStart);
		_datap.faces = (ArfData::Face*)((size_t)pdata + data->FaceStart);
		_datap.subMesh = (ArfData::SubMesh*)((size_t)pdata + data->SubMeshStart);


		// Interleave data
		MeshData::MeshData* pmdata = new MeshData::MeshData;
		MeshData::MeshData& mdata = *pmdata;
		mdata.NumVertices = data->NumFace * 3;
		mdata.vertices = new MeshData::Vertex[mdata.NumVertices];
		uint32_t index = 0;
		for (uint32_t i = 0; i < data->NumSubMesh; i++)
		{
			for (uint32_t j = _datap.subMesh[i].faceStart; j < _datap.subMesh[i].faceCount; j++)
			{
				auto& face = _datap.faces[j];

				for (uint8_t r = 0; r < face.indexCount; r++)
				{
					
					auto& ind = face.indices[r];
					// Positions
					memcpy(&mdata.vertices[index].pos, &_datap.positions[ind.index[0]], sizeof(MeshData::Position));
					// Normals
					memcpy(&mdata.vertices[index].norm, &_datap.normals[ind.index[2]], sizeof(MeshData::Normal));
					// TexCoords
					memcpy(&mdata.vertices[index].tex, &_datap.texCoords[ind.index[1]], sizeof(MeshData::TexCoord));
					
					index++;
				}
				
			}
		}

		mdata.IndexCount = data->NumFace * 3;
		mdata.Indices = new uint32_t[mdata.IndexCount];
		for (uint32_t i = 0; i < mdata.IndexCount; i++)
		{
			mdata.Indices[i] = i;
		}


		// Save parsed data.
		operator delete(rdata->data);
		delete rdata;
		r.SetData(pmdata);


		Core::GetInstance()->GetGraphics()->CreateMeshBuffers(r);

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