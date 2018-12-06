#pragma once
#include "annotations.h"

struct kernel_swap_state_t
{
   _IN_OPT_ handle_t ThisHandle;
   uint64_t Flags;
};

class IKernelComponent
{
   public:
   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

   inline bool IsLoaded()
   {
      return _isLoaded;
   }

   protected:
   bool _isLoaded;
};