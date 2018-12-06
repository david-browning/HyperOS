#pragma once
#include "annotations.h"

enum ELF_ABIS : uint8_t
{
	SYSTEM_V = 0x00,
	HP_UX = 0x01,
	NETBSD = 0x02,
	LINUX = 0x03,
	GNU_HURD = 0x04,
	SOLARIS = 0x06,
	AIX = 0x07,
	IRIX = 0x08,
	FREE_BSD = 0x09,
	TRU64 = 0x0A,
	NOVELL_MODESTO = 0x0B,
	OPEN_BSD = 0x0C,
	OPEN_VMS = 0x0D,
	NONSTOP_KERNEL = 0x0E,
	AROS = 0x0F,
	FENIX_OS = 0x10,
	CLOUD_ABI = 0x11,
	SORTIX = 0x53,
};

enum class ELF_TYPES : uint16_t
{
	RELOCATABLE = 1,
	EXECUTABLE = 2,
	SHARED = 3,
	CORE = 4,
};

enum class ELF_MACHINE_TYPES : uint16_t
{
	NONE = 0x00,
	SPARC = 0x02,
	X86 = 0x03,
	MIPS = 0x08,
	POWERPC = 0x14,
	ARM = 0x28,
	SUPERH = 0x2A,
	IA64 = 0x32,
	X86_64 = 0x3E,
	AARCH64 = 0xB7,
	RISC_V = 0xF3,
};

enum class ELF_SEGMENT_TYPES : uint32_t
{
	NULL_SEGMENT = 0x00000000,
	LOAD = 0x00000001,
	DYNAMIC = 0x00000002,
	INTERP = 0x00000003,
	NOTE = 0x00000004,
	SHLIB = 0x00000005,
	PHDR = 0x00000006,
	LOOS = 0x60000000,
	HIOS = 0x6FFFFFFF,
	LOPROC = 0x70000000,
	HIPROC = 0x7FFFFFFF,
};

enum class ELF_SECTION_HEADER_TYPES : uint32_t
{
	NULL_SECTION = 0x00,
	PROGRAM_DATA = 0x01,
	SYMBOL_TABLE = 0x02,
	STRING_TABLE = 0x03,
	RELOCATION_ADDENDS = 0x04,
	SYMBOL_HASH_TABLE = 0x05,
	DYNAMIC_LINKING = 0x06,
	NOTES = 0x07,
	BSS = 0x08,
	RELOCATION_NO_ADDENDS = 0x09,
	RESERVED = 0x0A,
	DYNAMIC_LINKER_SYMBOL_TABLE = 0x0B,

	CONSTRUCTOR_ARRAY = 0x0E,
	DESTRUCTOR_ARRAY = 0x0F,
	PRECONSTRUCTOR_ARRAY = 0x10,
	SECTION_GROUP = 0x11,
	EXT_SECTION_INDX = 0x12,
	NUM_DEFINED_TYPES = 0x13,
	
	START_OS_SPEC = 0x60000000,
};

enum class ELF_SECTION_ATTRIBUTES : uint32_t
{
	WRITABLE = 0x01,
	ALLOCATE = 0x02,
	EXECUTABLE = 0x04,
	MERGE = 0x10,
	STRINGS = 0x20,
	INFO_LINK = 0x40,
	LINK_ORDER = 0x80,
	OS_NONCONFORMING = 0x100,
	GROUP = 0x200,
	THREAD_LOCAL_DATA = 0x400,
	OS_SPEC = 0x0ff00000,
	PROCESSOR_SPEC = 0xf0000000,
	ORDERING_REQ = 0x4000000,
	EXCLUDE = 0x8000000,
};

struct elf_header_t
{
	uint32_t Magic;

	//Set to 1 to indicate 32 bit. 2 indicates 64 bit format.
	uint8_t Class;

	//1 indicates little endian. 2 indicates big endian. 
	uint8_t Endianess;

	uint8_t Version1;

	ELF_ABIS OSABI;

	uint8_t ABIVersion;
	uint8_t Unused1;
	uint8_t Unused2;
	uint8_t Unused3;
	uint8_t Unused4;
	uint8_t Unused5;
	uint8_t Unused6;
	uint8_t Unused7;

	ELF_TYPES Type;

	ELF_MACHINE_TYPES Machine;

	uint32_t Version2;

	//This is the memory address of the entry point from where the process starts executing.
	uintptr_t Entry;

	//Points to the start of the program header table.
	uintptr_t ProgramHeaderTableOffset;

	//Points to the start of the section header table.
	uintptr_t SectionHeaderTableOffset;

	//Interpretation of this field depends on the target architecture.
	uint32_t Flags;

	//Size of this header. 64 or 32 bytes.
	uint16_t HeaderSize;

	//Contains the size of a program header table entry.
	uint16_t ProgramHeaderTableEntrySize;

	//Contains the number of entries in the program header table.
	uint16_t NumProgramHeaderEntries;

	//Contains the size of a section header table entry.
	uint16_t SectionHeaderTableEntrySize;

	//Contains the number of entries in the section header table.
	uint16_t NumSectionHeaderEntries;

	//Contains index of the section header table entry that contains the section names.
	uint16_t SectionNamesIndex;
};

struct elf_program_header_t
{
	ELF_SEGMENT_TYPES Type;

	#if defined __x86_64__ || defined __x86_64 || defined _M_X64 || defined _M_IA64 || defined _IA64
	uint32_t Flags;
	#endif

	uintptr_t SegmentOffset;
	uintptr_t SegmentVirtAddress;
	uintptr_t SegmentPhysicalAddress;
	size_t SegmentSizeBytes;
	size_t SegmentMemSizeBytes;
	
	#if !defined __x86_64__ && !defined __x86_64 && !defined _M_X64 && !defined _M_IA64 && !defined _IA64
	uint32_t Flags;
	#endif

	size_t Alignment;
};

struct elf_section_header_t
{
	uint32_t NameOffset;
	ELF_SECTION_HEADER_TYPES Type;
	uint64_t Flags;
	uintptr_t SectionVirtAddress;
	size_t SectionOffset;
	size_t SectionSizeBytes;
	uint32_t SectionIndex;
	uint32_t Info;
	size_t SectionAlignment;
	size_t EntrySizeBytes;
};

inline constexpr bool ELFHasCorrectMagicNumber(const elf_header_t* header_p)
{
	return header_p->Magic == (header_p->Endianess == 1 ? 0x464C457F : 0x7F454C46);
}

inline constexpr bool ELFIsx86_64(const elf_header_t* header_p)
{
	return header_p->Machine == ELF_MACHINE_TYPES::IA64 || 
		header_p->Machine == ELF_MACHINE_TYPES::X86_64;
}

