#include "DebugLog.h"
#include <fstream>
#include "DebugLog.h"
#include <Windows.h>
DebugLog* DebugLog::_instance = nullptr;

DebugLog::DebugLog()
{
	if (AllocConsole())
	{
		freopen("conin$", "r", stdin);
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);

		printf("<----||Console Initialized||---->\n\n");
	}

	_running = true;
	_thread = std::thread(&DebugLog::_Run, this);

	SetThreadPriority(_thread.native_handle(), THREAD_MODE_BACKGROUND_BEGIN);
}

DebugLog::~DebugLog()
{
	_running = false;
	_thread.join();
	_Dump();
	FreeConsole();
}

const void DebugLog::_AddToQue(const std::string & data)
{
	_mutex.lock();
	_toPrint.push(data);
	_log.push_back(data);
	_mutex.unlock();
}

const void DebugLog::_Run()
{

	while (_running)
	{
		if (_toPrint.size())
		{
			_mutex.lock();
			const std::string& d = _toPrint.front();

			printf(d.c_str());

			_toPrint.pop();
			_mutex.unlock();
		}
	}

}

const void DebugLog::_Dump()
{
	if (_log.size() > 0)
	{
		std::ofstream fout("Errorlog.txt");
		for (auto &msg : _log)
		{
			fout << msg << "\n";
		}
		fout.close();
	}
}

const void DebugLog::Initialize()
{
	if(!_instance)
		_instance = new DebugLog;
}

const void DebugLog::Shutdown()
{
	delete _instance;
	_instance = nullptr;
}

const void DebugLog::PrintToConsole(const char * message, ...)
{
	if (_instance)
	{
		va_list args;
		va_start(args, message);
		int len = vsnprintf(NULL, 0, message, args) + 1;
		// instance found, add message to que.
		char* buffer = new char[len];

		vsprintf(buffer, message, args);
		va_end(args);

		_instance->_AddToQue(buffer);
		delete[] buffer;
	}
}

const void DebugLog::PrintToConsole(const wchar_t * message, ...)
{
	std::wstring wstrValue = message;

	std::string strValue;
	strValue.assign(wstrValue.begin(), wstrValue.end());  // convert wstring to string
	va_list args;							// This might
	va_start(args, message);				// not
	PrintToConsole(strValue.c_str(), args);	// work.
}


