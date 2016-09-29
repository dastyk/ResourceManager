#include "DebugLogger.h"
#include <fstream>


DebugLogger::DebugLogger()
{

}

DebugLogger::~DebugLogger()
{

	
}

void DebugLogger::Dump()
{
	if (_errorLog.size() > 0)
	{
		std::ofstream fout("Errorlog.txt");
		for (auto &msg : _errorLog)
		{
			fout << msg << "\n";
		}
		fout.close();
	}
}

DebugLogger * DebugLogger::GetInstance()
{
	static DebugLogger instance;
	return &instance;
}

void DebugLogger::AddMsg(std::string msg)
{
	_errorLog.push_back(msg);
}
