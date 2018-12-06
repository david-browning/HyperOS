#include "kernel_interrupt.h"
#include "mem.h"
#include "kernel_cpu.h"
#include "kernel_pic.h"
#include "kernel_debug.h"
#include "drvr_serial.h"
#include "kernel_print.h"

static IDTManager* __global_idt_manager__ = nullptr;

#if defined(__cplusplus)
extern "C"
#endif
extern void FillInterruptArray();

#if defined(__cplusplus)
extern "C"
#endif
extern uint64_t INTERRUPT_ARRAY[256];

result_t DefaultInterruptHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   //WaitForDebugger();
   return RES_SUCCESS;
}

void INTPageFault()
{
   __asm
   {
      int 14;
   }
}

void INTGeneralProtectionFault()
{
   __asm
   {
      int 13;
   }
}

void INTDoubleFault()
{
   __asm
   {
      int 8;
   }
}

result_t IRQHandle(irqnumber_t irqNum, uint32_t error)
{
   if (irqNum < 0 || irqNum > IRQ_COUNT - 1 || __global_idt_manager__ == nullptr)
   {
      //The interrupt is out of range.
      return RES_INVALID_ARG;
   }

   auto handler = __global_idt_manager__->GetIRQHandler(irqNum);
   if (handler == nullptr)
   {
      WaitForDebugger();
      return RES_NOT_FOUND;
   }

   if (irqNum == INT_SYSTEM_CALL)
   {
      //System call
      WaitForDebugger();
   }

   result_t res = handler->Handler(irqNum, error, handler->HandlerData);

   if (irqNum >= __global_idt_manager__->PIC.GetOffset1() &&
       irqNum <= (__global_idt_manager__->PIC.GetOffset1() + 8))
   {
      __global_idt_manager__->PIC.SendEOI(uint8_t(irqNum - irqnumber_t(0x20)));
   }

   if (ResultFailed(res))
   {
      PrintResult(res);
   }

   return res;
}

IDTManager::IDTManager()
{
   _isLoaded = false;
}

KERNEL_FUNC result_t IDTManager::Initialize()
{
   _curIDT = nullptr;
   _nextHandle = 0;
   _isLoaded = true;
   return RES_SUCCESS;
}

result_t IDTManager::AllocateIDT(_OUT_HANDLE_ handle_t* handle_p,
                                 _IN_OPT_ InterruptHandlerFunc defaultHandler)
{
   FillInterruptArray();

   //Check to make sure we can add another idt.
   if (_nextHandle >= MAX_IDT)
   {
      return RES_ARRAY_FULL;
   }

   if (defaultHandler == nullptr)
   {
      defaultHandler = DefaultInterruptHandler;
   }

   //Install a handler for all IRQs.
   for (auto i = 0; i < IRQ_COUNT; i++)
   {
      InstallHandler(i, defaultHandler, nullptr, _nextHandle);
   }

   //Pass the handle back to the caller.
   *handle_p = _nextHandle;
   //Increment the internal handle for the next GDT.
   _nextHandle++;
   return RES_SUCCESS;
}

result_t IDTManager::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   DisableInterrupts();

   //Check that the handle is valid
   if (state_p->ThisHandle >= MAX_IDT)
   {
      return RES_INVALID_ARG;
   }

   _curIDT = &_idts[state_p->ThisHandle];

   //Create a struct that gets passed to the CPU.
   struct _loadableIDT
   {
      uint16_t NumEntries;
      uint64_t TableAddr;
   }
   __attribute__((packed));

   _loadableIDT idt =
   {
      IRQ_COUNT * sizeof(uint64_t),
      (uint64_t)&_curIDT->Table[0]
   };

   //Load the swapped table
   __asm
   {
      lidt idt;
   }

   __global_idt_manager__ = this;
   return RES_SUCCESS;
}

result_t IDTManager::SwapOut(_IN_POINTER_ kernel_swap_state_t * state_p)
{
   return result_t();
}

void IDTManager::InstallHandler(_IN_INT_ irqnumber_t irqNum,
                                _IN_FUNCTION_ InterruptHandlerFunc handlerFunc,
                                _IN_POINTER_ void* data,
                                _IN_HANDLE_ handle_t handle,
                                uint8_t ist)
{
   _idts[handle].Functions[irqNum] = interrupthandler_t
   {
      .Handler = handlerFunc,
      .HandlerData = data,
   };

   idtentry_t* newEntry = &_idts[handle].Table[irqNum];
   IRQMakeEntry(INTERRUPT_ARRAY[irqNum], newEntry, ist);
}

const interrupthandler_t* IDTManager::GetIRQHandler(_IN_INT_ irqnumber_t irqNum)
{
   return &_curIDT->Functions[irqNum];
}
