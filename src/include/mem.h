#pragma once
#include "annotations.h"
#include "kernel_debug.h"

enum class MEMORY_TYPES : uint32_t
{
	RESERVED = 0,
	RAM = 1,
	ACPI = 3,
	PRESERVED = 4,
};

struct memory_region_t
{
	uintptr_t BaseAddress;
	uintptr_t HighAddress;
	uint32_t Reserved;
	MEMORY_TYPES Type;
};

inline constexpr uintptr_t MemAlignAddress(const uintptr_t addr,
														 const size_t alignment)
{
	return (addr + alignment - 1) & ~(alignment - 1);
}

//Stacks are 2MB in size.
static constexpr size_t STACK_ALIGNMENT = 2 * 1024 * 1024;



template<typename T>
LIB_EXPORT INLINE_K void MemoryReplace(_IN_ARR_Z_ T* ptr,
													_IN_ const T toReplace,
													_IN_ const T with,
													_IN_SIZE_ size_t numElements)
{
	if (numElements > 0 && ptr != nullptr)
	{
		for (int i = 0; i < numElements; i++)
		{
			if (ptr[i] == toReplace)
			{
				ptr[i] = with;
			}
		}
	}
}

template<typename T>
LIB_EXPORT INLINE_K void MemorySet(_IN_POINTER_ T* ptr,
											  _IN_ T val,
											  _IN_SIZE_ size_t numElements)
{
	if (numElements > 0 && ptr != nullptr)
	{
		numElements--;
		while (numElements--)
		{
			ptr[numElements] = val;
		}
	}
}

/*
Copies the "numElements" number of elements of type "T" from "src" to "dest".
The number of bytes copied is numElements * sizeof(T)
Does nothing if either pointer is nullptr.
*/
template<typename T>
LIB_EXPORT INLINE_K void MemoryCopy(_IN_POINTER_ T* dest,
												_IN_POINTER_ const T* src,
												_IN_SIZE_ size_t numElements)
{
	if (numElements != 0 && dest != nullptr && src != nullptr)
	{
		for (int i = 0; i < numElements; i++)
		{
			dest[i] = src[i];
		}
	}
}

/*
Copies memory from dest to src until terminator is found in src.
*/
template<typename T>
LIB_EXPORT INLINE_K void MemoryCopyUntil(_IN_POINTER_ T* dest,
													  _IN_POINTER_ const T* src,
													  _IN_SIZE_ T terminator)
{
	if (dest == nullptr || src == nullptr)
	{
		return;
	}

	int i = 0;
	while (src[i] != terminator)
	{
		dest[i] = src[i];
		i++;
	}
}

/*
Searches an array of "numElements" elements of type T until it finds an element equal to "val".
Returns a pointer to the first occurrence of "val" in the block of memory pointed to by "ptr".
Returns nullptr if the value is not found.
*/
template<typename T>
LIB_EXPORT INLINE_K T* MemoryFind(_IN_POINTER_ const T* ptr,
											 _IN_ T val,
											 _IN_SIZE_ size_t numElements)
{
	for (int i = 0; i < numElements; i++)
	{
		if (ptr[i] == val)
		{
			return ptr + i;
		}
	}
}

/*
Compares each element in p1 to p2 until an element is not equal.
Returns 0 if the blocks of memory each pointer points to are equal.
Returns -1 if the p1 element is less than p2's element.
Returns 1 if the p1 element is greater than p2's element.
Returns -2 is one of any of the arguments are invalid.
*/
template<typename T>
LIB_EXPORT INLINE_K int MemoryCompare(_IN_POINTER_ const T* p1,
												  _IN_POINTER_ const T* p2,
												  _IN_SIZE_ const size_t numElements)
{
	if (p1 == nullptr || p2 == nullptr || numElements == 0)
	{
		return -2;
	}

	for (int i = 0; i < numElements; i++)
	{
		if (p1[i] < p2[i])
		{
			return -1;
		}
		else if (p1[i] > p2[i])
		{
			return 1;
		}
	}

	return 0;
}

/*
Counts the number of elements until an element of value 0 is found.
The returned count does not include the null terminator.
*/
template<typename T>
LIB_EXPORT INLINE_K constexpr size_t MemoryLength(_IN_ARR_Z_ const T* str,
																  _IN_OPT_ const T terminator = 0,
																  _IN_OPT_ const size_t i = 0)
{
	return (str && str[i] != terminator) ? MemoryLength(str, terminator, i + 1) + 1 : 0;
}

template<typename T>
LIB_EXPORT INLINE_K void MemoryReverse(_IN_ARR_ T* mem,
													_IN_SIZE_ const size_t memLen)
{
	size_t i = 0;
	size_t j = memLen - 1;
	for (; i < j; i++, j--)
	{
		T temp = mem[i];
		mem[i] = mem[j];
		mem[j] = temp;
	}
}


static constexpr uint8_t PAGE_OFFSET_BITS = 12;
static constexpr uint8_t PAGE_TABLE_BITS = 9;
static constexpr uint8_t PAGE_DIRECTORY_BITS = 9;
static constexpr uint8_t PAGE_DIRECTORY_POINTER_BITS = 9;
static constexpr uint8_t PAGE_MAP_BITS = 9;
static constexpr uint64_t BASE_ADDRESS_MASK = 0xF'FFFF'FFFF'F000;


inline constexpr uint16_t GetL1Offset(const uint64_t vaddr)
{
	return (vaddr >> 12) & 0x1FF;
}

inline constexpr uint16_t GetPageOffset(const uint64_t vaddr)
{
	return vaddr & 0xFFF;
}

inline constexpr uint16_t GetL2Offset(const uint64_t vaddr)
{
	return (vaddr >> 21) & 0x1FF;
}

inline constexpr uint16_t GetL3Offset(const uint64_t vaddr)
{
	return (vaddr >> 30) & 0x1FF;
}

inline constexpr uint16_t GetL4Offset(const uint64_t vaddr)
{
	return (vaddr >> 39) & 0x1FF;
}

inline constexpr uint64_t* GetL1EntryPtr(const uint64_t cr3, const uintptr_t vaddr)
{
	uint64_t* l4_p = (uint64_t*)(cr3 & 0xFFFFFFFFFF000);
	uint64_t* l4Entry_p = l4_p + GetL4Offset(vaddr);

	if (l4Entry_p == nullptr)
	{
		WaitForDebugger();
	}

	uint64_t* l3_p = (uint64_t*)(*l4Entry_p & 0xFFFFFFFFFF000);
	uint64_t* l3Entry_p = l3_p + GetL3Offset(vaddr);
	if (l3Entry_p == nullptr)
	{
		WaitForDebugger();
	}

	uint64_t* l2_p = (uint64_t*)(*l3Entry_p & 0xFFFFFFFFFF000);
	uint64_t* l2Entry_p = l2_p + GetL2Offset(vaddr);
	if (l2Entry_p == nullptr)
	{
		WaitForDebugger();
	}

	uint64_t* l1_p = (uint64_t*)(*l2Entry_p & 0xFFFFFFFFFF000);
	uint64_t* l1Entry_p = l1_p + GetL1Offset(vaddr);
	if (l1Entry_p == nullptr)
	{
		WaitForDebugger();
	}

	return l1Entry_p;
}

extern "C"
static uintptr_t GetRealAddress(uintptr_t vaddr);