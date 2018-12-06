#include "process_blocker.h"
#include "kernel_procs.h"

ProcessBlocker::ProcessBlocker()
{
	_blockedProcs_p = nullptr;
}

ProcessBlocker::~ProcessBlocker()
{
	UnblockAll();
}

void ProcessBlocker::Block(_IN_POINTER_ context_t* proc_p)
{
	if (proc_p == nullptr)
	{
		return;
	}

	proc_p->IsBlocked = true;
	if (_blockedProcs_p == nullptr)
	{
		_blockedProcs_p = new ll_node_t<context_t*>(&proc_p);
	}
	else
	{
		LLInsertLast(_blockedProcs_p, &proc_p);
	}

	ProcessYield();
}

void ProcessBlocker::UnblockFirst()
{
	if (_blockedProcs_p == nullptr)
	{
		return;
	}

	//Mark the process as unblocked and remove it from this list.
	_blockedProcs_p->Data->IsBlocked = false;
	LLRemove(_blockedProcs_p, &_blockedProcs_p->Data);
}

void ProcessBlocker::UnblockAll()
{
	if (_blockedProcs_p == nullptr)
	{
		return;
	}

	auto temp = _blockedProcs_p;
	while (temp)
	{
		temp->Data->IsBlocked = false;
		temp = temp->Next;
	}

	LLFree(_blockedProcs_p);
	delete _blockedProcs_p;
	_blockedProcs_p = nullptr;
}
