#include "vmem_heap_allocator.h"
#include "kernel_debug.h"

HeapAllocator::HeapAllocator()
{

}

result_t HeapAllocator::Initialize(_IN_POINTER_ memory_region_t* memoryRegion_p,
                                   _IN_ const size_t alignment,
                                   _IN_POINTER_ PageAllocator* pageAllocator_p)
{
   result_t res = IVMemAllocator::Initialize(memoryRegion_p, alignment, pageAllocator_p);


   _firstChunk_p = (_mem_chunk*)IVMemAllocator::_allocate(16);
   _firstChunk_p->IsAllocated = true;
   _firstChunk_p->Size = 16;
   _lastAddr += 16;

   return res;
}

void* HeapAllocator::_allocate(_IN_SIZE_ size_t bytes)
{
   size_t alignedBytes = MemAlignAddress(bytes + 2 * sizeof(_mem_chunk), _alignment);
   _mem_chunk* curChunk = _firstChunk_p;
   _mem_chunk* retChunk = nullptr;

   while (!_memoryFits(curChunk, alignedBytes))
   {
      if (curChunk->Size > 0x100000000)
      {
         WaitForDebugger();
      }
      curChunk = (_mem_chunk*)(uintptr_t(curChunk) + curChunk->Size);
   }

   if (uintptr_t(curChunk) + alignedBytes >= _lastAddr)//MemAlignAddress(_lastAddr, PAGE_SIZE_BYTES))
   {
      IVMemAllocator::_allocate(alignedBytes);
      retChunk = (_mem_chunk*)(uintptr_t(curChunk) + curChunk->Size);
      //retChunk = (_mem_chunk*)_lastAddr;
   }
   else
   {
      if (curChunk->Size > alignedBytes)
      {
         alignedBytes = curChunk->Size;
      }

      retChunk = curChunk;
   }

   retChunk->IsAllocated = true;
   retChunk->Flags = 3;
   retChunk->Size = alignedBytes;
   _mem_chunk* footerChunk = (_mem_chunk*)(uintptr_t(retChunk) + alignedBytes - sizeof(_mem_chunk));

   MemoryCopy<_mem_chunk>(footerChunk, retChunk, 1);
   void* ret = (void*)(uintptr_t(retChunk) + sizeof(_mem_chunk));
   return ret;
}

void HeapAllocator::_deallocate(_IN_POINTER_ void* ptr)
{
   //Mark this chunk as free.
   _mem_chunk* firstChunk = (_mem_chunk*)(uintptr_t(ptr) - sizeof(_mem_chunk));
   firstChunk->IsAllocated = false;
   _mem_chunk* footerChunk = (_mem_chunk*)(uintptr_t(firstChunk) + firstChunk->Size - sizeof(_mem_chunk));
   MemoryCopy<_mem_chunk>(footerChunk, firstChunk, 1);

   //If the previous chunk is free, then combine this chunk with the one before it.
   _mem_chunk* prevFooter = (_mem_chunk*)(uintptr_t(firstChunk) - sizeof(_mem_chunk));
   if (!prevFooter->IsAllocated)
   {
      size_t prevChunkSize = prevFooter->Size;

      //The previous chunk now has more size.
      prevFooter->Size += firstChunk->Size;

      //Update the previous chunk's header chunk to match the changes to the footer chunk.
      _mem_chunk* prevHeader = (_mem_chunk*)(uintptr_t(prevFooter) - prevChunkSize + sizeof(_mem_chunk));
      MemoryCopy(prevHeader, prevFooter, 1);
      MemoryCopy(footerChunk, prevHeader, 1);
   }

   return;
}
