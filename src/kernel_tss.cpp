#include "kernel_tss.h"
#include "mem.h"
#include "kernel_debug.h"

TSSManager::TSSManager()
{
	_isLoaded = false;
}

result_t TSSManager::Initialize()
{
	_curTSS = nullptr;
	_nextHandle = 0;
	_isLoaded = true;
	return RES_SUCCESS;
}

result_t TSSManager::AllocateTSS(_IN_INT_ uint16_t gdtOffset, 
											_OUT_HANDLE_ handle_t* tssHandle_p)
{
	if (_nextHandle >= MAX_TSS)
	{
		return RES_ARRAY_FULL;
	}

	_tss[_nextHandle].GDTOffset = gdtOffset;
	static const _task_segment_t nullSegment =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	_tss[_nextHandle].TSS = nullSegment;

	*tssHandle_p = _nextHandle;
	_nextHandle++;
	return RES_SUCCESS;
}

result_t TSSManager::SetStack(_IN_INT_ uint8_t stackIndex, 
										_IN_INT_ uintptr_t stackAddr,
										_IN_SIZE_ size_t stackSize,
										_IN_HANDLE_ handle_t tssHandle)
{
	if (tssHandle >= MAX_TSS || stackIndex < 1 || stackIndex > 7)
	{
		return RES_INVALID_ARG;
	}

	*((uint64_t*)&_tss[tssHandle].TSS.IST1 + stackIndex - 1) = stackAddr + stackSize - 8;
	return RES_SUCCESS;
}

result_t TSSManager::SwapIn(_IN_POINTER_ kernel_swap_state_t * state_p)
{
	if (state_p->ThisHandle >= MAX_TSS)
	{
		return RES_INVALID_ARG;
	}

	_curTSS = &_tss[state_p->ThisHandle];
	uint16_t tss = _curTSS->GDTOffset;

	__asm
	{
		mov ax, tss;
		ltr ax;
	}

	return RES_SUCCESS;
}

result_t TSSManager::SwapOut(_IN_POINTER_ kernel_swap_state_t * state_p)
{
	return RES_SUCCESS;
}
