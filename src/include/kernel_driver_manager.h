#pragma once
#include "annotations.h"
#include "kernel_interrupt.h"
#include "kernel_device_driver.h"
#include "ikernel_component.h"
#include "vmem_stack_allocator.h"

class DriverManager : public IKernelComponent
{
	public:
	DriverManager();

	result_t Initialize(_IN_POINTER_ StackAllocator* stackAllocator_p);

	/*
	 Installs a driver into the driver manager.
	 After this returns successfully, driver_p can be freed or go out of scope.
	 */
	result_t InstallDriver(_IN_POINTER_ IDeviceDriver* driver_p,
								  _IN_SIZE_ size_t driverSize,
								  _OUT_HANDLE_ handle_t* devHandle_p);

	/*
	 Uninstalls a driver from the driver manager.
	 */
	result_t RemoveDriver(_IN_HANDLE_ handle_t devHandle);

	/*
	 Given an interrupt number, this executes any interrupt a device is subscribed to.
	 */
	result_t HandleInterrupt(_IN_INT_ irqnumber_t irq);

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

	/*
	 Swaps a device driver in so its the driver in charge of a device.
	 */
	result_t SwapDriverIn(_IN_HANDLE_ handle_t devHandle);

	private:
	IDeviceDriver** _drivers;
	size_t _numDrivers;
	size_t _driversArraySize;
	StackAllocator* _stacks_p;

	static constexpr size_t _initialDriverCount = 8;
};