#include "kernel_multiboot.h"
#include "mem.h"

MultiBootManager::MultiBootManager()
{
   this->_numModueles = 0;
   this->_bootLoaderName[0] = '\0';
   this->_bootCMD[0] = '\0';
   _highestVirtAddress = 0;
}

result_t MultiBootManager::Load(_IN_POINTER_ multiboot_start_tag_t* startTag_p)
{
   multiboot_tag_t* curTag = (multiboot_tag_t*)(uintptr_t(startTag_p) +
                                                sizeof(multiboot_start_tag_t));
   result_t res = RES_SUCCESS;

   while (curTag->Type != MULTIBOOT_TAG_TYPES::END)
   {
      switch (curTag->Type)
      {
         case MULTIBOOT_TAG_TYPES::BOOT_COMMAND_LINE:
         {
            size_t numBytes = curTag->Size - sizeof(multiboot_tag_t);
            if (numBytes > MultiBootManager::BOOT_CMD_MAX_CHARS)
            {
               numBytes = MultiBootManager::BOOT_CMD_MAX_CHARS;
            }

            MemoryCopy(this->_bootCMD,
               (char*)((multiboot_tag_boot_cmd_t*)curTag)->String,
                       numBytes);

            this->_bootCMD[numBytes - 1] = '\0';
            break;
         }
         case MULTIBOOT_TAG_TYPES::BOOT_LOADER_NAME:
         {
            size_t numBytes = curTag->Size - sizeof(multiboot_tag_t);
            if (numBytes > MultiBootManager::BOOTLOADER_NAME_MAX_CHARS)
            {
               numBytes = MultiBootManager::BOOTLOADER_NAME_MAX_CHARS;
            }

            MemoryCopy(this->_bootLoaderName,
               (char*)((multiboot_tag_boot_loader_name_t*)curTag)->String,
                       numBytes);

            this->_bootLoaderName[numBytes - 1] = '\0';
            break;
         }
         case MULTIBOOT_TAG_TYPES::MODULE:
         {
            if (_numModueles > MAX_LOADABLE_MODULES)
            {
               res = RES_OUT_OF_MEMORY;
            }
            else
            {
               const multiboot_tag_module_t* mod = static_cast<multiboot_tag_module_t*>(curTag);

               this->_modules[_numModueles].StartAddress = mod->ModStart;
               this->_modules[_numModueles].Size = mod->ModEnd - mod->ModStart;

               size_t numBytes = mod->Size - sizeof(multiboot_tag_t) - sizeof(uint64_t) - sizeof(uint64_t);
               if (numBytes > MODULE_NAME_MAX_LEN)
               {
                  numBytes = MODULE_NAME_MAX_LEN;
               }

               MemoryCopy<char>(this->_modules[_numModueles].Name, (char*)mod->String, numBytes);

               _numModueles++;
            }

            break;
         }
         case MULTIBOOT_TAG_TYPES::BASIC_MEMORY_INFORMATION:
         {
            multiboot_tag_basic_mem_info_t* memInfo = static_cast<multiboot_tag_basic_mem_info_t*>(curTag);
            this->_lowMem = memInfo->MemLower;
            this->_highMem = memInfo->MemUpper;
            break;
         }
         case MULTIBOOT_TAG_TYPES::BIOS_BOOT_DEVICE:
         {

            multiboot_bios_boot_dev_t* bootDev = static_cast<multiboot_bios_boot_dev_t*>(curTag);

            if (bootDev->BIOSDevice == 0x00 || bootDev->BIOSDevice == 0x80)
            {
               this->_bootDevice.IsEFI = false;
            }
            else
            {
               this->_bootDevice.IsEFI = true;
            }

            this->_bootDevice.BIOSDevice = bootDev->BIOSDevice;
            this->_bootDevice.Partition = bootDev->Partition;
            this->_bootDevice.SubPartition = bootDev->SubPartition;

            break;
         }
         case MULTIBOOT_TAG_TYPES::MEMORY_MAP:
         {
            multiboot_tag_mem_map_t* memmap = static_cast<multiboot_tag_mem_map_t*>(curTag);
            static constexpr size_t bytesBeforeEntry = sizeof(multiboot_tag_t) +
               sizeof(multiboot_tag_mem_map_t::EntrySize) + sizeof(multiboot_tag_mem_map_t::EntryVersion);

            this->_numMemRegions = (curTag->Size - bytesBeforeEntry) / memmap->EntrySize;

            multiboot_tag_memory_map_entry_t* entry =
               (multiboot_tag_memory_map_entry_t*)((uintptr_t)memmap + bytesBeforeEntry);

            for (auto i = 0; i < this->_numMemRegions; i++)
            {
               this->_memRegions[i] = memory_region_t
               {
                  .BaseAddress = entry->BaseAddress,
                  .HighAddress = entry->BaseAddress + entry->LengthBytes,
                  .Reserved = 0,
                  .Type = entry->Type,
               };

               entry = (multiboot_tag_memory_map_entry_t*)((uintptr_t)entry + memmap->EntrySize);
            }

            break;
         }
         case MULTIBOOT_TAG_TYPES::VBE_INFO:
         {
            const	multiboot_tag_vbe_t* vbeInfo = static_cast<multiboot_tag_vbe_t*>(curTag);
            this->_vbe.Mode = vbeInfo->Mode;
            this->_vbe.InterfaceSegment = vbeInfo->InterfaceSeg;
            this->_vbe.InterfaceOffset = vbeInfo->InterfaceOffset;
            this->_vbe.InterfaceLength = vbeInfo->InterfaceLen;
            MemoryCopy<uint8_t>(this->_vbe.ControlInfo, vbeInfo->ControlInfo, 512);
            MemoryCopy<uint8_t>(this->_vbe.ModeInfo, vbeInfo->ModeInfo, 256);

            break;
         }
         case MULTIBOOT_TAG_TYPES::FRAME_BUFFER_INFO:
         {
            //Requires special care so it doesn't overwrite other class members. 

            const multiboot_tag_framebuffer_t* frmbuf = static_cast<multiboot_tag_framebuffer_t*>(curTag);

            this->_frameBuffer.PhysicalAddress = frmbuf->FrameBufferAddress;
            this->_frameBuffer.Pitch = frmbuf->FrameBufferPitch;

            this->_frameBuffer.BitsPerPixel = frmbuf->FrameBufferBPP;


            this->_frameBuffer.WidthPixels = frmbuf->FrameBufferType == 2 ? 0 : frmbuf->FrameBufferWidth;
            this->_frameBuffer.HeightPixels = frmbuf->FrameBufferType == 2 ? 0 : frmbuf->FrameBufferHeight;

            this->_frameBuffer.WidthChars = frmbuf->FrameBufferType == 2 ? 0 : frmbuf->FrameBufferWidth;
            this->_frameBuffer.HeightChars = frmbuf->FrameBufferType == 2 ? 0 : frmbuf->FrameBufferHeight;

            this->_frameBuffer.CursorCol = 0;
            this->_frameBuffer.CursorRow = 0;

            break;
         }
         case MULTIBOOT_TAG_TYPES::EFL_SYMBOLS:
         {
            const multiboot_tag_elf_tags_t* elfTags = static_cast<multiboot_tag_elf_tags_t*>(curTag);

            this->_kernelElf.NumSectionHeaderEntries = uint16_t(elfTags->Num);
            this->_kernelElf.SectionNamesIndex = uint16_t(elfTags->SHIndex);
            this->_kernelElf.SectionHeaderTableEntrySize = uint16_t(elfTags->EntrySize);

            //Check that the entry size matches the struct size
            if (this->_kernelElf.SectionHeaderTableEntrySize != sizeof(elf_section_header_t))
            {
               res = RES_UNSUPPORTED;
            }

            elf_section_header_t* curSec = (elf_section_header_t*)(uintptr_t(elfTags) + 20);
            elf_section_header_t* stringSec = (elf_section_header_t*)(uintptr_t(curSec) + (elfTags->SHIndex * elfTags->EntrySize));
            for (auto i = 0; i < elfTags->Num; i++)
            {

               if (_highestVirtAddress < curSec->SectionVirtAddress + curSec->SectionSizeBytes)
               {
                  _highestVirtAddress = curSec->SectionVirtAddress + curSec->SectionSizeBytes;
               }

               curSec = (elf_section_header_t*)(uintptr_t(curSec) + elfTags->EntrySize);
            }

            break;
         }
         case MULTIBOOT_TAG_TYPES::APM_TABLE:
         case MULTIBOOT_TAG_TYPES::END:
         default:
         {
            break;
         }
      }

      curTag = (multiboot_tag_t*)(uintptr_t(curTag) + (curTag->Size + 7) & ~7);
   }

   return res;
}

void MultiBootManager::GetBestMemRegion(_OUT_ memory_region_t* region_p)
{
   region_p->HighAddress = 0;
   for (auto i = 0; i < _numMemRegions; i++)
   {
      //Find a chunk of RAM
      if (_memRegions[i].Type == MEMORY_TYPES::RAM)
      {
         //Find the biggest chunk of RAM
         if (_memRegions[i].HighAddress >= region_p->HighAddress)
         {
            *region_p = _memRegions[i];
         }
      }
   }

   //If the biggest chunk contains reserved data, just change the base
   uintptr_t highestAlignedAddress = MemAlignAddress(_highestVirtAddress, 8);
   if (region_p->BaseAddress < highestAlignedAddress)
   {
      region_p->BaseAddress = highestAlignedAddress;
   }
}
