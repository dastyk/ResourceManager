#ifndef GUID_H
#define GUID_H

#include <stdint.h>

struct SM_GUID
{
	SM_GUID()
	{

	}

	SM_GUID(const char* c)
	{
		// SM_GUID = c.Hash();
	}
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint64_t Data4;
};


#endif