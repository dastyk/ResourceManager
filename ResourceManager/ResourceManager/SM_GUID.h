#ifndef SM_GUID_H
#define SM_GUID_H

#include <stdint.h>
#include <string>

struct SM_GUID
{
	uint64_t data;

	SM_GUID()
	{
		data = 0;
	}

	SM_GUID(const char* c)
	{
		data = std::hash<std::string>{} (c);
	}
	operator uint64_t() {
		return data;
	}

};



#endif
