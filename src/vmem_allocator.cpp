#include "vmem_allocator.h"
#include "kernel_cpu.h"


static IVMemAllocator* __mem_allocator__ = nullptr;

IVMemAllocator * GetHeapAllocator()
{
   return __mem_allocator__;
}

void SetHeapAllocator(IVMemAllocator* allocator_p)
{
   __mem_allocator__ = allocator_p;
}

void* memset(void * ptr, int value, size_t num)
{
   MemorySet<uint8_t>((uint8_t*)ptr, uint8_t(value), num);
   return ptr;
}

void* operator new(size_t sz)
{
   if (!__mem_allocator__)
   {
      return nullptr;
   }

   return __mem_allocator__->Allocate<uint8_t>(sz);
}

void* operator new[](size_t count)
{
   if (!__mem_allocator__)
   {
      return nullptr;
   }

   return __mem_allocator__->Allocate<uint8_t>(count);
}

void operator delete(void* ptr)
{
   if (__mem_allocator__)
      __mem_allocator__->Deallocate(ptr);
}

void operator delete[](void * ptr)
{
   if (__mem_allocator__)
      __mem_allocator__->Deallocate(ptr);
}

IVMemAllocator::IVMemAllocator()
{
   _physPageAllocator = nullptr;
}

result_t IVMemAllocator::Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p,
                                    _IN_ const size_t alignment,
                                    _IN_POINTER_ PageAllocator* pageAllocator_p)
{
   result_t res = IMemoryAllocator::Initialize(memoryRegion_p, alignment);
   if (ResultFailed(res))
   {
      return res;
   }

   _physPageAllocator = pageAllocator_p;
   return RES_SUCCESS;
}

void * IVMemAllocator::_allocate(_IN_SIZE_ size_t bytes)
{
   //_region would be something like 1 << 39 to 16 << 39
   //It is the range of L4 entries.

   //Get the address to return and increment the internal nextAddress; 
   uintptr_t ret = _lastAddr;
   const size_t alignedBytes = MemAlignAddress(bytes, _alignment);
   _lastAddr += alignedBytes;

   //Make sure everything except L1 entry is present, etc...
   //Mark L1 entry as allocated using available bits.
   uintptr_t retIterator = ret;
   while (retIterator <= ret + alignedBytes)
   {
      //Figure out where the l4 entry is.
      auto l4Offset = GetL4Offset(retIterator) * sizeof(uint64_t);
      uint64_t* l4EntryPtr = (uint64_t*)((CPUGetCR3() & BASE_ADDRESS_MASK) + l4Offset);

      //Check if address is in range of the allowed memory. 
      if (uintptr_t(l4EntryPtr) >= _region.HighAddress)
      {
         return nullptr;
      }

      //Check if the L4 entry is present and read-writable.
      if ((*l4EntryPtr & 0b11) != 0b11)
      {
         //Allocate an L3 for this entry to point to.
         uintptr_t baseAddress = uintptr_t(_physPageAllocator->Allocate<uint64_t>(PAGE_SIZE_BYTES));

         //Set read and write for it.
         *l4EntryPtr = baseAddress | 0b11;
      }

      auto l3Offset = GetL3Offset(retIterator) * sizeof(uint64_t);
      uint64_t* l3EntryPtr = (uint64_t*)(uintptr_t(*l4EntryPtr & BASE_ADDRESS_MASK) + l3Offset);
      if ((*l3EntryPtr & 0b11) != 0b11)
      {
         //Allocate an L2 for this entry to point to.
         uintptr_t baseAddress = uintptr_t(_physPageAllocator->Allocate<uint64_t>(PAGE_SIZE_BYTES));

         //Set read and write for it.
         *l3EntryPtr = baseAddress | 0b11;
      }

      auto l2Offset = GetL2Offset(retIterator) * sizeof(uint64_t);
      uint64_t* l2EntryPtr = (uint64_t*)(uintptr_t(*l3EntryPtr & BASE_ADDRESS_MASK) + l2Offset);
      if ((*l2EntryPtr & 0b11) != 0b11)
      {
         //Allocate an L1 for this entry to point to.
         uintptr_t baseAddress = uintptr_t(_physPageAllocator->Allocate<uint64_t>(PAGE_SIZE_BYTES));

         //Set read and write for it.
         *l2EntryPtr = baseAddress | 0b11;
      }

      //The page fault handler takes care of allocating a page for L1 to point to.
      //It only works if the allocated bit is set.
      auto l1Entry = GetL1EntryPtr(CPUGetCR3(), retIterator);
      *l1Entry = *l1Entry | (1 << 9);
      retIterator += PAGE_SIZE_BYTES;
   }

   return (void*)ret;
}

void IVMemAllocator::_deallocate(_IN_POINTER_ void* ptr)
{
   //Mark the L1 entry as not present and not available.


   //Free the pages 
}
