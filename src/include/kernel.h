#pragma once
#include "annotations.h"
#include "mem.h"
#include "kernel_interrupt.h"
#include "kernel_gdt.h"
#include "page_allocator.h"
#include "kernel_driver_manager.h"
#include "kernel_tss.h"
#include "rr_scheduler.h"
#include "kernel_context.h"

#include "mem_identity_allocator.h"
#include "vmem_stack_allocator.h"
#include "vmem_heap_allocator.h"

#include "drvr_vga.h"
#include "drvr_ps2kybd.h"

#include "ikernel_component.h"
#include "kernel_procs.h"

static constexpr result_t RES_NEXT_KERNEL = MakeResult(false, FACILITY_KERNEL, 0xFFFF);
static constexpr result_t RES_PRE_KERNEL = MakeResult(false, FACILITY_KERNEL, 0xFFFE);
static constexpr result_t RES_SHUTDOWN = MakeResult(false, FACILITY_KERNEL, 0xFEFE);

class Kernel : public IKernelComponent
{
	public:
	Kernel();

	result_t Initialize(_IN_POINTER_ memory_region_t* const kernelRegion,
							  _IN_POINTER_ GDTManager* const gdt_p,
								_IN_POINTER_ TSSManager* const tss_p);


	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

	private:
	//These are global resources that all instances of the kernel must share.
	GDTManager* _gdtManager_p;
	kernel_swap_state_t _gdtState;

	TSSManager* _tss_p;
	kernel_swap_state_t _tssState;

	//Management classes and their states.
	IDTManager _idtManager;
	kernel_swap_state_t _idtState;

	DriverManager* _drvManager_p;
	kernel_swap_state_t _drvState;

	RRScheduler<context_t>* _scheduler;
	kernel_swap_state_t _schedState;

	ProcessManager* _procManager_p;
	kernel_swap_state_t _procState;

	//Drivers
	VGADriver* _vga_p;
	PS2KeyboardDriver _kb;


	//Pointers to stacks.
	uint8_t* _tssStack1;
	uint8_t* _tssStack2;
	uint8_t* _tssStack3;
	uint8_t* _tssStack4;
	uint8_t* _tssStack5;
	uint8_t* _tssStack6;
	uint8_t* _tssStack7;

	//Memory Allocators.
	PageAllocator _pageAllocator;
	IdentityAllocator _identityAllocator;
	HeapAllocator _kHeapAllocator;
	StackAllocator* _kStackAllocator_p;

	//Heaps must be aligned on 8 bytes.
	static constexpr size_t HEAP_ALIGNMENT = 8;

	//How big the stacks are for interrupts.
	static constexpr size_t TSS_STACK_SIZE_BYTES = PAGE_SIZE_BYTES;
};