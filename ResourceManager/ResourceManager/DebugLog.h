#ifndef _DEBUG_LOGGER_H_
#define _DEBUG_LOGGER_H_

#include <thread>
#include <vector>
#include <queue>
#include <string>
#include <mutex>

#ifndef PrintDebugString
#define PrintDebugString(message, ...) DebugLog::PrintToConsole(message, __VA_ARGS__)
#endif


class DebugLog
{
private:
	std::vector<std::string> _log;
	std::queue<std::string> _toPrint;

	static DebugLog* _instance;
	DebugLog();
	~DebugLog();

	bool _running;
	std::thread _thread;
	std::mutex _mutex;
	const void _AddToQue(const std::string& data);
	const void _Run();
	const void _Dump();
public:
	static const void Initialize();
	static const void Shutdown();
	static const void PrintToConsole(const char* message, ...);
	static const void PrintToConsole(const wchar_t* message, ...);
};

#endif
