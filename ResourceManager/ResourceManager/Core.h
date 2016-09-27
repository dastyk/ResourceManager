#ifndef _CORE_H_
#define _CORE_H_
#include <SDL.h>
#include <vector>
#include "Window.h"
#include "Direct3D11.h"
#include "CameraManager.h"
#include "Timer.h"

//Singleton
class Core
{
private:
	Core();
	~Core();
	static Core* _instance;
	Window* _window;
	Direct3D11* _d3d11;
	CameraManager* _cameraManager;

	Timer* _timer;

public:
	static void CreateInstance();
	static Core* GetInstance();
	static void ShutDown();
	void Init(uint32_t width, uint32_t height, bool fullscreen);
	void Update();

	Window* GetWindow() const;
	Direct3D11* GetDirect3D11() const;
	CameraManager* GetCameraManager() const;
	Timer* GetTimer() const;



	
};


#endif