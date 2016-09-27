#include "Core.h"
#include "Macros.h"
#include <exception>
#include <map>

Core* Core::_instance = nullptr;

Core::Core()
{
	_window = nullptr;
	_d3d11 = nullptr;
	_cameraManager = nullptr;
	_timer = nullptr;
}
Core::~Core()
{

}

void Core::CreateInstance()
{
	if (!_instance)
		_instance = new Core();
}
Core* Core::GetInstance()
{
	return _instance;
}

void Core::ShutDown()
{
	SAFE_DELETE(Core::GetInstance()->_window);
	SAFE_DELETE(Core::GetInstance()->_d3d11);
	SAFE_DELETE(Core::GetInstance()->_cameraManager);
	SAFE_DELETE(Core::GetInstance()->_timer);
	delete _instance;
	_instance = nullptr;
}

void Core::Init(uint32_t width, uint32_t height, bool fullscreen)
{
	_window = new Window();
	_d3d11 = new Direct3D11();
	_cameraManager = new CameraManager();
	_timer = new Timer();

}

void Core::Update()
{
	_timer->Update();
	_d3d11->Draw();
}

Window * Core::GetWindow() const
{
	return _window;
}

Direct3D11 * Core::GetDirect3D11() const
{
	return _d3d11;
}

CameraManager * Core::GetCameraManager() const
{
	return _cameraManager;
}

Timer * Core::GetTimer() const
{
	return _timer;
}

