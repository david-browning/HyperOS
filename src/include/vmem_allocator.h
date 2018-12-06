#pragma once

#include "annotations.h"
#include "mem.h"
#include "imemallocator.h"
#include "page_allocator.h"

class IVMemAllocator : public IMemoryAllocator
{
	public:
	IVMemAllocator();

	//The memory region that gets passed to this is the range of virtual address allowed.
	//It should be between 1 << 39 and 511 < 39.
	virtual result_t Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p,
										 _IN_ const size_t alignment,
										 _IN_POINTER_ PageAllocator* pageAllocator_p);

	protected:

	virtual void* _allocate(_IN_SIZE_ size_t bytes);

	virtual void _deallocate(_IN_POINTER_ void* ptr);

	PageAllocator* _physPageAllocator;
};


extern IVMemAllocator* GetHeapAllocator();

extern void SetHeapAllocator(IVMemAllocator* allocator_p);

template<typename T>
T* MemoryAllocate(const size_t sz)
{
	return GetHeapAllocator()->Allocate<T>(sz);
}

inline void MemoryFree(void* ptr)
{
	GetHeapAllocator()->Deallocate(ptr);
}

extern "C"
extern void* memset(void* ptr, int value, size_t num);

extern void* operator new(size_t sz);

extern void* operator new[](size_t count);

extern void operator delete(void* ptr);

extern void operator delete[](void* ptr);