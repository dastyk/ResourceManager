#include "Core.h"
#include "DebugLogger.h"
#include <SDL_events.h>
#include <sstream>
#include <string>
#include "ResourceManager.h"

int main(int argc, char** argv)
{
	Core::CreateInstance();
	Core* core = Core::GetInstance();
	core->Init(800, 600, false);

	ResourceManager::CreateInstance();

	//ResourceManager::GetInstance()->LoadResource("Test/test.obj");

	ResourceManager::DeleteInstance();

	Core::ShutDown();
	DebugLogger::GetInstance()->Dump();

	

	return 0;
}