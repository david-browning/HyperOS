#include "drvr_ps2kybd.h"
#include "vmem_allocator.h"
#include "kernel_debug.h"
#include "mem.h"
#include "kernel_procs.h"

static PS2KeyboardDriver* __keyboard_driver__;

PS2KeyboardDriver::PS2KeyboardDriver()
{

}

PS2KeyboardDriver::~PS2KeyboardDriver()
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

result_t PS2KeyboardDriver::Load(_IN_HANDLE_ handle_t driverHandle)
{
	result_t res = RES_SUCCESS;

	_handle = driverHandle;

	//Set the device name.
	char* kbName = "PS2 Keyboard";
	_devName = new char[MemoryLength(kbName)];
	MemoryCopyUntil<char>(_devName, kbName, '\0');

	//Set up the IRQ's this wants to respond to.
	_numIRQs = 1;
	_irqSubscriptions = new irqnumber_t[_numIRQs];
	_irqSubscriptions[0] = 0x21;

	//Set up keyboard driver specific stuff.
	_scanCodeSet = KEYBOARD_MAP_SET_1;
	_isDualChannel = false;

	//Disable port 1
	//Do not need to wait after writing to command port.
	IOWriteToPort<uint8_t>(KEYBAORD_COMMAND_PORT, 0xAD);

	//Disable port 2
	IOWriteToPort<uint8_t>(KEYBAORD_COMMAND_PORT, 0xA7);

	//Flush
	_readDataReg();

	//Read byte 0 from keyboard RAM to get current configuration
	IOWriteToPort<uint8_t>(KEYBAORD_COMMAND_PORT, 0x20);
	_waitForOutputFull();
	uint8_t config = this->_readDataReg();

	//Check that bit 5 was set. If set, then this is a dual channel PS2 controller.
	_isDualChannel = ((config & 0b0010'0000) != 0);

	//Modify config:
	config = 0b0110'0101;

	//Write config to controller.
	IOWriteToPort<uint8_t>(KEYBAORD_COMMAND_PORT, 0x60);
	_writeDataRegister(config);

	//Controller self test
	IOWriteToPort<uint8_t>(KEYBAORD_COMMAND_PORT, 0xAA);
	_waitForOutputFull();
	if (_readDataReg() != 0x55)
	{
		return RES_UNKNOWN_DEVICE_ERROR;
	}

	//Enable devices
	IOWriteToPort<uint8_t>(KEYBAORD_COMMAND_PORT, 0xAE);

	//Enable scanning
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, 0xF4);
	//_waitForOutputFull();
	if (_readDataReg() != 0xFA)
	{
		return RES_UNKNOWN_DEVICE_ERROR;
	}

	//Reset devices
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, 0xFF);
	_waitForOutputEmpty();
	if (_readDataReg() != 0xAA)
	{
		return RES_UNKNOWN_DEVICE_ERROR;
	}

	this->SetScanCodeSet(KEYBOARD_SCAN_SETS::SCAN_1);

	_isLoaded = true;
	return RES_SUCCESS;
}


result_t PS2KeyboardDriver::Write(_IN_BYTE_ARR_ const uint8_t * buffer, 
											 _IN_SIZE_ size_t bufLen, 
											 _IN_SIZE_ size_t byteOffset)
{
	return RES_UNSUPPORTED;
}

result_t PS2KeyboardDriver::Read(_IN_SIZE_ size_t bufLen, 
											_OUT_BYTE_ARR_ uint8_t * buffer,
											_IN_SIZE_ size_t byteOffset)
{
	return RES_UNSUPPORTED;
}

result_t PS2KeyboardDriver::Unload()
{
	return RES_SUCCESS;
}

result_t PS2KeyboardDriver::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
	__keyboard_driver__ = this;
	return RES_SUCCESS;
}

result_t PS2KeyboardDriver::SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p)
{
	return RES_SUCCESS;
}

result_t PS2KeyboardDriver::Interrupt(_IN_INT_ irqnumber_t irq)
{
	uint8_t code = _getScanCode();
	_kbBuffer.Push(&code);

	//Unblock processes waiting on this.
	this->UnblockFirst();

	return RES_SUCCESS;
}

VIRTUAL_KEYS PS2KeyboardDriver::GetNextVirtualKey()
{
	if (_kbBuffer.IsEmpty())
	{
		this->Block(GetCurProc());
	}

	uint8_t code;
	_kbBuffer.Pop(&code);

	VIRTUAL_KEYS ret = _scanCodeSet[code];
	if (_scanCodeSet == KEYBOARD_MAP_SET_1)
	{
		if (code > 0x80 && code < 0xE0)
		{
			//The key was released. Returns a key that matches the character but clear the pressed bit.
			ret = ReleaseVirtualKey(_scanCodeSet[code - 0x80]);
		}
		else if (code == 0xE0)
		{
			//More codes to follow
			ret = GetNextVirtualKey();
		}
	}
	else if (_scanCodeSet == KEYBOARD_MAP_SET_2)
	{
		if (code == 0xE0)
		{
			//Special key pressed or released.
			ret = GetNextVirtualKey();
		}
		else if (code == 0xF0)
		{
			//Key was released.
			ret = ReleaseVirtualKey(GetNextVirtualKey());
		}
	}
	else
	{

	}

	IDTManager::EnableInterrupts();
	return ret;
}

result_t PS2KeyboardDriver::SetScanCodeSet(_IN_ENUM_ KEYBOARD_SCAN_SETS set)
{
	//Write scan code set to keyboard.
	_waitForOutputEmpty();
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, 0xF0);
	_waitForOutputEmpty();
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, (uint8_t)set);

	switch (set)
	{
		case KEYBOARD_SCAN_SETS::SCAN_1:
		{
			this->_scanCodeSet = (VIRTUAL_KEYS*)KEYBOARD_MAP_SET_1;
			break;
		}
		case KEYBOARD_SCAN_SETS::SCAN_2:
		{
			this->_scanCodeSet = (VIRTUAL_KEYS*)KEYBOARD_MAP_SET_2;
			break;
		}
		case KEYBOARD_SCAN_SETS::SCAN_3:
		{
			//TODO Create scan code set 3.
			return RES_UNSUPPORTED;
			break;
		}
		default:
		{
			return RES_UNSUPPORTED;
		}
	}

	return RES_SUCCESS;
}

void PS2KeyboardDriver::_writeDataRegister(uint8_t data)
{
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, data);
	_waitForInputEmpty();
}

KEYBOARD_SCAN_SETS PS2KeyboardDriver::GetScanCodeSet()
{
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, 0xF0);
	IOWriteToPort<uint8_t>(KEYBOARD_DATA_PORT, 0x00);
	return KEYBOARD_SCAN_SETS::SCAN_1;
}

result_t KBHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
	PS2KeyboardDriver* kbp = (PS2KeyboardDriver*)data_p;
	if (irqNumber == 0x21)
	{
		return kbp->Interrupt(irqNumber);
	}

	return RES_NOT_AVAILABLE;
}

PS2KeyboardDriver * GetKeyboardDriverPtr()
{
	return __keyboard_driver__;
}
