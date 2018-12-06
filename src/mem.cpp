#include "mem.h"
#include "kernel_cpu.h"

uintptr_t GetRealAddress(uintptr_t vaddr)
{
   uint64_t* l1Entry = GetL1EntryPtr(CPUGetCR3(), vaddr);
   *l1Entry = *l1Entry & ~0xFFF;
   return *l1Entry;
}
