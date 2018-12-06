#pragma once

#include "annotations.h"
#include "kernel_io.h"

extern bool APICSupported();

class PICDriver
{
	public:
	/*
	Call this at the end of an interrupt to tell it the interrupt was handled.
	*/
	KERNEL_FUNC void SendEOI(uint8_t irq);

	/*
	arguments:
	offset1 - vector offset for master PIC
	vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
	*/
	KERNEL_FUNC void Remap(uint8_t offset1, uint8_t offset2);

	/*
	Disables the PIC so you can use the local CPU APIC and IOAPIC
	*/
	inline KERNEL_FUNC void Disable()
	{
		IOWriteToPort<uint8_t>(0xA1, 0xFF);
		IOWriteToPort<uint8_t>(0x21, 0xFF);
	}

	KERNEL_FUNC result_t EnableAPIC();

	/*
	Causes the PIC to ignore interrupt along the IRQ Line.
	*/
	KERNEL_FUNC void SetMask(uint8_t irqLine);

	/*
	Causes the PIC to raise an interrupt for an interrupt along the IRQ Line.
	*/
	KERNEL_FUNC void ClearMask(uint8_t irqLine);

	inline uint8_t GetOffset1()
	{
		return _off1;
	}

	inline uint8_t GetOffset2()
	{
		return _off2;
	}

	inline uint16_t GetIIR()
	{
		return _getreg(0x0A);
	}

	inline uint16_t GetISR()
	{
		return _getreg(0x0B);
	}

	private:
	uint8_t _off1;
	uint8_t _off2;

	uint16_t _getreg(uint8_t ocw3);
};