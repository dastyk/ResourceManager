#include "StackAllocator.h"



StackAllocator::StackAllocator(void* memoryLocation, size_t stackSize) : _stack(memoryLocation), _stackSize(stackSize), _marker(0)
{
	
}

StackAllocator::~StackAllocator()
{
	
}

