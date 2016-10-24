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
#include "DarferLoader.h"
#include "FileLoader.h"
#include "Scene.h"

void ArfParser(const Resource::Ptr& resource);
void Objarser(const Resource::Ptr& resource);



std::string bytesToString(uint32_t freeMemory)
{
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
}

int main(int argc, char** argv)
{
	srand(time(NULL));

	if (!AllocConsole()) throw std::runtime_error("Failed to alloc console.");
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	printf("<----||Console Initialized||---->\n\n");

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Flex/Bison causes 3 memory leaks per run time, does not increase during runtime.
	//_crtBreakAlloc = 312;
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);
	MemoryManager::CreateInstance();
	MemoryManager::GetInstance()->Init(1024U * 1024U * 1024U);
	ResourceManager& r = ResourceManager::Instance(); // Kickstart at our will
	r.Init(512U * 1024U * 1024U);
	r.SetEvictPolicy(ResourceManager::EvictPolicies::FirstFit);
	//r.SetAssetLoader(new ZipLoader("data.dat"));
	//r.SetAssetLoader(new FileLoader("filelist.txt"));
	r.SetAssetLoader(new DarferLoader("data.drf"));
	r.AddParser("jpg",
		[](const Resource::Ptr& resource)
	{
		auto g = Core::GetInstance()->GetGraphics();
		g->CreateShaderResource(resource.guid, resource.data, resource.size);
		resource.RegisterObserver(g);
	});
	r.AddParser("png",
		[](const Resource::Ptr& resource)
	{
		auto g = Core::GetInstance()->GetGraphics();
		g->CreateShaderResource(resource.guid, resource.data, resource.size);
		resource.RegisterObserver(g);
	});

	r.AddParser("arf", ArfParser);

	r.AddParser("obj", Objarser);

	r.Startup();



	r.TestAlloc();

	//DarferLoader drfLoader("test.drf");



	//Resource& tex1 = ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::LOAD_AND_WAIT);
	//ResourceManager::Instance().LoadResource("gold.jpg", Resource::Flag::LOAD_AND_WAIT);
	// For some reason if you removed the line above this comment, the LOD textures all turn black.

	//ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::LOAD_AND_WAIT);



	GameObject sphereObject;
	sphereObject.AddLODMesh("Sphere0.arf");
	sphereObject.AddLODMesh("Sphere1.arf");
	sphereObject.AddLODMesh("Sphere2.arf");
	sphereObject.AddLODMesh("Sphere3.arf");
	sphereObject.AddLODMesh("Sphere4.arf");
	sphereObject.AddLODMesh("Sphere5.arf");

	sphereObject.AddLODTexture("b0.jpg");
	sphereObject.AddLODTexture("b1.jpg");
	sphereObject.AddLODTexture("b2.jpg");
	sphereObject.AddLODTexture("b3.jpg");
	sphereObject.AddLODTexture("b4.jpg");
	sphereObject.AddLODTexture("b5.jpg");

	DirectX::XMStoreFloat4x4(&sphereObject.transform, DirectX::XMMatrixScaling(0.6,0.6,0.6) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 10.0f));
	sphereObject.pos = DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f);
	sphereObject.scale = 0.6f;
	sphereObject.radius = 0.8f;

	Scene testScene;
	testScene.AddGameObject(sphereObject);

	GameObject bunnyObject;
	bunnyObject.AddLODMesh("Bunny0.arf");
	bunnyObject.AddLODMesh("Bunny1.arf");
	bunnyObject.AddLODMesh("Bunny2.arf");
	bunnyObject.AddLODMesh("Bunny3.arf");

	bunnyObject.AddLODTexture("blue.png");

	DirectX::XMStoreFloat4x4(&bunnyObject.transform, DirectX::XMMatrixScaling(10, 10, 10) * DirectX::XMMatrixRotationY(DirectX::XM_PI)* DirectX::XMMatrixTranslation(-10.0f, 0.0f, 10.0f));
	bunnyObject.pos = DirectX::XMFLOAT3(-10.0f, 0.0f, 10.0f);
	bunnyObject.scale = 10.0f;
	bunnyObject.radius = 0.3f;

	testScene.AddGameObject(bunnyObject);


	GameObject dragonObject;
	dragonObject.AddLODMesh("Dragon0.arf");
	dragonObject.AddLODMesh("Dragon1.arf");
	dragonObject.AddLODMesh("Dragon2.arf");

	dragonObject.AddLODTexture("blue.png");

	DirectX::XMStoreFloat4x4(&dragonObject.transform, DirectX::XMMatrixScaling(0.3, 0.3, 0.3) * DirectX::XMMatrixTranslation(10.0f, 0.0f, 10.0f));
	dragonObject.pos = DirectX::XMFLOAT3(10.0f, 00.0f, 10.0f);
	dragonObject.scale = 0.25f;
	dragonObject.radius = 0.3f;

	testScene.AddGameObject(dragonObject);


	GameObject cat;
	cat.AddLODMesh("cat.arf");
	cat.AddLODTexture("cat_diff.jpg");

	cat.pos = DirectX::XMFLOAT3(10.0f, 00.0f, 5.0f);
	cat.scale = 1.0f;
	cat.radius = 1.0f;
	DirectX::XMStoreFloat4x4(&cat.transform, DirectX::XMMatrixScaling(cat.scale, cat.scale, cat.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&cat.pos)));
	
	testScene.AddGameObject(cat);

	GameObject doge;
	doge.AddLODMesh("Doge.arf");
	doge.AddLODTexture("Tex_0552_7.jpg");

	doge.pos = DirectX::XMFLOAT3(-10.0f, 00.0f, 5.0f);
	doge.scale = 0.025f;
	doge.radius = 1.0f;
	DirectX::XMStoreFloat4x4(&doge.transform, DirectX::XMMatrixScaling(doge.scale, doge.scale, doge.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&doge.pos)));

	testScene.AddGameObject(doge);

	GameObject earth;
	earth.AddLODMesh("Sphere3.arf");
	earth.AddLODTexture("4096_earth.jpg");

	earth.pos = DirectX::XMFLOAT3(-10.0f, 00.0f, -5.0f);
	earth.scale = 0.75f;
	earth.radius = 1.0f;
	DirectX::XMStoreFloat4x4(&earth.transform, DirectX::XMMatrixScaling(earth.scale, earth.scale, earth.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&earth.pos)));

	testScene.AddGameObject(earth);


	GameObject Astro;
	Astro.AddLODMesh("Astronaut.arf");
	Astro.AddLODTexture("Spacesuit_D.png");

	Astro.pos = DirectX::XMFLOAT3(-10.0f, 0.55f, -5.0f);
	Astro.scale = 0.75f;
	Astro.radius = 1.0f;
	DirectX::XMStoreFloat4x4(&Astro.transform, DirectX::XMMatrixScaling(Astro.scale, Astro.scale, Astro.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Astro.pos)));

	testScene.AddGameObject(Astro);


	GameObject cube;
	cube.mesh = "Cube.arf";
	cube.texture = "tileable-light-wood-textures-5.jpg";

	cube.pos = DirectX::XMFLOAT3(0.0f, -1.5f, 0.0f);
	cube.scale = 0.75f;
	cube.radius = 1.0f;
	DirectX::XMStoreFloat4x4(&cube.transform, DirectX::XMMatrixScaling(100.0f, cube.scale, 100.0f) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&cube.pos)));

	//testScene.AddGameObject(cube);

	// 50 50 50
	//sphereObject.pos = DirectX::XMFLOAT3(45.0f, 50.0f, 50.0f);
	//sphereObject.scale = 0.4f;
	//DirectX::XMStoreFloat4x4(&sphereObject.transform, DirectX::XMMatrixScaling(sphereObject.scale, sphereObject.scale, sphereObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&sphereObject.pos)));
	//testScene.AddGameObject(sphereObject);
	//sphereObject.pos = DirectX::XMFLOAT3(47.0f, 50.0f, 50.0f);
	//sphereObject.scale = 0.4f;
	//DirectX::XMStoreFloat4x4(&sphereObject.transform, DirectX::XMMatrixScaling(sphereObject.scale, sphereObject.scale, sphereObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&sphereObject.pos)));
	//testScene.AddGameObject(sphereObject);
	//sphereObject.pos = DirectX::XMFLOAT3(49.0f, 50.0f, 50.0f);
	//sphereObject.scale = 0.4f;
	//DirectX::XMStoreFloat4x4(&sphereObject.transform, DirectX::XMMatrixScaling(sphereObject.scale, sphereObject.scale, sphereObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&sphereObject.pos)));
	//testScene.AddGameObject(sphereObject);

	//bunnyObject.pos = DirectX::XMFLOAT3(45.0f, 52.0f, 50.0f);
	//bunnyObject.scale = 1.8f;
	//DirectX::XMStoreFloat4x4(&bunnyObject.transform, DirectX::XMMatrixScaling(bunnyObject.scale, bunnyObject.scale, bunnyObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&bunnyObject.pos)));
	//testScene.AddGameObject(bunnyObject);
	//bunnyObject.pos = DirectX::XMFLOAT3(47.0f, 52.0f, 50.0f);
	//bunnyObject.scale = 1.8f;
	//DirectX::XMStoreFloat4x4(&bunnyObject.transform, DirectX::XMMatrixScaling(bunnyObject.scale, bunnyObject.scale, bunnyObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&bunnyObject.pos)));
	//testScene.AddGameObject(bunnyObject);
	//bunnyObject.pos = DirectX::XMFLOAT3(49.0f, 52.0f, 50.0f);
	//bunnyObject.scale = 1.8f;
	//DirectX::XMStoreFloat4x4(&bunnyObject.transform, DirectX::XMMatrixScaling(bunnyObject.scale, bunnyObject.scale, bunnyObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&bunnyObject.pos)));
	//testScene.AddGameObject(bunnyObject);


	r.LoadResource("Cube.arf", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);
	r.LoadResource("tileable-light-wood-textures-5.jpg", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);


	printf("<----||Starting Game loop||---->\n\n");
	InputManager* input = core->GetInputManager();
	std::vector<GameObject> renderObjects;
	core->GetWindow()->LockMouseToScreen(true);
	while(!input->WasKeyPressed(SDLK_ESCAPE))
	{
		
		//Check event to handle input
		if (input->WasKeyPressed(SDLK_k))
			core->GetWindow()->ToggleLockMouseToScreen();

		if (input->IsKeyDown(SDLK_w))
			core->GetCameraManager()->MoveForward(4.0f * core->GetTimer()->GetDeltaTime());

		if (input->IsKeyDown(SDLK_s))
			core->GetCameraManager()->MoveForward(-4.0f * core->GetTimer()->GetDeltaTime());

		if (input->IsKeyDown(SDLK_d))
			core->GetCameraManager()->MoveRight(4.0f * core->GetTimer()->GetDeltaTime());

		if (input->IsKeyDown(SDLK_a))
			core->GetCameraManager()->MoveRight(-
				4.0f * core->GetTimer()->GetDeltaTime());
		if (input->WasKeyPressed(SDLK_2))
			core->GetCameraManager()->TranslateActiveCamera(50, 50, 50);


		float xrotation = input->GetMouseXMovement() * core->GetTimer()->GetDeltaTime() * 0.01f;
		float yrotation = input->GetMouseYMovement() * core->GetTimer()->GetDeltaTime() * 0.01f;

		core->GetCameraManager()->RotatePitch(yrotation);
		core->GetCameraManager()->RotateYaw(xrotation);
		
		if (input->WasKeyPressed(SDLK_1))
		{
			sphereObject.pos = DirectX::XMFLOAT3(rand()%20 -10, rand() % 20 - 10, rand() % 20 - 10);
			sphereObject.scale = rand()%8 / 10 + 0.2f;
			sphereObject.radius = rand() % 8 / 10 + 0.2f;

			DirectX::XMStoreFloat4x4(&sphereObject.transform, DirectX::XMMatrixScaling(sphereObject.scale, sphereObject.scale, sphereObject.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&sphereObject.pos)));
			
			testScene.AddGameObject(sphereObject);
		}

		if (input->IsKeyDown(SDLK_r))
		{


			testScene.Update(core->GetCameraManager()->GetActiveCamera().position);

			renderObjects.clear();
			testScene.GetObjectsToRender(renderObjects);
		}
		core->GetGraphics()->AddToRenderQueue(cube);
		
		for (int i = 0; i < renderObjects.size(); i++)
		{
			core->GetGraphics()->AddToRenderQueue(renderObjects[i]);
		}

		core->Update();


		uint32_t maxMemory = r.MaxMemory();
		uint32_t freeMemory = r.FreeMemory();
		std::string maxMemoryStr = bytesToString(maxMemory);
		std::string freeMemoryStr = bytesToString(freeMemory);

		std::stringstream ss;
		ss << "Free memory: " << freeMemoryStr.c_str() << " / " << maxMemoryStr.c_str() << " (" << std::setprecision(1) << std::fixed << 100.0f * freeMemory / maxMemory << "%)";

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


void ArfParser(const Resource::Ptr& resource)
{
	// Setup pointers
	ArfData::Data* data = (ArfData::Data*)resource.data;
	ArfData::DataPointers _datap;
	void* pdata = (void*)((size_t)data + sizeof(ArfData::Data));
	_datap.positions = (ArfData::Position*)((size_t)pdata + data->PosStart);
	_datap.texCoords = (ArfData::TexCoord*)((size_t)pdata + data->TexStart);
	_datap.normals = (ArfData::Normal*)((size_t)pdata + data->NormStart);
	_datap.faces = (ArfData::Face*)((size_t)pdata + data->FaceStart);
	_datap.subMesh = (ArfData::SubMesh*)((size_t)pdata + data->SubMeshStart);

	// Interleave data
	uint32_t numVertices = data->NumFace * 3;
	MeshData::Vertex* vertices = new MeshData::Vertex[numVertices];
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
				memcpy(&vertices[index].pos, &_datap.positions[ind.index[0] - 1], sizeof(MeshData::Position));
				vertices[index].pos.z = -vertices[index].pos.z;
				// Normals
				memcpy(&vertices[index].norm, &_datap.normals[ind.index[2] - 1], sizeof(MeshData::Normal));
				// TexCoords
				memcpy(&vertices[index].tex, &_datap.texCoords[ind.index[1] - 1], sizeof(MeshData::TexCoord));
				vertices[index].tex.v = 1 - vertices[index].tex.v;
				index++;
			}
		}
	}

	uint32_t indexCount = data->NumFace * 3;
	uint32_t* indices = new uint32_t[indexCount];
	for (uint32_t i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}

	auto g = Core::GetInstance()->GetGraphics();
	g->CreateMeshBuffers(resource.guid, vertices, numVertices, indices, indexCount);
	resource.RegisterObserver(g);
	delete[] vertices;
	delete[] indices;
}

void Objarser(const Resource::Ptr& resource)
{
	MeshData::MeshData pdata;
	ParseObj(resource.data, pdata);

	auto g = Core::GetInstance()->GetGraphics();
	g->CreateMeshBuffers(resource.guid, pdata.vertices, pdata.NumVertices, pdata.Indices, pdata.IndexCount);
	resource.RegisterObserver(g);

	delete[] pdata.vertices;
	delete[] pdata.Indices;
}