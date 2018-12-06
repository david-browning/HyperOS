#include "kernel_gdt.h"
#include "mem.h"

GDTManager::GDTManager()
{
	_isLoaded = false;
}

result_t GDTManager::Initialize()
{
	_curGDT_p = nullptr;
	_nextHandle = 0;
	_isLoaded = true;
	return RES_SUCCESS;
}

result_t GDTManager::AllocateGDT(_OUT_HANDLE_ handle_t* handle_p)
{
	if (_nextHandle >= MAX_GDT)
	{
		return RES_ARRAY_FULL;
	}

	//Fill the new GDT with null.
	static const gdtdescriptor64_t nullDescriptor  =
	{
		0,0,0,0,0
	};

	MemorySet<gdtdescriptor64_t>(_gdts[_nextHandle].Table, nullDescriptor, MAX_GDT_ENTRIES);
	_gdts[_nextHandle].NextIndex = 1;// sizeof(gdtdescriptor64_t);
	_gdts[_nextHandle].NumEntries = 1; //There is one entry for the null

	*handle_p = _nextHandle;
	_nextHandle++;
	return RES_SUCCESS;
}

result_t GDTManager::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
	//Check that the handle is valid.
	if (state_p->ThisHandle >= MAX_GDT)
	{
		return RES_INVALID_ARG;
	}

	_curGDT_p = &_gdts[state_p->ThisHandle];

	//Create a struct that gets passed to the CPU.
	struct _loadableGDT
	{
		uint16_t NumEntries;
		uint64_t TableAddr;
	}
	__attribute__((packed));

	_loadableGDT gdt =
	{
		_curGDT_p->NumEntries * sizeof(uint64_t),
		(uint64_t)GDTManager::_curGDT_p->Table
	};

	//Load the swapped table.
	__asm
	{
		lgdt gdt;
	}

	return RES_SUCCESS;
}

result_t GDTManager::SwapOut(_IN_POINTER_ kernel_swap_state_t * state_p)
{
	return RES_SUCCESS;
}

result_t GDTManager::_addEntry(handle_t handle,
										 uint8_t* index_p,
										 uint8_t* data_p,
										 size_t dataSize)
{
	_gdt_t* gdtToUse = &_gdts[handle];
	if (gdtToUse->NumEntries > MAX_GDT_ENTRIES)
	{
		return RES_ARRAY_FULL;
	}

	if (handle > MAX_GDT)
	{
		return RES_INVALID_ARG;
	}

	MemoryCopy<uint8_t>((uint8_t*)&gdtToUse->Table[gdtToUse->NextIndex],
							  data_p, dataSize);
	*index_p = gdtToUse->NextIndex;

	gdtToUse->NextIndex += dataSize / sizeof(gdtdescriptor64_t);
	gdtToUse->NumEntries += dataSize / sizeof(gdtdescriptor64_t);

	return RES_SUCCESS;
}
