#include "kernel_cpu.h"

uint64_t CPUGetCR3()
{
	uint64_t ol4;
	__asm
	{
		push rax;
		mov rax, cr3;
		mov ol4, rax;
		pop rax;
	};
	return ol4;
}

uint64_t CPUGetCR2()
{
	uint64_t ol4;
	__asm
	{
		push rax;
		mov rax, cr2;
		mov ol4, rax;
		pop rax;
	};
	return ol4;
}

void CPUSetCR3(uintptr_t l4BaseAddress)
{
	//CR3 register pointing to page table level 4.
	struct cr3_reg_t
	{
		uint64_t Reserved1 : 3;
		uint64_t PWT : 1;
		uint64_t PCD : 1;
		uint64_t Reserved2 : 7;
		uint64_t L4BaseAddress : 40;
		uint64_t Reserved3 : 12;
	};

	cr3_reg_t cr;
	cr.PCD = 0;
	cr.PWT = 0;
	cr.Reserved1 = 0;
	cr.Reserved2 = 0;
	cr.Reserved3 = 0;
	//l4s is an array of pointers to l4s. The 0th entry is a pointer to the 0th l4 entry in a page map.
	cr.L4BaseAddress = l4BaseAddress;
	uint64_t cr3val = *(uint64_t*)&cr;

	__asm
	{
		push r8;
		mov r8, cr3val;
		mov cr3, r8;
		pop r8;
	}

}
