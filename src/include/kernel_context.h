#pragma once
#include "annotations.h"

static constexpr uint64_t RFLAGS_DEFAULT = (1 << 9) | (1 << 1);


/*
 Do not change the order of any struct in this file.
*/

struct reg128_t
{
   uint64_t Upper;
   uint64_t Lower;

   reg128_t()
   {
      Upper = Lower = 0;
   }

   reg128_t(reg128_t* r)
   {
      Upper = r->Upper;
      Lower = r->Lower;
   }
}
__attribute__((packed));

struct reg256_t
{
   uint64_t B0;
   uint64_t B1;
   uint64_t B2;
   uint64_t B3;

   reg256_t()
   {
      B0 = B1 = B2 = B3 = 0;

   }
}
__attribute__((packed));

struct reg512_t
{
   uint64_t B0;
   uint64_t B1;
   uint64_t B2;
   uint64_t B3;
   uint64_t B4;
   uint64_t B5;
   uint64_t B6;
   uint64_t B7;

   reg512_t()
   {
      B0 = B1 = B2 = B3 = B4 = B5 = B6 = B7 = 0;
   }
}
__attribute__((packed));

struct context_t
{
   handle_t ProcessID;

   uint64_t R15;
   uint64_t R14;
   uint64_t R13;
   uint64_t R12;
   uint64_t R11;
   uint64_t R10;
   uint64_t R9;
   uint64_t R8;
   uint64_t RSP;
   uint64_t RBP;
   uint64_t RDX;
   uint64_t RCX;
   uint64_t RBX;
   uint64_t RAX;

   uint16_t SS;
   uint16_t CS;
   uint16_t DS;
   uint16_t ES;
   uint16_t FS;
   uint16_t GS;
   uint32_t Pad1;

   uint64_t RSI;
   uint64_t RDI;

   uint64_t RIP;
   uint64_t CR3;
   uint64_t RFLAGS;
   uint64_t MXCSR;

   reg128_t FPU0;
   reg128_t FPU1;
   reg128_t FPU2;
   reg128_t FPU3;
   reg128_t FPU4;
   reg128_t FPU5;
   reg128_t FPU6;
   reg128_t FPU7;

   uint64_t FPUIP;
   uint64_t FPUData;
   uint16_t FPUControl;
   uint16_t FPUStatus;
   uint16_t FPUTag;
   uint16_t FPUOp;


   #ifdef k1om

   reg512_t ZMM0;
   reg512_t ZMM1;
   reg512_t ZMM2;
   reg512_t ZMM3;
   reg512_t ZMM4;
   reg512_t ZMM5;
   reg512_t ZMM6;
   reg512_t ZMM7;
   reg512_t ZMM8;
   reg512_t ZMM9;
   reg512_t ZMM10;
   reg512_t ZMM11;
   reg512_t ZMM12;
   reg512_t ZMM13;
   reg512_t ZMM14;
   reg512_t ZMM15;
   reg512_t ZMM16;
   reg512_t ZMM17;
   reg512_t ZMM18;
   reg512_t ZMM19;
   reg512_t ZMM20;
   reg512_t ZMM21;
   reg512_t ZMM22;
   reg512_t ZMM23;
   reg512_t ZMM24;
   reg512_t ZMM25;
   reg512_t ZMM26;
   reg512_t ZMM27;
   reg512_t ZMM28;
   reg512_t ZMM29;
   reg512_t ZMM30;
   reg512_t ZMM31;

   uint16_t K0;
   uint16_t K1;
   uint16_t K2;
   uint16_t K3;
   uint16_t K4;
   uint16_t K5;
   uint16_t K6;
   uint16_t K7;

   #else

   uint64_t MMX0;
   uint64_t MMX1;
   uint64_t MMX2;
   uint64_t MMX3;
   uint64_t MMX4;
   uint64_t MMX5;
   uint64_t MMX6;
   uint64_t MMX7;

   reg128_t BND0;
   reg128_t BND1;
   reg128_t BND2;
   reg128_t BND3;
   uint64_t BNDCFG;
   uint64_t BNDSTATUS;

   reg128_t XMM0;
   reg128_t XMM1;
   reg128_t XMM2;
   reg128_t XMM3;
   reg128_t XMM4;
   reg128_t XMM5;
   reg128_t XMM6;
   reg128_t XMM7;
   reg128_t XMM8;
   reg128_t XMM9;
   reg128_t XMM10;
   reg128_t XMM11;
   reg128_t XMM12;
   reg128_t XMM13;
   reg128_t XMM14;
   reg128_t XMM15;

   reg256_t YMM0;
   reg256_t YMM1;
   reg256_t YMM2;
   reg256_t YMM3;
   reg256_t YMM4;
   reg256_t YMM5;
   reg256_t YMM6;
   reg256_t YMM7;
   reg256_t YMM8;
   reg256_t YMM9;
   reg256_t YMM10;
   reg256_t YMM11;
   reg256_t YMM12;
   reg256_t YMM13;
   reg256_t YMM14;
   reg256_t YMM15;

   #endif

   uint64_t OriginalRSP;
   bool IsBlocked;

   context_t(uint64_t procID, uint64_t rsp, uint64_t rip, uint64_t cr3, uint64_t cs) :
      ProcessID(procID), RSP(rsp), RIP(rip), CR3(cr3), CS(cs), OriginalRSP(rsp)
   {
      /*
      R15 = 0x1515151515151515;
      R14 = 0x1414141414141414;
      R13 = 0x1313131313131313;
      R12 = 0x1212121212121212;
      R11 = 0x1111111111111111;
      R10 = 0x1010101010101010;
      R9 = 0x09999999999999999;
      R8 = 0x08888888888888888;
      RDX = 0x0DDDDDDDDDDDDDDD;
      RCX = 0x0CCCCCCCCCCCCCCC;
      RBX = 0x0BBBBBBBBBBBBBBB;
      RAX = 0x0AAAAAAAAAAAAAAA;
      RSI = 0xEEEEEEEEEEEEEEEE;
      RDI = 0xFFFFFFFFFFFFFFFF;
      RBP = 0;
      SS = 0;
      DS = 0;
      ES = 0;
      FS = 0;
      GS = 0;
      RFLAGS = RFLAGS_DEFAULT;
      Pad1 = 0xFFFFFFFF;
      */

      R15 = 0;
      R14 = 0;
      R13 = 0;
      R12 = 0;
      R11 = 0;
      R10 = 0;
      R9 = 0;
      R8 = 0;
      RDX = 0;
      RCX = 0;
      RBX = 0;
      RAX = 0;
      RSI = 0;
      RDI = 0;
      RBP = 0;
      SS = 0;
      DS = 0;
      ES = 0;
      FS = 0;
      GS = 0;
      RFLAGS = RFLAGS_DEFAULT;
      Pad1 = 0xFFFFFFFF;
      IsBlocked = false;
   }

   context_t()
   {

   }

   friend bool operator<(context_t const& l, context_t const& r)
   {
      return false;
   }

   friend bool operator>(context_t const& l, context_t const& r)
   {
      return false;
   }

   friend bool operator==(context_t const &l, context_t const &r)
   {
      return l.ProcessID == r.ProcessID;
   }
}
__attribute__((packed));

