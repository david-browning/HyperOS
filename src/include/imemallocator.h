#pragma once
#include "annotations.h"
#include "mem.h"

class IMemoryAllocator
{
   public:
   IMemoryAllocator()
   {

   }

   virtual result_t Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p, _IN_ const size_t alignment)
   {
      if (memoryRegion_p->Type != MEMORY_TYPES::RAM)
      {
         return RES_INVALID_ARG;
      }

      _alignment = alignment;
      _region = *memoryRegion_p;

      //Realign addresses
      _lastAddr = _firstAddr = MemAlignAddress(_region.BaseAddress, alignment);
      _region.BaseAddress = _lastAddr;

      return RES_SUCCESS;
   }

   inline void Deallocate(_IN_POINTER_ void* ptr)
   {
      _deallocate(ptr);
   }

   template<typename T>
   inline T* Allocate(_IN_SIZE_ size_t bytes)
   {
      return (T*)_allocate(bytes);
   }

   inline uintptr_t GetRegionBaseAddress()
   {
      return _region.BaseAddress;
   }

   inline uintptr_t GetRegionHighAddress()
   {
      return _region.HighAddress;
   }

   protected:

   struct _mem_chunk
   {
      size_t Size : 61;
      size_t IsAllocated : 1;
      size_t Flags : 2;
   };

   virtual void* _allocate(_IN_SIZE_ size_t bytes) = 0;

   virtual void _deallocate(_IN_POINTER_ void* ptr) = 0;

   static constexpr size_t MEM_CHUNK_HEADER_SIZE = 4 * sizeof(uint64_t);

   inline bool _memChunkEmpty(_mem_chunk* mem_p)
   {
      return mem_p->IsAllocated == 0;
   }

   memory_region_t _region;
   uintptr_t _lastAddr = 0;
   uintptr_t _firstAddr = 0;
   size_t _alignment = 0;
};