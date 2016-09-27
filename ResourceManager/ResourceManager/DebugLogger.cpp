#include "DebugLogger.h"
#include <fstream>

DebugLogger* DebugLogger::_instance = nullptr;

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
	if (!_instance)
		_instance = new DebugLogger();
	return _instance;
}

void DebugLogger::AddMsg(std::string msg)
{
	_errorLog.push_back(msg);
}
