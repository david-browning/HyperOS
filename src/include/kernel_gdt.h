#pragma once

#include "annotations.h"
#include "ikernel_component.h"

//Maximum number of entries in a GDT.
static constexpr size_t MAX_GDT_ENTRIES = 8;

typedef uint16_t targetselector_t;

struct gdtdescriptor64_t
{
	uint16_t SegLimitLow;
	uint16_t BaseAddrLow;
	uint8_t BaseAddrMid;
	uint16_t Flags;
	uint8_t BaseAddrHigh;
}
__attribute__((packed));

struct gdtdescriptor128_t
{
	uint16_t SegLimitLow;
	uint16_t BaseAddrLow;
	uint8_t BaseAddrMid;
	uint16_t Flags;
	uint8_t BaseAddrHigh;

	uint32_t BaseAddrHighHigh;
	uint32_t ReservedIGN;
}
__attribute__((packed));

enum class PRIVILAGE_LEVELS : uint8_t
{
	KERNEL = 0,
	KERNEL_DRIVER = 1,
	ABOVE_USER = 2,
	USER = 3,
};

inline constexpr void GDTMakeEntry(_IN_INT_ const uint32_t baseAddress,
											  _IN_INT_ const uint32_t segmentLimit,
											  _IN_ENUM_ const PRIVILAGE_LEVELS priv,
											  _IN_BOOL_ const bool isCode,
											  _IN_BOOL_ const bool isWritable,
											  _OUT_POINTER_ gdtdescriptor64_t* desc_p)
{

	uint8_t segmentHigh = uint8_t(segmentLimit >> 8 & 0x0F);
	desc_p->SegLimitLow = uint16_t(segmentLimit & 0xFFFF);
	desc_p->BaseAddrLow = uint16_t(baseAddress & 0xFFFF);
	desc_p->BaseAddrMid = uint8_t(baseAddress >> 16 & 0xFF);
	desc_p->BaseAddrHigh = uint8_t(baseAddress >> 24 & 0xFF);
	const uint8_t GR = 0;
	const uint8_t DC = isCode ? uint8_t(0) : uint8_t(1);
	desc_p->Flags = uint16_t(GR << 15 | 0 << 14 | 1 << 13 | 0 << 12 | segmentHigh << 8 |
									 1 << 7 | (uint8_t)priv << 5 | 1 << 4 | (isCode ? 1 << 3 : 0) | DC << 2 | (isWritable ? 2 : 0) | 0);
}

inline constexpr void GDTMakeEntry(_IN_INT_ const uint64_t baseAddress,
											  _IN_INT_ const uint32_t segmentLimit,
											  _IN_ENUM_ const PRIVILAGE_LEVELS priv,
											  _IN_BOOL_ const bool isCode,
											  _IN_BOOL_ const bool isWritable,
											  _OUT_POINTER_ gdtdescriptor128_t* desc_p)
{
	uint8_t segmentHigh = uint8_t((segmentLimit >> 8) & 0x0F);
	desc_p->SegLimitLow = uint16_t(segmentLimit & 0xFFFF);
	desc_p->BaseAddrLow = uint16_t(baseAddress & 0xFFFF);
	desc_p->BaseAddrMid = uint8_t(baseAddress >> 16 & 0xFF);
	desc_p->BaseAddrHigh = uint8_t((baseAddress >> 24) & 0xFF);
	const uint8_t GR = 0;
	const uint8_t DC = isCode ? uint8_t(0) : uint8_t(1);
	desc_p->Flags = uint16_t(0b0000'0000'1000'1001 | uint16_t(segmentHigh << 8));

	desc_p->ReservedIGN = 0;
	desc_p->BaseAddrHighHigh = baseAddress >> 32 & 0xFFFFFFFF;

}

static constexpr targetselector_t GLOBAL_DESCRIPTOR_TABLE_NULL_INDEX = 0;
static constexpr targetselector_t GLOBAL_DESCRIPTOR_TABLE_KERNEL_CODE_INDEX = 0x8;
static constexpr targetselector_t GLOBAL_DESCRIPTOR_TABLE_TSS = 0x10;

static constexpr gdtdescriptor64_t GDT_NULL_ENTRY = gdtdescriptor64_t{ 0, 0, 0, 0, 0 };

/*
 Manages global descriptor tables.
 Only one instance of the class can exist at once so do not instantiate it.
 The kernel keeps the 1 static copy of it and it can be retrieved by GDTGetGDTManagerPtr().
 */
class GDTManager : public IKernelComponent
{
	public:
	KERNEL_FUNC GDTManager();

	KERNEL_FUNC result_t Initialize();

	KERNEL_FUNC result_t AllocateGDT(_OUT_HANDLE_ handle_t* handle_p);

	inline KERNEL_FUNC result_t AddEntry(_IN_ gdtdescriptor64_t* entry_p,
													 _IN_HANDLE_ handle_t handle,
													 _OUT_INT_ uint8_t* index_p)
	{
		return _addEntry(handle, index_p, (uint8_t*)entry_p, sizeof(gdtdescriptor64_t));
	}

	inline KERNEL_FUNC result_t AddEntry(_IN_ gdtdescriptor128_t* entry_p,
													 _IN_HANDLE_ handle_t handle,
													 _OUT_INT_ uint8_t* index_p)
	{
		return _addEntry(handle, index_p, (uint8_t*)entry_p, sizeof(gdtdescriptor128_t));
	}

	virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

	virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

	private:
	//Maximum number of GDTs that the manager can hold.
	static constexpr size_t MAX_GDT = 4;

	struct _gdt_t
	{
		gdtdescriptor64_t Table[MAX_GDT_ENTRIES];
		uint16_t NumEntries;
		uint8_t NextIndex;
	};

	result_t _addEntry(handle_t handle,
							 uint8_t* index_p,
							 uint8_t* data_p,
							 size_t dataSize);

	_gdt_t* _curGDT_p;
	_gdt_t _gdts[MAX_GDT];
	handle_t _nextHandle;
};