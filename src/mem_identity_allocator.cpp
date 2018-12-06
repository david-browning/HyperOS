#include "mem_identity_allocator.h"
#include "kernel_debug.h"

//Entry in the page map level 4 table.
struct pml4_entry_t
{
	//Present.
	uint64_t P : 1;
	//1 To allow writing.
	uint64_t RW : 1;
	//0 is supervisor access only.
	uint64_t US : 1;
	//Page-level write-through; indirectly determines the memory type used to access the page-directory-pointer table
	//referenced by this entry
	uint64_t PWT : 1;
	//Page-level cache disable; indirectly determines the memory type used to access the page-directory-pointer table
	//referenced by this entry
	uint64_t PCD : 1;
	//Accessed.
	uint64_t A : 1;
	uint64_t IGN : 1;
	uint64_t MBZ : 2;
	uint64_t Available1 : 3;
	uint64_t BaseAddress : 40;
	uint64_t Available2 : 11;

	//Execute disable. 1 indicates instruction fetches are not allowed.
	uint64_t NX : 1;
};

//Entry in the page directory pointer table. (Level 3)
struct pdp_entry_t
{
	uint64_t P : 1;
	uint64_t RW : 1;
	uint64_t US : 1;
	//Page level write though. Determines the memory type used to access the PML4 during
	//linear address translation.
	uint64_t PWT : 1;
	//Page level cache disable.
	uint64_t PCD : 1;
	uint64_t A : 1;
	uint64_t IGN : 1;
	uint64_t Zero : 1;
	uint64_t MBZ : 1;
	uint64_t Available1 : 3;
	uint64_t BaseAddress : 40;
	uint64_t Available2 : 11;
	//Must be zero.
	uint64_t NX : 1;

};

//Entry in the page directory table. (Level 2)
struct pd_entry_t
{
	uint64_t P : 1;
	uint64_t RW : 1;
	uint64_t US : 1;
	uint64_t PWT : 1;
	uint64_t PCD : 1;
	uint64_t A : 1;
	uint64_t IGN1 : 1;
	uint64_t Zero : 1;
	uint64_t IGN2 : 1;
	uint64_t Available1 : 3;
	uint64_t BaseAddress : 40;
	uint64_t Available2 : 11;
	uint64_t NX : 1;
};

//Page table entry. (Level 1)
struct pt_entry_t
{
	uint64_t P : 1;
	uint64_t RW : 1;
	uint64_t US : 1;
	uint64_t PWT : 1;
	uint64_t PCD : 1;
	uint64_t A : 1;
	uint64_t D : 1;
	uint64_t PAT : 1;
	uint64_t G : 1;
	uint64_t Available1 : 3;
	uint64_t BaseAddress : 40;
	uint64_t Available2 : 11;
	uint64_t NX : 1;
};


IdentityAllocator::IdentityAllocator()
{
	_physPageAllocator = nullptr;
}

result_t IdentityAllocator::Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p,
													_IN_ const size_t alignment, 
													_IN_POINTER_ PageAllocator* pageAllocator_p)
{
	if (!pageAllocator_p)
	{
		return RES_INVALID_ARG;
	}

	result_t res = IMemoryAllocator::Initialize(memoryRegion_p, alignment);
	if (ResultFailed(res))
	{
		return res;
	}

	_physPageAllocator = pageAllocator_p;

	//Set the page allocator to protected so the identity map cannot be freed.
	_physPageAllocator->SetProtectedMode();

	//Set up the identity map.
	size_t numAllocated = 0;
	//Number of pages needed to be allocated.
	size_t numRequired = (_physPageAllocator->GetRegionHighAddress() - _physPageAllocator->GetRegionBaseAddress()) / PAGE_SIZE_BYTES;
	//Iterator for counting how many l4 entries were allocated.
	while (numAllocated < numRequired)
	{
		pml4_entry_t* l4 = _physPageAllocator->Allocate<pml4_entry_t>(4096);
		if (!l4)
		{
			return RES_OUT_OF_MEMORY;
		}

		auto l4i = 0;
		while (numAllocated < numRequired && l4i < ENTRIES_PER_PAGE)
		{
			pdp_entry_t* l3 = _physPageAllocator->Allocate<pdp_entry_t>(4096);
			if (!l3)
			{
				return RES_OUT_OF_MEMORY;
			}

			auto l3i = 0;
			while (numAllocated < numRequired && l3i < ENTRIES_PER_PAGE)
			{
				pd_entry_t* l2 = _physPageAllocator->Allocate<pd_entry_t>(4096);
				if (!l2)
				{
					return RES_OUT_OF_MEMORY;
				}

				auto l2i = 0;

				//Fill in the l2 
				while (numAllocated < numRequired && l2i < ENTRIES_PER_PAGE)
				{
					pt_entry_t* l1 = _physPageAllocator->Allocate<pt_entry_t>(4096);
					if (!l1)
					{
						return RES_OUT_OF_MEMORY;
					}

					auto i = 0;
					//Fill in the l1
					while (numAllocated < numRequired && i < ENTRIES_PER_PAGE)
					{
						l1[i] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
						l1[i].BaseAddress = (numAllocated * PAGE_SIZE_BYTES) >> 12;
						l1[i].P = 1;
						l1[i].RW = 1;
						//Mark the page as allocated.
						l1[i].Available1 = 1;
						numAllocated++;
						i++;
					}

					//Set the current l2 entry to point to the l1.
					l2[l2i] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
					l2[l2i].BaseAddress = uintptr_t(l1) >> 12;
					l2[l2i].P = 1;
					l2[l2i].RW = 1;
					l2i++;
				}

				//Set the current l3 entry to point to the l2
				l3[l3i] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
				l3[l3i].BaseAddress = uintptr_t(l2) >> 12;
				l3[l3i].P = 1;
				l3[l3i].RW = 1;
				l3i++;
			}

			//Set the current l4 entry to point to the l3
			l4[l4i] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
			l4[l4i].BaseAddress = uintptr_t(l3) >> 12;
			l4[l4i].P = 1;
			l4[l4i].RW = 1;
			l4i++;
		}

		_l4 = uint64_t(l4) >> 12;
	}

	_physPageAllocator->ClearProtectedMode();
	return RES_SUCCESS;
}

void* IdentityAllocator::_allocate(_IN_SIZE_ size_t bytes)
{
	//Not allowed to allocate memory on the identity level.
	return nullptr;
}

void IdentityAllocator::_deallocate(_IN_POINTER_ void * ptr)
{

}
