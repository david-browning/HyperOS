#pragma once
#include "annotations.h"
#include "ikernel_component.h"

class TSSManager : public IKernelComponent
{
   public:
   TSSManager();

   result_t Initialize();

   result_t AllocateTSS(_IN_INT_ uint16_t gdtOffset,
                        _OUT_HANDLE_ handle_t* tssHandle_p);

   /*
    Caller is responsible for making sure the stack is not used for anything except this.
    */
   result_t SetStack(_IN_INT_ uint8_t stackIndex,
                     _IN_INT_ uintptr_t stackAddr,
                     _IN_SIZE_ size_t stackSize,
                     _IN_HANDLE_ handle_t tssHandle);

   uintptr_t GetTSSPointer(_IN_HANDLE_ handle_t tssHandle)
   {
      return uintptr_t(&_tss[tssHandle].TSS);
   }

   /*
    Returns the size in bytes, of the a TSS that is the TSS.
    */
   inline size_t GetTSSSize()
   {
      return sizeof(_task_segment_t);
   }

   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

   private:
   static constexpr uint8_t MAX_TSS = 4;

   struct _task_segment_t
   {
      uint32_t IGN1;
      uintptr_t RSP0;
      uintptr_t RSP1;
      uintptr_t RSP2;
      uint64_t IGN2;
      uintptr_t IST1;
      uintptr_t IST2;
      uintptr_t IST3;
      uintptr_t IST4;
      uintptr_t IST5;
      uintptr_t IST6;
      uintptr_t IST7;
      uint64_t IGN3;
      uint16_t IGN4;
      uint16_t IOMapBase;
   }
   __attribute__((packed));

   struct _tss_state
   {
      _task_segment_t TSS;
      uint16_t GDTOffset;
   };

   _tss_state _tss[MAX_TSS];
   _tss_state* _curTSS;
   handle_t _nextHandle;
};