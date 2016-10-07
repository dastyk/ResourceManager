#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <iomanip>
#include <string>
#include "ResourceManager.h"
#include "MemoryManager.h"
#include <crtdbg.h>
#include "ZipLoader.h"

#include "MeshData.h"
#include "TextureData.h"
#include "flexbison\ObjParser.h"
#include "ArfData.h"

#include "Scene.h"


int main(int argc, char** argv)
{
	srand(time(NULL));

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Flex/Bison causes 3 memory leaks per run time, does not increase during runtime.
	//_crtBreakAlloc = 312;
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);
	MemoryManager::CreateInstance();
	MemoryManager::GetInstance()->Init(512U * 1024U * 1024U);
	ResourceManager& r = ResourceManager::Instance(); // Kickstart at our will
	

	r.SetAssetLoader(new ZipLoader("data.dat"));

	r.AddParser("jpg",
		[](Resource& r)
	{
		RawData* temp = (RawData*)r.GetData();
		TextureData* texd = new TextureData;
		texd->size = temp->size;
		texd->data = temp->data;

		delete temp;
		r.SetData(texd, [](void* data) 
		{
			delete ((TextureData*)data)->data;
			delete data;
		});

		Core::GetInstance()->GetGraphics()->CreateShaderResource(r);
	});

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
					memcpy(&mdata.vertices[index].pos, &_datap.positions[ind.index[0]-1], sizeof(MeshData::Position));
					// Normals
					memcpy(&mdata.vertices[index].norm, &_datap.normals[ind.index[2]-1], sizeof(MeshData::Normal));
					// TexCoords
					memcpy(&mdata.vertices[index].tex, &_datap.texCoords[ind.index[1]-1], sizeof(MeshData::TexCoord));
					
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
		r.Destroy();
		r.SetData(pmdata, [](void* data) 
		{
			MeshData::MeshData* pmdata = (MeshData::MeshData*)data;
			delete[] pmdata->vertices;
			delete[] pmdata->Indices;
			delete data;
		});


		Core::GetInstance()->GetGraphics()->CreateMeshBuffers(r);
	});

	r.AddParser("obj", [](Resource& r)
	{
		MeshData::MeshData* pdata = new MeshData::MeshData;
		RawData* rdata = (RawData*)r.GetData();
		ParseObj(rdata->data, *pdata);


		//// Save parsed data.
		r.Destroy();
		r.SetData(pdata, [](void* data)
		{
			MeshData::MeshData* pmdata = (MeshData::MeshData*)data;
			delete[] pmdata->vertices;
			delete[] pmdata->Indices;
			delete data;
		});
		
		Core::GetInstance()->GetGraphics()->CreateMeshBuffers(r);
	});

	r.Startup();


	if (!AllocConsole()) throw std::runtime_error("Failed to alloc console.");
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	printf("<----||Console Initialized||---->\n\n");





	//Resource& tex1 = ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::LOAD_AND_WAIT);
	ResourceManager::Instance().LoadResource("gold.jpg", Resource::Flag::LOAD_AND_WAIT);
	// For some reason if you removed the line above this comment, the LOD textures all turn black.





	GameObject gg;
	gg.AddLODMesh("Sphere0.arf");
	gg.AddLODMesh("Sphere1.arf");
	gg.AddLODMesh("Sphere2.arf");
	gg.AddLODMesh("Sphere3.arf");
	gg.AddLODMesh("Sphere4.arf");
	gg.AddLODMesh("Sphere5.arf");

	gg.AddLODTexture("b0.jpg");
	gg.AddLODTexture("b1.jpg");
	gg.AddLODTexture("b2.jpg");
	gg.AddLODTexture("b3.jpg");
	gg.AddLODTexture("b4.jpg");
	gg.AddLODTexture("b5.jpg");

	DirectX::XMStoreFloat4x4(&gg.transform, DirectX::XMMatrixScaling(0.6,0.6,0.6) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 10.0f));
	gg.pos = DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f);
	gg.scale = 0.6f;
	gg.radius = 0.8f;

	Scene testScene;
	testScene.AddGameObject(gg);

	printf("<----||Starting Game loop||---->\n\n");
	InputManager* input = core->GetInputManager();
	std::vector<GameObject> renderObjects;
	while(!input->WasKeyPressed(SDLK_ESCAPE))
	{
		
		//Check event to handle input
		if (input->WasKeyPressed(SDLK_k))
			printf("jim was here");

		if (input->IsKeyDown(SDLK_w))
			core->GetCameraManager()->MoveForward(4.0f * core->GetTimer()->GetDeltaTime());

		if (input->IsKeyDown(SDLK_s))
			core->GetCameraManager()->MoveForward(-4.0f * core->GetTimer()->GetDeltaTime());

		if (input->IsKeyDown(SDLK_d))
			core->GetCameraManager()->RotateActiveCamera(0.0f, 0.01f * core->GetTimer()->GetDeltaTime(), 0.0f);

		if (input->IsKeyDown(SDLK_a))
			core->GetCameraManager()->RotateActiveCamera(0.0f, -0.01f * core->GetTimer()->GetDeltaTime(), 0.0f);

		if (input->WasKeyPressed(SDLK_1))
		{
			gg.pos = DirectX::XMFLOAT3(rand()%20 -10, rand() % 20 - 10, rand() % 20 - 10);
			gg.scale = rand()%8 / 10 + 0.2f;
			gg.radius = rand() % 8 / 10 + 0.2f;

			DirectX::XMStoreFloat4x4(&gg.transform, DirectX::XMMatrixScaling(gg.scale, gg.scale, gg.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&gg.pos)));
			
			testScene.AddGameObject(gg);
		}

		testScene.Update(core->GetCameraManager()->GetActiveCamera().position);

		renderObjects.clear();
		testScene.GetObjectsToRender(renderObjects);


		for (int i = 0; i < renderObjects.size(); i++)
		{
			core->GetGraphics()->AddToRenderQueue(renderObjects[i]);
		}

		//core->GetGraphics()->AddToRenderQueue(gg);
		core->Update();

	//	ResourceManager::Instance().LoadResource("Sphere5.arf", Resource::Flag::NEEDED_NOW);
		//ResourceManager::Instance().LoadResource("Sphere4.arf", Resource::Flag::NEEDED_NOW);
		//ResourceManager::Instance().LoadResource("Sphere3.arf", Resource::Flag::NEEDED_NOW);
		//ResourceManager::Instance().LoadResource("Sphere2.arf", Resource::Flag::NEEDED_NOW);
		//ResourceManager::Instance().LoadResource("Sphere1.arf", Resource::Flag::NEEDED_NOW);

		auto bytesToString = [](uint32_t freeMemory) -> std::string {
			// http://stackoverflow.com/questions/3758606
			std::stringstream ss;
			const uint32_t base = 1024;

			if (freeMemory < base)
			{
				ss << freeMemory << " B";
			}
			else
			{
				uint32_t exp = static_cast<uint32_t>(log(freeMemory) / log(base));
				char unit = std::string("kMGTPE").at(exp - 1);
				ss << std::setprecision(1) << std::fixed << freeMemory / pow(base, exp) << " " << unit << "B";
			}

			return std::move(ss.str());
		};

		uint32_t maxMemory = 512 * 1024 * 1024;
		uint32_t freeMemory = 454646548;
		std::string maxMemoryStr = bytesToString(maxMemory);
		std::string freeMemoryStr = bytesToString(freeMemory);

		std::stringstream ss;
		ss << "Free memory: " << freeMemoryStr.c_str() << " / " << maxMemoryStr.c_str() << " (" << std::setprecision(1) << std::fixed << 100.0f * freeMemory / maxMemory << "%)";
		//printf("Free memory: %s / %s (%.1f%%)\n", freeMemoryStr.c_str(), maxMemoryStr.c_str(), 100.0f * freeMemory / maxMemory);

		HWND hwnd = core->GetWindow()->GetHandle();
		SetWindowTextA(hwnd, ss.str().c_str());
	}


	printf("\n\n<----||Game loop ended||---->\n\n");


	ResourceManager::Instance().ShutDown();
	MemoryManager::DeleteInstance();
	Core::ShutDown();
	
	getchar();
	DebugLogger::GetInstance()->Dump();
	return 0;
}