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
#include "ArfData.h"
#include "DarferLoader.h"
#include "FileLoader.h"
#include "Scene.h"
#include "Parsers.h"
void ArfParser(const Resource::Ptr& resource);
void ObjParser(const Resource::Ptr& resource);



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
	r.SetEvictPolicy(ResourceManager::EvictPolicies::FirstCumulativeFit);
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

	r.AddParser("obj", ObjParser);

	r.Startup();



	r.TestAlloc();

	//for (int i = 0; i < 100; i++)
	//{
	//	ResourceManager::Instance().LoadResource("b5.jpg", Resource::Flag::LOAD_AND_WAIT);
	//	Sleep(1550);
	//	ResourceManager::Instance().UnloadResource("b5.jpg");
	//	Sleep(1550);
	//}

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
	bunnyObject.AddLODMesh("Bunny4.arf");
	r.LoadResource("Bunny4.arf", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);

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
	dragonObject.AddLODMesh("Dragon3.arf");
	dragonObject.AddLODMesh("Dragon4.arf");
	dragonObject.AddLODTexture("blue.png");
	r.LoadResource("Dragon4.arf", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);
	
	DirectX::XMStoreFloat4x4(&dragonObject.transform, DirectX::XMMatrixScaling(0.3, 0.3, 0.3) * DirectX::XMMatrixTranslation(10.0f, 0.0f, 10.0f));
	dragonObject.pos = DirectX::XMFLOAT3(10.0f, 00.0f, 10.0f);
	dragonObject.scale = 0.25f;
	dragonObject.radius = 0.3f;

	testScene.AddGameObject(dragonObject);


	GameObject cat;
	cat.AddLODMesh("Cat0.arf");
	cat.AddLODMesh("Cat1.arf");
	cat.AddLODMesh("Cat2.arf");
	cat.AddLODMesh("Cat3.arf");
	cat.AddLODMesh("Cat4.arf");
	r.LoadResource("Cat4.arf", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);

	cat.AddLODTexture("cat_diff.jpg");

	cat.pos = DirectX::XMFLOAT3(10.0f, 00.0f, 5.0f);
	cat.scale = 1.0f;
	cat.radius = 1.0f;
	DirectX::XMStoreFloat4x4(&cat.transform, DirectX::XMMatrixScaling(cat.scale, cat.scale, cat.scale) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&cat.pos)));
	
	testScene.AddGameObject(cat);

	GameObject doge;
	doge.AddLODMesh("Dog0.arf");
	doge.AddLODMesh("Dog1.arf");
	doge.AddLODMesh("Dog2.arf");
	doge.AddLODMesh("Dog3.arf");
	doge.AddLODMesh("Dog4.arf");
	r.LoadResource("Dog4.arf", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);

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
	Astro.AddLODMesh("Astronaut0.arf");
	Astro.AddLODMesh("Astronaut1.arf");
	Astro.AddLODMesh("Astronaut2.arf");
	Astro.AddLODMesh("Astronaut3.arf");
	Astro.AddLODMesh("Astronaut4.arf");
	r.LoadResource("Astronaut4.arf", Resource::Flag::PERSISTENT | Resource::Flag::LOAD_AND_WAIT);


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

void Interleave(const Resource::Ptr& resource, ArfData::Data& data, ArfData::DataPointers& datap)
{
	// Interleave data
	uint32_t numVertices = data.NumFace * 3;
	MeshData::Vertex* vertices = new MeshData::Vertex[numVertices];
	uint32_t index = 0;
	for (uint32_t i = 0; i < data.NumSubMesh; i++)
	{
		for (uint32_t j = datap.subMesh[i].faceStart; j < datap.subMesh[i].faceCount; j++)
		{
			auto& face = datap.faces[j];

			for (uint8_t r = 0; r < face.indexCount; r++)
			{
				auto& ind = face.indices[r];
				// Positions
				memcpy(&vertices[index].pos, &datap.positions[ind.index[0] - 1], sizeof(MeshData::Position));
				vertices[index].pos.z = -vertices[index].pos.z;
				// Normals
				memcpy(&vertices[index].norm, &datap.normals[ind.index[2] - 1], sizeof(MeshData::Normal));
				// TexCoords
				if (ind.index[1] == UINT32_MAX)
				{
					memcpy(&vertices[index].tex, &datap.texCoords[0], sizeof(MeshData::TexCoord));
					vertices[index].tex.v = 1 - vertices[index].tex.v;
				}
				else
				{
					memcpy(&vertices[index].tex, &datap.texCoords[ind.index[1] - 1], sizeof(MeshData::TexCoord));
					vertices[index].tex.v = 1 - vertices[index].tex.v;
				}
				index++;

			}
		}
	}

	uint32_t indexCount = data.NumFace * 3;
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
void ArfParser(const Resource::Ptr& resource)
{
	// Setup pointers
	ArfData::Data* data = (ArfData::Data*)resource.data;
	ArfData::DataPointers datap;
	datap.buffer = (void*)(data + 1);
	datap.positions = (ArfData::Position*)(datap.buffer);
	datap.texCoords = (ArfData::TexCoord*)(datap.positions + data->NumPos);
	datap.normals = (ArfData::Normal*)(datap.texCoords + data->NumTex);
	datap.faces = (ArfData::Face*)(datap.normals + data->NumNorm);
	datap.subMesh = (ArfData::SubMesh*)(datap.faces + data->NumFace);

	Interleave(resource, *data, datap);
}

void ObjParser(const Resource::Ptr& resource)
{
	ArfData::Data data;
	ArfData::DataPointers dp;
	ParseObj(resource.data, resource.size, &data, &dp);

	Interleave(resource, data, dp);
}