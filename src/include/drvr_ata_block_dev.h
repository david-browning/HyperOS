#pragma once

#include "annotations.h"
#include "drv_block_dev.h"

static constexpr size_t ATA_BLOCK_SIZE = 512;

class ATABlockDriver : public IBlockDevDriver
{
	public:
	ATABlockDriver();

	virtual ~ATABlockDriver();

	virtual result_t Load(_IN_HANDLE_ handle_t driverHandle);

	virtual result_t Write(_IN_BYTE_ARR_ const uint8_t* buffer, _IN_SIZE_ size_t bufLen, _IN_SIZE_ size_t byteOffset);

	virtual result_t Read(_IN_SIZE_ size_t bufLen, _OUT_BYTE_ARR_ uint8_t* buffer, _IN_SIZE_ size_t byteOffset);

	virtual result_t Unload();

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t Interrupt(irqnumber_t irq);

	private:
};
