#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "Resource.h"

// Flöjt TODO:
// Chunked allocation, i.e. make this a pool allocator with doubly linked list of
// free blocks. Allocation is a matter if going to first empty (ordered) and see if
// we can find enough blocks in a row to store a file.
// Use asset loaders and asset parsers with GUID to load.

class ResourceManager : public Observer
{
public:
	ResourceManager();
	~ResourceManager();

	void Notify(GUID guid);

private:
	ResourceManager(const ResourceManager& other);
	ResourceManager& operator=(const ResourceManager& rhs);

private:

};

#endif