#pragma once
#include "annotations.h"
#include "drvr_vga.h"

inline void CPUHalt()
{
   __asm
   {
      hlt;
   }
}

inline void CPUHaltError(_IN_POINTER_ VGADriver* const vgaDrvr)
{
   vgaDrvr->WriteString(CONSOLE_ERROR_ATTRIBUTE, "\nCPU HALTED.\n");

   __asm
   {
      hlt;
   }
}

inline void CPUID(uint32_t code, uint32_t* a_p, uint32_t* d_p)
{
   __asm
   {
      mov r8, a_p;
      mov r9, d_p;
      mov eax, code
         cpuid;
      mov[r8], rax;
      mov[r9], rdx;
   }
}

inline void CPUGetMSR(uint32_t msr, uint32_t* lo, uint32_t* hi)
{
   __asm
   {
      mov r8, lo;
      mov r9, hi;
      mov ecx, msr;
      rdmsr;
      mov[r8], rax;
      mov[r9], rdx;
   }
}

inline void CPUSetMSR(uint32_t msr, uint32_t lo, uint32_t hi)
{
   __asm
   {
      mov eax, lo;
      mov edx, hi;
      mov ecx, msr;
      wrmsr;
   }
}

inline void CPUReset()
{
   __asm
   {
      ; Wait for a empty Input Buffer
      wait1 :
      in   al, 0x64
         test al, 00000010b
         jne  wait1

         ; Send 0xFE to the keyboard controller.
         mov  al, 0xFE
         out  0x64, al
   }
}

inline uint64_t CPUGetRSP()
{
   __asm
   {
      mov rax, rsp;
   }
}

extern LIB_EXPORT uint64_t CPUGetCR3();

extern LIB_EXPORT uint64_t CPUGetCR2();

extern LIB_EXPORT KERNEL_FUNC void CPUSetCR3(uintptr_t l4BaseAddress);