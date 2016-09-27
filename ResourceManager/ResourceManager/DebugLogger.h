#ifndef _DEBUG_LOGGER_H_
#define _DEBUG_LOGGER_H_

#include <list>
#include <string>

class DebugLogger
{
private:
	static DebugLogger* _instance;
	std::list<std::string> _errorLog;
	DebugLogger();
	~DebugLogger();
public:
	static DebugLogger* GetInstance();
	void AddMsg(std::string msg);
	void Dump();
};

#endif
