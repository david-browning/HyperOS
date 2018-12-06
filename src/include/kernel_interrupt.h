#pragma once
#include "annotations.h"
#include "kernel_pic.h"
#include "kernel_gdt.h"
#include "kernel_interrupt_codes.h"
#include "ikernel_component.h"

typedef result_t(*InterruptHandlerFunc)(irqnumber_t irqNumber, uint32_t error, void* data_p);

struct interrupthandler_t
{
	InterruptHandlerFunc Handler;
	void* HandlerData;
};

struct idtentry_t
{
	uint16_t TargetOffsetLow;
	targetselector_t TargetSelector;
	uint8_t IST;
	uint8_t Attr;
	uint16_t TargetOffsetMedium;
	uint32_t TargetOffsetHigh;
	uint32_t IGNReserved;
}
__attribute__((packed));

static constexpr uint16_t IRQ_COUNT = 256;

LIB_EXPORT void INTPageFault();
LIB_EXPORT void INTGeneralProtectionFault();
LIB_EXPORT void INTDoubleFault();

/*
 Creates an IDT entry from a pointer to an assembly stub.
 target is the assembly function for the interrupt.
 */
inline void IRQMakeEntry(_IN_ uintptr_t target, _OUT_ idtentry_t* entry_p, _IN_OPT_ uint8_t ist = 0)
{
	entry_p->IGNReserved = 0;
	entry_p->TargetOffsetHigh = (target & 0xFFFFFFFF00000000) >> 32;
	entry_p->TargetOffsetMedium = (target & 0xFFFF'0000) >> 16;
	entry_p->IST = ist;
	entry_p->Attr = 0x8E;
	entry_p->TargetSelector = GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX;
	entry_p->TargetOffsetLow = target & 0xFFFF;
}

/*
 This is the function that gets called once an interrupt has occurred and saved its registers.
 The irqNumber is the interrupt number and the error is an error given by the CPU. If the CPU
 does not give an error code, this is 0.
 */
#if defined(__cplusplus)
extern "C"
#endif
extern KERNEL_FUNC LIB_EXPORT result_t IRQHandle(irqnumber_t irqNum, uint32_t error);

/*
 Manages the interrupt descriptor tables.
 Only one instance of the class can exist at once so do not instantiate it.
 The kernel keeps the 1 static copy of it and it can be retrieved by IDTGetIDTManagerPtr().
*/
class IDTManager : public IKernelComponent
{
	public:
	KERNEL_FUNC IDTManager();

	KERNEL_FUNC result_t Initialize();

	/*
	 Adds a new interrupt descriptor table to the list of available IDTs.
	 This does not install the IDT. Use Swap to swap in an IDT that was loaded into
	 this manager class.
	 If this function returns successfully, then the table and number of entries passed to this can
	 be freed or go out of scope.
	 The new IDT has no interrupts associated with it. Swap to the newly installed IDT and use
	 InstallHandler() to install handler functions.
	 Returns RES_ARRAY_FULL if there is no more room to add an IDT.
	 Returns RES_INVALID_ARG if there are too many entries in the IDT that is passed.
	 */
	KERNEL_FUNC result_t AllocateIDT(_OUT_HANDLE_ handle_t* handle_p,
												_IN_OPT_ InterruptHandlerFunc defaultHandler = nullptr);

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

	KERNEL_FUNC void InstallHandler(_IN_INT_ irqnumber_t irqNum,
											  _IN_FUNCTION_ InterruptHandlerFunc handlerFunc,
											  _IN_POINTER_ void* data,
											  _IN_HANDLE_ handle_t handle,
											  uint8_t ist = 0);

	inline static KERNEL_FUNC void DisableInterrupts()
	{
		__asm
		{
			cli;
		}
	}

	inline static KERNEL_FUNC void EnableInterrupts()
	{
		__asm
		{
			sti;
		}
	}

	/*
	 Gets the interrupt handler for the IRQ number. The handler returned is the handler for the
	 currently swapped-in interrupt descriptor table.
	 */
	const interrupthandler_t* GetIRQHandler(_IN_INT_ irqnumber_t irqNum);

	PICDriver PIC;

	private:
	static constexpr size_t MAX_IDT = 4;

	struct _idt_t
	{
		interrupthandler_t Functions[IRQ_COUNT];
		idtentry_t Table[IRQ_COUNT];
	};

	_idt_t* _curIDT;
	_idt_t _idts[MAX_IDT];
	handle_t _nextHandle;
};