#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <string>
#include "ResourceManager.h"
#include "MemoryManager.h"
int main(int argc, char** argv)
{
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

	

	return 0;
}