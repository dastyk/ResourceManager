#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <string>
#include "ResourceManager.h"
#include "MemoryManager.h"
#include <crtdbg.h>


int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);
	MemoryManager::CreateInstance();
	MemoryManager::GetInstance()->Init(512U * 1024U * 1024U);
	ResourceManager::Instance(); // Kickstart at our will

	ResourceManager::Instance().PrintOccupancy();

	//ResourceManager::GetInstance()->LoadResource("Test/test.obj");

	MemoryManager::DeleteInstance();
	Core::ShutDown();
	DebugLogger::GetInstance()->Dump();
	ResourceManager::Instance().ShutDown();
	return 0;
}