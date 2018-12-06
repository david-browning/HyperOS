#pragma once

#include "annotations.h"
#include "mem.h"
#include "kernel_module.h"
#include "kernel_boot.h"
#include "kernel_video.h"
#include "elf.h"

enum class MULTIBOOT_TAG_TYPES : uint32_t
{
	END = 0,
	BOOT_COMMAND_LINE = 1,
	BOOT_LOADER_NAME = 2,
	MODULE = 3,
	BASIC_MEMORY_INFORMATION = 4,
	BIOS_BOOT_DEVICE = 5,
	MEMORY_MAP = 6,
	VBE_INFO = 7,
	FRAME_BUFFER_INFO = 8,
	EFL_SYMBOLS = 9,
	APM_TABLE = 10,
};

struct multiboot_start_tag_t
{
	//Total size of the boot information, including this field and the terminating tag.
	uint32_t TotalSizeBytes;
	//Ignore, Set to 0.
	uint32_t Reserved;
}
__attribute__((packed));

struct multiboot_tag_t
{
	MULTIBOOT_TAG_TYPES Type;
	uint32_t Size;
}
__attribute__((packed));

struct multiboot_tag_basic_mem_info_t : public multiboot_tag_t
{
	//Amount of memory in KB. Lower memory starts at address 0.
	//Maximum value is 640
	uint32_t MemLower;

	//Upper memory starts at address 1MB.
	uint32_t MemUpper;
}
__attribute__((packed));

//Indicates which BIOS disk device the bootloader loaded the OS from.
struct multiboot_bios_boot_dev_t : public multiboot_tag_t
{
	//BIOS drive number. 0x00 for first floppy. ox80 for first hard disk.
	uint32_t BIOSDevice;
	uint32_t Partition;
	uint32_t SubPartition;
}
__attribute__((packed));

struct multiboot_tag_boot_cmd_t : public multiboot_tag_t
{
	uint8_t String[0];
}
__attribute__((packed));

struct multiboot_tag_module_t : public multiboot_tag_t
{
	//Start address of the boot module.
	uint64_t ModStart;

	//End address of the boot module.
	uint64_t ModEnd;
	uint8_t String[0];
}
__attribute__((packed));

struct multiboot_tag_elf_tags_t : public multiboot_tag_t
{
	uint32_t Num;
	uint32_t EntrySize;
	uint32_t SHIndex;
	uint8_t Sections[0];
}
__attribute__((packed));

struct multiboot_tag_memory_map_entry_t
{
	uint64_t BaseAddress;

	//Size of the memory region.
	uint64_t LengthBytes;

	MEMORY_TYPES Type;

	uint32_t Reserved;
}
__attribute__((packed));

struct multiboot_tag_mem_map_t : public multiboot_tag_t
{
	//Size of an entry in the map.
	uint32_t EntrySize;

	uint32_t EntryVersion;

	multiboot_tag_memory_map_entry_t FirstEntry[0];
};

struct multiboot_tag_boot_loader_name_t : public multiboot_tag_t
{
	uint8_t String[0];
};

struct multiboot_tag_apm_table_t : public multiboot_tag_t
{
	uint16_t Version;
	uint16_t CodeSeg;
	//Offset to the entry point.
	uint32_t Offset;
	uint16_t CodeSeg16;
	uint16_t DataSeg;
	uint16_t Flags;
	uint16_t CodeSegLen;
	uint16_t CodeSeg16Len;
	uint16_t DataSegLen;
};

struct multiboot_tag_vbe_t : public multiboot_tag_t
{
	uint16_t Mode;
	uint16_t InterfaceSeg;
	uint16_t InterfaceOffset;
	uint16_t InterfaceLen;
	uint8_t ControlInfo[512];
	uint8_t ModeInfo[256];
};

struct multiboot_framebuffer_palette_t
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

struct multiboot_tag_framebuffer_t : public multiboot_tag_t
{
	uint64_t FrameBufferAddress;
	uint32_t FrameBufferPitch;
	uint32_t FrameBufferWidth;
	uint32_t FrameBufferHeight;

	//Bits per pixel.
	uint8_t FrameBufferBPP;

	//0 means indexed color. 1 mean direct RGB. 2 means EGA text.
	//If 2, then width and height are specified in characters, not pixels bpp is 16
	//and pitch is bytes per text line.
	uint8_t FrameBufferType;
	uint8_t Reserved;

	union 
	{
		struct 
		{
			uint16_t NumColors;
			multiboot_framebuffer_palette_t Palette[0];
		}
		Indexed;

		struct 
		{
			uint8_t RFieldPos;
			uint8_t RMaskSize;
			uint8_t GFieldPos;
			uint8_t GMaskSize;
			uint8_t BFieldPos;
			uint8_t BMaskSize;
		}
		DirectRGB;
	}
	ColorMode;
};

/*
 Holds a copy of the information passed from the boot loader to the kernel.
 Instantiate this immediately after entering long mode and use Load to copy the boot
 info into this. After calling Load, the kernel can use the memory where the boot info
 was saved.
 If this class goes out of scope, all data is lost.
 */
class MultiBootManager
{
	public:
	MultiBootManager();

	result_t Load(_IN_POINTER_ multiboot_start_tag_t* startTag_p);

	/*
	 Returns a pointer to the loader name. You are not allowed to change this.
	 */
	inline const char* GetBootLoaderName()
	{
		return this->_bootLoaderName;
	}

	/*
	 Returns a pointer to the boot command line arguments string passed to the kernel.
	 You are not allowed to change this.
	 */
	inline const char* GetBootCMD()
	{
		return this->_bootCMD;
	}

	inline size_t GetNumMemoryRegions()
	{
		return _numMemRegions;
	}

	inline uintptr_t GetLowMemAddress()
	{
		return _lowMem;
	}

	inline uintptr_t GetHighMemAddress()
	{
		return _highMem;
	}

	void GetBestMemRegion(_OUT_ memory_region_t* region_p);

	private:
	static constexpr size_t BOOTLOADER_NAME_MAX_CHARS = 32;
	static constexpr size_t BOOT_CMD_MAX_CHARS = 1024;
	static constexpr size_t MAX_LOADABLE_MODULES = 8;
	static constexpr size_t MAX_MEM_REGIONS = 128;

	char _bootLoaderName[BOOTLOADER_NAME_MAX_CHARS];
	char _bootCMD[BOOT_CMD_MAX_CHARS];

	module_t _modules[MAX_LOADABLE_MODULES];
	size_t _numModueles;

	uintptr_t _lowMem;
	uintptr_t _highMem;
	bootdevice_t _bootDevice;

	memory_region_t _memRegions[MAX_MEM_REGIONS];
	size_t _numMemRegions;
	uintptr_t _highestVirtAddress;
	
	elf_header_t _kernelElf;
	memory_region_t _kernelMemRegions[MAX_MEM_REGIONS];

	vbe_t _vbe;
	frame_buffer_info_t _frameBuffer;
};