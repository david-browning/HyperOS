#include "kernel_procs.h"
#include "kernel_debug.h"
#include "kernel_cpu.h"
#include "kernel_gdt.h"
#include "kernel_interrupt.h"
#include "kernel_interrupt_codes.h"

static context_t* __curProc__ = nullptr;
static context_t* __nextProc__ = nullptr;
static ProcessManager* __proc_manager__;
static constexpr proc_id_t FIRST_PROC_ID = 0;

void NoProc(void* arg)
{
	ProcessManager* procMan = (ProcessManager*)arg;
	while (true)
	{
		int x = 0;
		ProcessYield();
	}

	return;
}

result_t ProcYeildHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
	ProcessManager* procMan = (ProcessManager*)data_p;

	procMan->ScheduleNext();

	while (__curProc__->IsBlocked)
	{
		procMan->ScheduleNext();
	}

	return RES_SUCCESS;
}

result_t ProcExitHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
	ProcessManager* procMan = (ProcessManager*)data_p;
	return procMan->UnloadProcess(GetCurProc());
}

context_t* GetCurProc()
{
	return __curProc__;
}

ProcessManager::ProcessManager()
{
	_procStackAllocator_p = nullptr;
	_scheduler_p = nullptr;
	__curProc__ = nullptr;
	__nextProc__ = nullptr;
	_nextProc = nullptr;
	_curProc = nullptr;
}

ProcessManager::~ProcessManager()
{
	if (_schedulerProc)
	{
		_procStackAllocator_p->Deallocate((void*)_schedulerProc->RSP);
		delete[] _schedulerProc;
	}
}

result_t ProcessManager::Initialize(_IN_POINTER_ IScheduler<context_t>* scheduler_p,
												_IN_POINTER_ IVMemAllocator* procStackAllocator_p,
												_IN_POINTER_ IDTManager* idt_p,
												_IN_HANDLE_ handle_t idtHandle)
{
	_numProcs = FIRST_PROC_ID;
	if (scheduler_p == nullptr || procStackAllocator_p == nullptr)
	{
		return RES_INVALID_ARG;
	}

	_scheduler_p = scheduler_p;
	_procStackAllocator_p = procStackAllocator_p;

	//Install the yield handler
	idt_p->InstallHandler(INT_PROC_YIELD, ProcYeildHandler, this, idtHandle);

	//Install the process exit handler
	idt_p->InstallHandler(INT_PROC_EXIT, ProcExitHandler, this, idtHandle, 7);

	uint8_t* schedulerProcStack = this->_procStackAllocator_p->Allocate<uint8_t>(STACK_ALIGNMENT);
	schedulerProcStack[0] = uintptr_t(NoProc);
	_schedulerProc = new context_t(_numProcs++, uint64_t(schedulerProcStack), uint64_t(NoProc),
														 CPUGetCR3(), GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX);

	_schedulerProc->RSP -= 8;
	_schedulerProc->RDI = uintptr_t(this);
	_scheduler_p->Insert(_schedulerProc);

	return RES_SUCCESS;
}

result_t ProcessManager::CreateProcess(_IN_FUNCTION_ process_t entry_point, 
													_IN_ void* arg, 
													_OUT_ proc_id_t* procID_p,
													_OUT_OPTIONAL_ context_t* context_p)
{
	//Allocate a new stack and initialize the context.
	uint64_t* stack = this->_procStackAllocator_p->Allocate<uint64_t>(STACK_ALIGNMENT);
	stack[0] = uintptr_t(ProcessExit);

	//Set the stack so the entry point is the first thing that runs.
	context_t newContext = context_t(_numProcs++, uint64_t(stack), (uint64_t)entry_point,
												CPUGetCR3(), GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX);
	newContext.RSP -= 8;
	newContext.RDI = uintptr_t(arg);
	_scheduler_p->Insert(&newContext);

	*procID_p = newContext.ProcessID;
	if (context_p)
	{
		*context_p = newContext;
	}

	return RES_SUCCESS;
}

result_t ProcessManager::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
	__proc_manager__ = this;
	__curProc__ = _curProc;
	__nextProc__ = _nextProc;
	return RES_SUCCESS;
}

result_t ProcessManager::SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p)
{
	_curProc = __curProc__;
	_nextProc = __nextProc__;
	return RES_SUCCESS;
}

void ProcessManager::ScheduleNext()
{
	_curProc = __curProc__ = __nextProc__;
	_nextProc = __nextProc__ = _scheduler_p->GetNext();
}

void ProcessManager::StartProcess(proc_id_t procID)
{
	context_t* proc = nullptr;
	while ((proc = _scheduler_p->GetNext())->ProcessID != procID);

	__nextProc__ = proc;

	ProcessYield();
	return;
}

result_t ProcessManager::UnloadProcess(_IN_POINTER_ context_t* context_p)
{
	//Free the stack.
	_procStackAllocator_p->Deallocate((void*)context_p->OriginalRSP);

	//Remove the process from the scheduler.
	_scheduler_p->Remove(context_p);

	return RES_SUCCESS;
}

context_t* ProcessCreate(_IN_FUNCTION_ process_t entry_point, _IN_ void* arg)
{
	proc_id_t proc;
	context_t* newContext = new context_t();
	result_t res = __proc_manager__->CreateProcess(entry_point, arg, &proc, newContext);
	if(ResultFailed(res))
	{
		return nullptr;
	}

	return newContext;
}
