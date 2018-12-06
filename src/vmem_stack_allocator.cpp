#include "vmem_stack_allocator.h"
#include "kernel_debug.h"

StackAllocator::StackAllocator()
{
   _stackMapCount = 0;
   _stackMap = nullptr;
   _numStacks = 0;
}

void* StackAllocator::_allocate(_IN_SIZE_ size_t bytes)
{
   size_t alignedBytes = MemAlignAddress(bytes, STACK_ALIGNMENT);

   //Iterate through the stack map until finding a free stack.
   size_t i = 0;
   bool foundEmpty = false;
   uintptr_t retStack = _region.BaseAddress;
   for (; i < _numStacks && !foundEmpty; i++)
   {
      if (!_stackMap[i].IsAllocated)
      {
         if (_stackMap[i].Size >= alignedBytes)
         {
            alignedBytes = _stackMap[i].Size;
         }

         foundEmpty = true;
      }
      else
      {
         retStack = retStack + _stackMap[i].Size;
      }
   }

   if (!foundEmpty)
   {
      //Check if need more memory for stacks
      if (i >= _stackMapCount)
      {
         const auto oldStackCount = _numStacks;
         _stackMapCount += 32;
         _stack_entry* oldMap = _stackMap;
         _stack_entry* newMap = nullptr;
         if ((newMap = new _stack_entry[_stackMapCount]) == nullptr)
         {
            return nullptr;
         }

         MemoryCopy<_stack_entry>(newMap, oldMap, oldStackCount);
         _stackMap = newMap;
         delete[] oldMap;
      }

      //Need new addresses.
      IVMemAllocator::_allocate(alignedBytes);
      _numStacks++;
   }

   else if (i > 0)
   {
      i--;
   }

   retStack += alignedBytes;
   _stackMap[i].Addr = retStack;
   _stackMap[i].Size = alignedBytes;
   _stackMap[i].IsAllocated = true;
   _stackMap[i].Flags = 0xF;

   return (void*)retStack;
}

void StackAllocator::_deallocate(_IN_POINTER_ void* ptr)
{
   for (auto i = 0; i < _stackMapCount; i++)
   {
      if (uintptr_t(ptr) == _stackMap[i].Addr && _stackMap[i].IsAllocated)
      {
         _stackMap[i].IsAllocated = false;
         return;
      }
   }
}
