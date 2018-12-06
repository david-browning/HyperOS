#pragma once

#include "vmem_allocator.h"

class StackAllocator : public IVMemAllocator
{
	public:
	StackAllocator();

	protected:

	//Returns pointer to the BASE of the stack. Do not add anything to the pointer.
	virtual void* _allocate(_IN_SIZE_ size_t bytes);

	virtual void _deallocate(_IN_POINTER_ void* ptr);

	private:

	struct _stack_entry
	{
		uint64_t IsAllocated : 1;
		uint64_t Flags : 15;
		uint64_t Size : 48;
		uintptr_t Addr; //This points to the BASE OF THE STACK!

		_stack_entry()
		{
			IsAllocated = false;
		}
	};

	_stack_entry* _stackMap;
	size_t _numStacks;
	size_t _stackMapCount;

};