#pragma once

#include "vmem_allocator.h"

class HeapAllocator : public IVMemAllocator
{
	public:
	HeapAllocator();

	virtual result_t Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p,
										 _IN_ const size_t alignment,
										 _IN_POINTER_ PageAllocator* pageAllocator_p);

	protected:

	virtual void* _allocate(_IN_SIZE_ size_t bytes);

	virtual void _deallocate(_IN_POINTER_ void* ptr);

	private:
	_mem_chunk* _firstChunk_p;

	inline bool _memoryFits(_IN_POINTER_ const _mem_chunk* chunk, _IN_SIZE_ const size_t memSize)
	{
		return ((!chunk->IsAllocated) && (chunk->Size == 0 || chunk->Size >= memSize));
	}
};