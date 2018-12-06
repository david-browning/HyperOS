#include "kernel.h"
#include "kernel_cpu.h"
#include "kernel_debug.h"
#include "key_codes.h"
#include "kernel_video.h"
#include "kernel_print.h"
#include "proc_terminal.h"

result_t GPHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   char* fmt = "General Protection Fault error %x.\n The system cannot continue.\n";
   char str[72];
   FormatString(fmt, str, error);
   GetVGADriverPtr()->WriteString(CONSOLE_ERROR_ATTRIBUTE, str);
   WaitForDebugger(GetVGADriverPtr());
   return RES_UNKNOWN_THREAD_ERROR;
}

result_t PageFaultHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{

   /*
   Page fault could be accessing without a translation, or access rights do not permit it.
   See Figure 4-12 for info on the error code.

   Bit 0: if 0, then there is not translation because the P flag was 0 in one of the paging structure entries

   W/R (bit 1).
   If the access causing the page - fault exception was a write, this flag is 1; otherwise, it is 0. This flag describes
   the access causing the page - fault exception, not the access rights specified by paging.

   U/S (bit 2).
   If a user - mode access caused the page - fault exception, this flag is 1; it is 0 if a supervisor - mode access did so.
   This flag describes the access causing the page - fault exception, not the access rights specified by paging.Usermode
   and supervisor - mode accesses are defined in Section 4.6.


   RSVD flag(bit 3).
   This flag is 1 if there is no translation for the linear address because a reserved bit was set in one of the paging
   structure entries used to translate that address. (Because reserved bits are not checked in a paging - structure
   entry whose P flag is 0, bit 3 of the error code can be set only if bit 0 is also set.1)

   I / D flag(bit 4).
   This flag is 1 if (1) the access causing the page - fault exception was an instruction fetch; and (2) either
   (a) CR4.SMEP = 1; or (b)both(i) CR4.PAE = 1 (either PAE paging or IA - 32e paging is in use); and
   (ii)IA32_EFER.NXE = 1. Otherwise, the flag is 0. This flag describes the access causing the page - fault
   exception, not the access rights specified by paging.

   PK flag(bit 5).
   This flag is 1 if (1) IA32_EFER.LMA = CR4.PKE = 1; (2) the access causing the page - fault exception was a data
   access; (3) the linear address was a user - mode address with protection key i; and (5) the PKRU register
   (see Section 4.6.2) is such that either(a) ADi = 1; or (b)the following all hold : (i)WDi = 1; (ii)the access is a write
   access; and (iii)either CR0.WP = 1 or the access causing the page - fault exception was a user - mode access.

   SGX flag (bit 15).
   This flag is 1 if the exception is unrelated to paging and resulted from violation of SGX-specific access-control
   requirements. Because such a violation can occur only if there is no ordinary page fault, this flag is set only if
   the P flag (bit 0) is 1 and the RSVD flag (bit 3) and the PK flag (bit 5) are both 0.

   Page-fault exceptions occur only due to an attempt to use a linear address. Failures to load the PDPTE registers
   with PAE paging (see Section 4.4.1) cause general-protection exceptions (#GP(0)) and not page-fault exceptions.

   */

   //Data is a pointer to the page allocator.
   PageAllocator* pageAlloc = (PageAllocator*)data_p;

   //Get the faulting address.
   uintptr_t faultingAdd = CPUGetCR2();

   //Get the L1 Entry and make sure it is marked as allocated. If not, error.
   uint64_t* l1 = GetL1EntryPtr(CPUGetCR3(), faultingAdd);
   if (*l1 & (1 << 9) != (1 << 9))
   {
      WaitForDebugger();
      return RES_PAGE_NOT_ALLOCATED;
   }

   //Allocate a physical page for the L1 to point to. 
   uint64_t* newPage = pageAlloc->Allocate<uint64_t>(PAGE_SIZE_BYTES);
   *l1 = *l1 | (uint64_t)newPage;
   if (*l1 == uint64_t(nullptr))
   {
      WaitForDebugger();
      return RES_OUT_OF_MEMORY;
   }

   //Mark the L1 entry as present and read-write.
   *l1 = *l1 | 0b11;

   return RES_SUCCESS;
}

result_t StackSegmentFaultHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t AlignmentCheckHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t DivideError(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   char* str = "Divide Exception.\n";
   GetVGADriverPtr()->WriteString(CONSOLE_ERROR_ATTRIBUTE, str);
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t DoubleFaultHanlder(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   char* str = "Invalid TSS Exception.\n";
   GetVGADriverPtr()->WriteString(CONSOLE_ERROR_ATTRIBUTE, str);
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t InvalidTSSHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t SegmentNotPresentFault(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t BoundRangeHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t OverflowHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t MachineCheckHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   WaitForDebugger(GetVGADriverPtr());
   return RES_SUCCESS;
}

result_t InvalidOpCodeHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   char* str = "Invalid Opcode: Address is %x, stack is %x.\n";
   char formated[72];
   FormatString(str, formated, CPUGetCR2(), CPUGetRSP());
   GetVGADriverPtr()->WriteString(CONSOLE_ERROR_ATTRIBUTE, formated);
   WaitForDebugger();
   return RES_SUCCESS;
}

Kernel::Kernel()
{
   //Set internal stuff to zero and null.
   _tssStack1 = nullptr;
   _tssStack2 = nullptr;
   _tssStack3 = nullptr;
   _tssStack4 = nullptr;
   _tssStack5 = nullptr;
   _tssStack6 = nullptr;
   _tssStack7 = nullptr;
   _isLoaded = false;
}

result_t Kernel::Initialize(_IN_POINTER_ memory_region_t* const kernelRegion,
                            _IN_POINTER_ GDTManager* const gdt_p,
                            _IN_POINTER_ TSSManager* const tss_p)
{
   //Create and initialize a physical page allocator.
   _pageAllocator = PageAllocator();
   result_t res = _pageAllocator.Initialize(kernelRegion, PAGE_SIZE_BYTES);
   if (ResultFailed(res))
   {
      goto cleanup1;
   }

   //Create an MMU and setup the identity map.
   _identityAllocator = IdentityAllocator();
   if (ResultFailed(res = _identityAllocator.Initialize(kernelRegion, 8, &_pageAllocator)))
   {
      goto cleanup2;
   }

   //Set CR3
   CPUSetCR3(_identityAllocator.GetL4Base());

   //Create a TSS.
   _tss_p = tss_p;
   if (!_tss_p->IsLoaded())
   {
      if (ResultFailed(res = _tss_p->Initialize()))
      {
         goto cleanup3;
      }
   }

   //Allocate a TSS in the TSS manager.
   if (ResultFailed(_tss_p->AllocateTSS(GLOBAL_DESCRIPTOR_TABLE_TSS, &_tssState.ThisHandle)))
   {
      goto cleanup4;
   }

   //Allocate stacks
   _tssStack1 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);
   _tssStack2 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);
   _tssStack3 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);
   _tssStack4 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);
   _tssStack5 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);
   _tssStack6 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);
   _tssStack7 = _pageAllocator.Allocate<uint8_t>(TSS_STACK_SIZE_BYTES);

   //Add the stacks to the TSS manager.
   res = _tss_p->SetStack(1, uintptr_t(_tssStack1), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);
   res = _tss_p->SetStack(2, uintptr_t(_tssStack2), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);
   res = _tss_p->SetStack(3, uintptr_t(_tssStack3), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);
   res = _tss_p->SetStack(4, uintptr_t(_tssStack4), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);
   res = _tss_p->SetStack(5, uintptr_t(_tssStack5), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);
   res = _tss_p->SetStack(6, uintptr_t(_tssStack6), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);
   res = _tss_p->SetStack(7, uintptr_t(_tssStack7), TSS_STACK_SIZE_BYTES, _tssState.ThisHandle);

   //Create and initialize the GDT.
   _gdtManager_p = gdt_p;
   if (!_gdtManager_p->IsLoaded())
   {
      if (ResultFailed(_gdtManager_p->Initialize()))
      {
         goto cleanup5;
      }
   }

   //Allocate a GDT in the GDT manager.
   if (ResultFailed(res = _gdtManager_p->AllocateGDT(&_gdtState.ThisHandle)))
   {
      goto cleanup6;
   }

   //Add a kernel code segment to the GDT.
   uint8_t gdtKernelCodeEntry_i;
   gdtdescriptor64_t gdtKernelCodeEntry;
   GDTMakeEntry(0, 0, PRIVILAGE_LEVELS::KERNEL, true, false, &gdtKernelCodeEntry);
   if (ResultFailed(res = _gdtManager_p->AddEntry(&gdtKernelCodeEntry, _gdtState.ThisHandle, &gdtKernelCodeEntry_i)))
   {
      goto cleanup7;
   }

   //Add a TSS segment to the GDT.
   uint8_t gdtTSSEntry_i;
   gdtdescriptor128_t gdtTSSEntry;
   GDTMakeEntry(_tss_p->GetTSSPointer(_tssState.ThisHandle), _tss_p->GetTSSSize(),
                PRIVILAGE_LEVELS::KERNEL, true, true, &gdtTSSEntry);
   if (ResultFailed(res = _gdtManager_p->AddEntry(&gdtTSSEntry, _gdtState.ThisHandle, &gdtTSSEntry_i)))
   {
      goto cleanup8;
   }

   //Initialize the IDT
   if (!_idtManager.IsLoaded())
   {
      if (ResultFailed(res = _idtManager.Initialize()))
      {
         goto cleanup9;
      }
   }

   //Disable interrupts during driver setup and what not.
   _idtManager.DisableInterrupts();

   //Remap the idtPIC
   _idtManager.PIC.Remap(0x20, 0x28);

   //Install a new IDT to the IDT manager.
   if (ResultFailed(res = _idtManager.AllocateIDT(&_idtState.ThisHandle)))
   {
      goto cleanup10;
   }

   //Set the stacks to certain interrupts
   _idtManager.InstallHandler(INT_GENERAL_PROTECTION, GPHandler, nullptr, _idtState.ThisHandle, 1);
   _idtManager.InstallHandler(INT_PAGE_FAULT, PageFaultHandler, &_pageAllocator, _idtState.ThisHandle, 2);
   _idtManager.InstallHandler(INT_SEGMENT_NOT_PRESENT, StackSegmentFaultHandler, nullptr, _idtState.ThisHandle, 3);
   _idtManager.InstallHandler(INT_STACK_SEGMENT_FAULT, StackSegmentFaultHandler, nullptr, _idtState.ThisHandle, 4);
   _idtManager.InstallHandler(INT_INVALID_TSS, InvalidTSSHandler, nullptr, _idtState.ThisHandle, 5);
   _idtManager.InstallHandler(INT_DOUBLE_FAULT, DoubleFaultHanlder, nullptr, _idtState.ThisHandle, 6);
   //Number 7 is set by the process manager.

   //Install additional interrupts.
   _idtManager.InstallHandler(INT_DIV_ERROR, DivideError, nullptr, _idtState.ThisHandle);
   _idtManager.InstallHandler(INT_BOUND_RANGE_EXCEPTION, BoundRangeHandler, nullptr, _idtState.ThisHandle);
   _idtManager.InstallHandler(INT_MACHINE_CHECK, MachineCheckHandler, nullptr, _idtState.ThisHandle);
   _idtManager.InstallHandler(INT_OVERFLOW, OverflowHandler, nullptr, _idtState.ThisHandle);
   _idtManager.InstallHandler(INT_ALIGNMENT_CHECK, AlignmentCheckHandler, nullptr, _idtState.ThisHandle);
   _idtManager.InstallHandler(INT_INVALID_OPCODE, InvalidOpCodeHandler, nullptr, _idtState.ThisHandle);


   //There is additional set up still required that cannot be completed until the kernel is swapped in.

   return RES_SUCCESS;

cleanup10:

cleanup9:

cleanup8:

cleanup7:

cleanup6:

cleanup5:

cleanup4:

cleanup3:

cleanup2:

cleanup1:

   return res;
}

result_t Kernel::SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   result_t res = RES_SUCCESS;
   _idtManager.DisableInterrupts();

   if (ResultFailed(res = _idtManager.SwapOut(&_idtState)) |
       ResultFailed(res = _gdtManager_p->SwapOut(&_gdtState)) |
       ResultFailed(res = _tss_p->SwapOut(&_tssState)) |
       ResultFailed(res = _scheduler->SwapOut(&_schedState)) |
       ResultFailed(res = _drvManager_p->SwapOut(&_drvState)) |
       ResultFailed(res = _procManager_p->SwapOut(&_procState)))
   {
      //If any failed, short circuit checking the others.
   }

   _vga_p->Clear(VGA_MODES::VGA, VGA_COLORS::Black);
   return res;
}

result_t Kernel::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   result_t res;
   VIRTUAL_KEYS k = KEY_NONE;
   char errorString[64];

   //Swap in the GDT
   if (ResultFailed(res = _gdtManager_p->SwapIn(&_gdtState)))
   {
      goto cleanup1;
   }

   //Swap in the TSS
   if (ResultFailed(res = _tss_p->SwapIn(&_tssState)))
   {
      goto cleanup2;
   }

   //Swap in the IDT
   if (ResultFailed(res = _idtManager.SwapIn(&_idtState)))
   {
      goto cleanup3;
   }

   //Enable interrupts
   _idtManager.EnableInterrupts();


   //There is additional setup required that could not be run when the kernel was not swapped in.
   if (!_isLoaded)
   {
      //Initialize the kernel heap allocator.
      memory_region_t kHeapRegion;
      kHeapRegion.BaseAddress = uint64_t(66) << 39;
      kHeapRegion.HighAddress = uint64_t(129) << 39;
      kHeapRegion.Type = MEMORY_TYPES::RAM;
      _kHeapAllocator = HeapAllocator();
      if (ResultFailed(res = _kHeapAllocator.Initialize(&kHeapRegion, HEAP_ALIGNMENT, &_pageAllocator)))
      {
         goto cleanup1;
      }

      SetHeapAllocator(&_kHeapAllocator);

      //Initialize the kernel stack allocator.
      memory_region_t kStackRegion;
      kStackRegion.BaseAddress = uint64_t(1) << 39;
      kStackRegion.HighAddress = uint64_t(65) << 39;
      kStackRegion.Type = MEMORY_TYPES::RAM;
      _kStackAllocator_p = new StackAllocator();
      if (ResultFailed(res = _kStackAllocator_p->Initialize(&kStackRegion, STACK_ALIGNMENT, &_pageAllocator)))
      {
         goto cleanup2;
      }

      //Load the scheduler.	
      _scheduler = new RRScheduler<context_t>();
      if (!_scheduler)
      {
         goto cleanup3;
      }

      if (ResultFailed(res = _scheduler->Initialize()))
      {
         goto cleanup4;
      }

      //Load the process manager
      _procManager_p = new ProcessManager();
      if (ResultFailed(res = _procManager_p->Initialize(_scheduler, _kStackAllocator_p,
                                                        &_idtManager, _idtState.ThisHandle)))
      {
         goto cleanup5;
      }

      //Load drivers.
      _drvManager_p = new DriverManager();
      if (ResultFailed(res = _drvManager_p->Initialize(_kStackAllocator_p)))
      {
         goto cleanup6;
      }

      _vga_p = new VGADriver();
      _vga_p->Load(0);
      _vga_p->Clear(VGA_MODES::VGA, CONSOLE_COLOR);
      _vga_p->WriteString(CONSOLE_DEFAULT_ATTRIBUTE, "Initializing Devices...\n");
      _vga_p->SwapIn(nullptr);

      res = _kb.Load(0);
      if (ResultFailed(res))
      {
         FormatString("Keyboard Error: %qx\n", errorString, res);
         _vga_p->WriteString(CONSOLE_ERROR_ATTRIBUTE, errorString);
         return res;
      }
      _kb.SwapIn(nullptr);

      _idtManager.InstallHandler(0x21, KBHandler, &_kb, _idtState.ThisHandle);
      _idtManager.PIC.ClearMask(1);
      _vga_p->WriteString(CONSOLE_SUCCESS_ATTRIBUTE, "Keyboard Initialized\n");

      //Bit 1 of the swap state's flags indicate doing an exhaustive memory check.
      if (state_p->Flags & 0b10)
      {
         _vga_p->WriteString(CONSOLE_DEFAULT_ATTRIBUTE, "Starting Memory Tests...\n");

         //Test the heap allocator.
         uint8_t* firstHeapAddr = _kHeapAllocator.Allocate<uint8_t>(10);
         uint8_t* secondHeapAddr = _kHeapAllocator.Allocate<uint8_t>(10);

         //Free the first and allocate something new to make sure it fits there.
         _kHeapAllocator.Deallocate(firstHeapAddr);
         uint8_t* newFirstHeap = _kHeapAllocator.Allocate<uint8_t>(10);
         if (newFirstHeap != firstHeapAddr)
         {
            return RES_NOT_AVAILABLE;
         }

         //Try something big
         const size_t bigHeapSize = 4 * 1024 * 1024;
         uint8_t* bigHeap = _kHeapAllocator.Allocate<uint8_t>(bigHeapSize);
         //Try writing something to it.
         for (auto i = 0; i < bigHeapSize; i++)
         {
            bigHeap[i] = 0xFF;
         }

         //Free the big heap. Something smaller should go in its place.
         _kHeapAllocator.Deallocate(bigHeap);
         uint8_t* bigHeapReplacement = _kHeapAllocator.Allocate<uint8_t>(256);
         if (bigHeap != bigHeapReplacement)
         {
            return RES_NOT_AVAILABLE;
         }

         //Free the first two blocks and make sure something new can go there.
         _kHeapAllocator.Deallocate(firstHeapAddr);
         _kHeapAllocator.Deallocate(secondHeapAddr);

         uint8_t* biggerFirstBlock = _kHeapAllocator.Allocate<uint8_t>(20);
         if (biggerFirstBlock != firstHeapAddr)
         {
            return RES_NOT_AVAILABLE;
         }

         //Clean up the rest of the allocated memory.
         _kHeapAllocator.Deallocate(biggerFirstBlock);
         _kHeapAllocator.Deallocate(bigHeapReplacement);

         //Now test a shit-load.
         _vga_p->WriteString(CONSOLE_DEFAULT_ATTRIBUTE, "Doing a shit-load of heap allocations...\n");
         static const size_t numAllocs = 512;
         uint64_t* heaps[numAllocs];
         for (auto i = 0; i < numAllocs; i++)
         {
            heaps[i] = new uint64_t[8];
         }

         for (auto i = 0; i < numAllocs; i++)
         {
            delete[] heaps[i];
         }

         _vga_p->WriteString(CONSOLE_SUCCESS_ATTRIBUTE, "Completed Memory Tests.\n");
      }

      _isLoaded = true;
   }

   //Swap in the Process Manager
   if (ResultFailed(res = _procManager_p->SwapIn(&_procState)))
   {
      goto cleanup7;
   }

   //Swap in the scheduler
   if (ResultFailed(res = _scheduler->SwapIn(&_schedState)))
   {
      goto cleanup8;
   }

   //Swap in the driver manager
   if (ResultFailed(res = _drvManager_p->SwapIn(&_drvState)))
   {
      goto cleanup9;
   }

   _vga_p->WriteString(CONSOLE_SUCCESS_ATTRIBUTE, "Kernel Running...\n");


   //Start the first process.
   proc_id_t terminalID;
   _procManager_p->CreateProcess(Terminal, _procManager_p, &terminalID, nullptr);
   _procManager_p->StartProcess(terminalID);

   return RES_SUCCESS;

cleanup9:

cleanup8:

cleanup7:

cleanup6:

cleanup5:

cleanup4:

cleanup3:

cleanup2:

cleanup1:

   return res;
}
