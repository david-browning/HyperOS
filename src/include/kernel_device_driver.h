#pragma once

#include "annotations.h"
#include "kernel_interrupt.h"
#include "ikernel_component.h"
#include "process_blocker.h"

class IDeviceDriver : public virtual IKernelComponent, protected virtual ProcessBlocker
{
	public:
	IDeviceDriver()
	{
		_irqSubscriptions = nullptr;
		_isLoaded = false;
		_devName = nullptr;
		_numIRQs = 0;
		_handle = 0;
	}

	virtual result_t Load(_IN_HANDLE_ handle_t driverHandle) = 0;

	virtual result_t Write(_IN_BYTE_ARR_ const uint8_t* buffer, _IN_SIZE_ size_t bufLen, _IN_SIZE_ size_t byteOffset) = 0;

	virtual result_t Read(_IN_SIZE_ size_t bufLen, _OUT_BYTE_ARR_ uint8_t* buffer, _IN_SIZE_ size_t byteOffset) = 0;

	virtual result_t Unload() = 0;

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

	virtual result_t Interrupt(irqnumber_t irq) = 0;

	//Driver manager uses this to know what IRQs this driver wants.
	irqnumber_t* GetIRQSubscriptions(_OUT_SIZE_ size_t* numSubs_p)
	{
		*numSubs_p = _numIRQs;
		return _irqSubscriptions;
	}

	protected:
	irqnumber_t* _irqSubscriptions;
	size_t _numIRQs;
	handle_t _handle;
	char* _devName;
};
