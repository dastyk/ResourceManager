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
	ResourceManager::Instance(); // Kickstart at our will
	ResourceManager::Instance().SetAssetLoader(new ZipLoader("Spheres.zip"));

	Sleep(10);
	//ResourceManager::Instance().PrintOccupancy();
	ResourceManager::Instance().TestAlloc();

	Resource& mesh1 = ResourceManager::Instance().LoadResource("Sphere0.arf", Resource::Flag::LOAD_RIGHT_THE_FUCK_NOW);
	ResourceManager::Instance().ShutDown();
	MemoryManager::DeleteInstance();
	Core::ShutDown();
	
	DebugLogger::GetInstance()->Dump();
	return 0;
}