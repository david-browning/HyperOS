#pragma once
#include "annotations.h"
#include "imemallocator.h"
#include "ikernel_component.h"

template<typename T>
class IScheduler : public IKernelComponent
{
   public:

   virtual result_t Initialize() = 0;

   virtual T* GetNext() = 0;

   virtual void Insert(T* obj) = 0;

   virtual void Remove(T* obj) = 0;

   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

   virtual bool IsEmpty() = 0;

   protected:
};