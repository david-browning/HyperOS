#pragma once
#include "annotations.h"
#include "kernel_interrupt_codes.h"
#include "kernel_context.h"
#include "ischeduler.h"
#include "vmem_allocator.h"
#include "ikernel_component.h"
#include "mem.h"
#include "linked_list.h"

typedef void(process_t)(void*);

typedef uint64_t proc_id_t;

static inline void ProcessYield(void)
{
	__asm int 0x70;
}

static inline void ProcessExit(void)
{
	__asm int 0x71;
}

extern context_t* ProcessCreate(_IN_FUNCTION_ process_t entry_point,
										  _IN_ void* arg);

extern "C"
extern context_t* GetCurProc();

class ProcessManager : public IKernelComponent
{
	public:
	ProcessManager();

	virtual ~ProcessManager();
	/*
	 The IVMemAllocator and IScheduler passed is passed by reference so do not let it go out of scope.
	 procStackAllocator_p is The stack allocator where the process stacks will be put.
	 */
	result_t Initialize(_IN_POINTER_ IScheduler<context_t>* scheduler_p,
							  _IN_POINTER_ IVMemAllocator* procStackAllocator_p,
							  _IN_POINTER_ IDTManager* idt_p,
							  _IN_HANDLE_ handle_t idtHandle);

	/*
	 Creates the resources necessary to run a new process and stores the info in the class's private fields.
	 Processes get their ID back.
	 */
	result_t CreateProcess(_IN_FUNCTION_ process_t entry_point,
								  _IN_ void* arg,
								  _OUT_ proc_id_t* procID_p,
								  _OUT_OPTIONAL_ context_t* context_p = nullptr);

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

	void ScheduleNext();

	void StartProcess(proc_id_t procID);

	result_t UnloadProcess(_IN_POINTER_ context_t* context_p);

	private:
	context_t* _curProc;
	context_t* _nextProc;
	context_t* _schedulerProc;

	size_t _numProcs;
	IScheduler<context_t>* _scheduler_p;

	//This allocates stacks for the processes this creates.
	IVMemAllocator* _procStackAllocator_p;
};