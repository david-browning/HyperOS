#include "kernel_main.h"
#include "kernel_print.h"
#include "mem.h"
#include "kernel_gdt.h"
#include "kernel_tss.h"
#include "kernel.h"
#include "kernel_debug.h"

result_t kernel_main(multiboot_start_tag_t* mbstart_p)
{
   result_t res = RES_SUCCESS;
   char errorString[64];

   //Read the multiboot stuff to figure out where the kernel is.
   MultiBootManager mbm = MultiBootManager();
   res = mbm.Load(mbstart_p);
   if (ResultFailed(res))
   {
      return res;
   }

   //Create a page allocator and give it the areas of memory it can operate in.
   memory_region_t kernelMem;
   //This gets the biggest area of memory that does not conflict with anything.
   mbm.GetBestMemRegion(&kernelMem);

   GDTManager gdt = GDTManager();
   TSSManager tss = TSSManager();

   static constexpr size_t numKernels = 1;
   size_t kernelIndex = 0;

   Kernel kernels[numKernels];
   kernel_swap_state_t kernelStates[numKernels];

   //Initialize the kernels.
   for (auto i = 0; i < numKernels; i++)
   {
      kernelStates[i].Flags = 0b10;
      if (ResultFailed(res = kernels[i].Initialize(&kernelMem, &gdt, &tss)))
      {
         return res;
      }
   }

   auto i = 0;
   while (res != RES_SHUTDOWN)
   {
      result_t kernelState = kernels[i].SwapIn(&kernelStates[i]);
   }

   return RES_SUCCESS;
}