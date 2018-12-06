#pragma once

#include "annotations.h"
#include "kernel_device_driver.h"
#include "vfs.h"

enum class BLOCK_DEVICE_TYPES
{
	MASS_STORAGE,
	PARTITION,
	CHARACTER,
};

class IBlockDevDriver : public IDeviceDriver
{
	public:
	IBlockDevDriver(_IN_SIZE_ size_t blockSizeBytes)
	{
		_blockSize = blockSizeBytes;
	}
	
	virtual result_t Load(_IN_HANDLE_ handle_t driverHandle) = 0;

	virtual result_t Write(_IN_BYTE_ARR_ const uint8_t* buffer, _IN_SIZE_ size_t bufLen, _IN_SIZE_ size_t byteOffset) = 0;

	virtual result_t Read(_IN_SIZE_ size_t bufLen, _OUT_BYTE_ARR_ uint8_t* buffer, _IN_SIZE_ size_t byteOffset) = 0;

	virtual result_t Unload() = 0;

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p) = 0;

	virtual result_t Interrupt(irqnumber_t irq) = 0;


	protected:
	size_t _blockSize;
	
	BLOCK_DEVICE_TYPES _type;
};