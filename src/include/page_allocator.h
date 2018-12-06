#pragma once

#include "annotations.h"
#include "imemallocator.h"

static constexpr size_t PAGE_SIZE_BYTES = 4096;
static constexpr size_t ENTRIES_PER_PAGE = PAGE_SIZE_BYTES / sizeof(uint64_t);

class PageAllocator : public IMemoryAllocator
{
	public:
	PageAllocator();

	virtual result_t Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p, _IN_ const size_t alignment);

	//Any pages allocated when protected mode is set cannot be freed.
	inline void SetProtectedMode()
	{
		_isProtected = true;
	}

	//Any pages allocated when protected mode is set cannot be freed.
	inline void ClearProtectedMode()
	{
		_isProtected = false;
	}

	protected:

	virtual void* _allocate(_IN_SIZE_ size_t bytes);

	virtual void _deallocate(_IN_POINTER_ void* ptr);

	struct _free_page_entry
	{
		uint16_t Flags : 14;
		uint16_t Protected : 1;
		uint16_t IsFree : 1;
	};

	//If the ith entry is 0, then the ith page is free.
	_free_page_entry* _freePages;
	size_t _numPages;

	//Set to true means that the pages allocated cannot be freed. Trying to free
	//them causes a GPF.
	bool _isProtected;
};
