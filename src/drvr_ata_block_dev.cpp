#include "drvr_ata_block_dev.h"

static constexpr ioport_t PRIMARY_DEV_CONTROL_REG_PORT = 0x3F6;
static constexpr ioport_t SECONDARY_DEV_CONTROL_REG_PORT = 0x376;

static constexpr irqnumber_t PRIMARY_BUS_IRQ = 14;
static constexpr irqnumber_t SECONDARY_BUS_IRQ = 15;

static constexpr uint8_t FLUSH_WRITE_CACHE_CMD = 0xE7;

ATABlockDriver::ATABlockDriver() : IBlockDevDriver(ATA_BLOCK_SIZE)
{

}

ATABlockDriver::~ATABlockDriver()
{
	if (_devName)
	{
		delete[] _devName;
		_devName = nullptr;
	}

	if (_irqSubscriptions)
	{
		delete[] _irqSubscriptions;
		_irqSubscriptions = nullptr;
	}
}

result_t ATABlockDriver::Load(_IN_HANDLE_ handle_t driverHandle)
{
	result_t res = RES_SUCCESS;
	_handle = driverHandle;
	char* devName = "ATA Driver";
	_devName = new char[MemoryLength(devName)];
	MemoryCopyUntil<char>(_devName, devName, '\0');
	
	//Set up the IRQs




	return res;
}

result_t ATABlockDriver::Write(_IN_BYTE_ARR_ const uint8_t * buffer, 
										 _IN_SIZE_ size_t bufLen, 
										 _IN_SIZE_ size_t byteOffset)
{
	return RES_SUCCESS;
}

result_t ATABlockDriver::Read(_IN_SIZE_ size_t bufLen,
										_OUT_BYTE_ARR_ uint8_t* buffer,
										_IN_SIZE_ size_t byteOffset)
{
	return RES_SUCCESS;
}

result_t ATABlockDriver::Unload()
{
	return RES_SUCCESS;
}

result_t ATABlockDriver::SwapIn(_IN_POINTER_ kernel_swap_state_t * state_p)
{
	return RES_SUCCESS;
}

result_t ATABlockDriver::SwapOut(_IN_POINTER_ kernel_swap_state_t * state_p)
{
	return RES_SUCCESS;
}

result_t ATABlockDriver::Interrupt(irqnumber_t irq)
{
	return RES_SUCCESS;
}
