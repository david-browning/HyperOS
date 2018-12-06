#include "kernel_context.h"

void SwapContexts(_IN_POINTER_ context_t* const cur, _IN_POINTER_ context_t* const next)
{
   cur->BND0 = next->BND0;
   cur->BND1 = next->BND1;
   cur->BND2 = next->BND2;
   cur->BND3 = next->BND3;

   cur->BNDCFG = next->BNDCFG;
   cur->BNDSTATUS = next->BNDSTATUS;

   cur->SS = next->SS;
   cur->CS = next->CS;
   cur->DS = next->DS;
   cur->ES = next->ES;
   cur->FS = next->FS;
   cur->GS = next->GS;

   cur->CR3 = next->CR3;
   cur->MXCSR = next->MXCSR;
   cur->ProcessID = next->ProcessID;
   cur->RIP = next->RIP;
   cur->RFLAGS = next->RFLAGS;

   cur->RSP = next->RSP;
   cur->RDI = next->RDI;
   cur->RSI = next->RSI;
   cur->RBP = next->RBP;
   cur->RAX = next->RAX;
   cur->RBX = next->RBX;
   cur->RCX = next->RCX;
   cur->RDX = next->RDX;
   cur->R8 = next->R8;
   cur->R9 = next->R9;
   cur->R10 = next->R10;
   cur->R11 = next->R11;
   cur->R12 = next->R12;
   cur->R13 = next->R13;
   cur->R14 = next->R14;
   cur->R15 = next->R15;
}