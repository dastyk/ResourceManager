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
	SM_GUID(uint64_t data) : data(data)
	{
		
	}
	operator uint64_t() const {
		return data;
	}
	void operator=(const SM_GUID& rhs)
	{
		this->data = rhs.data;
	}
	bool operator==(const SM_GUID& rhs) const
	{
		return rhs.data == this->data;
	}


};



#endif
