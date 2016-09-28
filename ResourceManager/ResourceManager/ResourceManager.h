#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "Resource.h"
#include <stack>
#include "flags.h"

// Fl�jt TODO:
// Chunked allocation, i.e. make this a pool allocator with doubly linked list of
// free blocks. Allocation is a matter if going to first empty (ordered) and see if
// we can find enough blocks in a row to store a file.
// Use asset loaders and asset parsers with GUID to load.

CreateFlag(Flags, uint32_t, 2,
	PERSISTENT = 1 << 0,
	FUZZY = 1 << 1
);


class ResourceManager
{
public:
	static void CreateInstance();
	static void DeleteInstance();
	static ResourceManager* GetInstance();
	Resource& LoadResource(SM_GUID guid, const Flags& flag);
private:
	ResourceManager();
	~ResourceManager();
	ResourceManager(const ResourceManager& other);
	ResourceManager& operator=(const ResourceManager& rhs);

private:
	static ResourceManager* _instance;
	std::stack<Resource> _resources;
};

#endif