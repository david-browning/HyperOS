#include "page_allocator.h"
#include "mem.h"
#include "kernel_debug.h"

PageAllocator::PageAllocator()
{
	_freePages = nullptr;
	_numPages = 0;
	_isProtected = false;
}

result_t PageAllocator::Initialize(_IN_POINTER_ memory_region_t * memoryRegion_p, _IN_ const size_t alignment)
{
	result_t res = IMemoryAllocator::Initialize(memoryRegion_p, PAGE_SIZE_BYTES);
	SetProtectedMode();
	if (ResultFailed(res))
	{
		return res;
	}

	//Figure out how many pages this region gets.
	const size_t numPages = (_region.HighAddress - _region.BaseAddress) / PAGE_SIZE_BYTES;
	_numPages = numPages;

	//Number of bytes needed to hold a list of entries that indicate if the page is free.
	size_t numBytes = numPages * sizeof(_free_page_entry);

	//Allocate memory to hold the free list.
	const size_t pagesNeeded = RoundUp(numBytes, PAGE_SIZE_BYTES) / PAGE_SIZE_BYTES;
	_lastAddr = _lastAddr + PAGE_SIZE_BYTES * pagesNeeded;
	_freePages = (_free_page_entry*)_firstAddr;
	
	//Mark all the pages as free except the ones needed to hold the free list.
	for (auto i = 0; i < numPages; i++)
	{
		if (i < pagesNeeded)
		{
			_freePages[i].Flags = 0;
			_freePages[i].IsFree = 0;
			_freePages[i].Protected = 1;
		}
		else
		{
			_freePages[i].Flags = 0;
			_freePages[i].IsFree = 1; 
			_freePages[i].Protected = 0;
		}
	}
	
	ClearProtectedMode();
	return RES_SUCCESS;
}

void* PageAllocator::_allocate(_IN_SIZE_ size_t bytes)
{
	uintptr_t ret = _lastAddr;
	size_t freeListIndex = (ret - _region.BaseAddress) / PAGE_SIZE_BYTES;

	//Mark the page as used.
	_freePages[freeListIndex].Protected = _isProtected;
	_freePages[freeListIndex].IsFree = 0;
	_freePages[freeListIndex].Flags = 0;

	//Loop _last chunk around if it goes out of bounds.
	if (_lastAddr + PAGE_SIZE_BYTES > _region.HighAddress)
	{
		_lastAddr = _region.BaseAddress;
	}
	else
	{
		_lastAddr += PAGE_SIZE_BYTES;
	}

	return (void*)ret;
}

void PageAllocator::_deallocate(_IN_POINTER_ void* ptr)
{
	WaitForDebugger();
	size_t freeListIndex = (uintptr_t(ptr) - _region.BaseAddress) / PAGE_SIZE_BYTES;

	//Raise a general protection fault if the page is not allowed to be freed.
	if (_freePages[freeListIndex].Protected)
	{
		INTGeneralProtectionFault();
	}

	_freePages[freeListIndex].IsFree = false;
}
