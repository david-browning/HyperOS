#pragma once

#include "imemallocator.h"
#include "page_allocator.h"


class IdentityAllocator : public IMemoryAllocator
{
   public:
   IdentityAllocator();

   virtual result_t Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p,
                               _IN_ const size_t alignment,
                               _IN_POINTER_ PageAllocator* pageAllocator_p);

   inline uintptr_t GetL4Base()
   {
      return _l4;
   }

   protected:

   //Not allowed to allocate memory on the identity level. Returns nullptr.
   virtual void* _allocate(_IN_SIZE_ size_t bytes);

   virtual void _deallocate(_IN_POINTER_ void* ptr);

   PageAllocator* _physPageAllocator;
   uintptr_t _l4;
};