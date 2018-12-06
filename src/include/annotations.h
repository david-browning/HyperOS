#pragma once

#ifdef __cplusplus

using namespace std;

#include <cstddef>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#else

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#endif

extern "C" void __cxa_pure_virtual();

#ifdef KERNEL_INLINE

#define INLINE_K __forceinline

#else

#define INLINE_K

#endif


#if !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)

#undef INLINE_K
#define INLINE_K

#ifndef _DEBUG
#define _DEBUG
#endif

#ifndef DEBUG
#define DEBUG
#endif

#endif

typedef uint64_t handle_t;

static constexpr size_t GUID_NUM_BYTES = 16;

typedef uint8_t guid_t[GUID_NUM_BYTES];

//Defines a function that only the kernel can run.
#define KERNEL_FUNC

#define LIB_EXPORT __attribute__((__visibility__("default")))

typedef int64_t result_t;
typedef uint32_t facility_result_t;
typedef uint32_t code_result_t;

inline constexpr result_t MakeResult(const bool failed,
												 const facility_result_t facility,
												 const code_result_t code)
{
	return (failed ? 0x8000000000000000 : 0) |
		(((uint64_t)facility & 0x7FFFFFFF) << (sizeof(code_result_t) * uint32_t(8))) |
		code;
}

//Returns true if the result indicates a failure.
inline constexpr bool ResultFailed(const result_t r)
{
	return r < 0;
}

//Returns the error code from the result.
inline code_result_t GetCodeFromResult(const result_t r)
{
	return (code_result_t)(r & 0x00000000FFFFFFFF);
}

//Returns the facility code from the result.
inline facility_result_t GetFacilityFromResult(const result_t r)
{
	return (facility_result_t)((r >> sizeof(code_result_t) * 8) & 0x00007FFFFFFF);
}

static constexpr facility_result_t FACILITY_NONE = 0x0000;

static constexpr facility_result_t FACILITY_DEVICE = 0x0001;
static constexpr facility_result_t FACILITY_KEYBOARD = 0x003;

static constexpr facility_result_t FACILITY_CPU = 0x0002;
static constexpr facility_result_t FACILITY_MEMORY = 0x0004;
static constexpr facility_result_t FACILITY_DISK = 0x0006;
static constexpr facility_result_t FACILITY_NETWORK = 0x0008;
static constexpr facility_result_t FACILITY_FILE = 0x0010;

static constexpr facility_result_t FACILITY_PROCESS = 0x0020;
static constexpr facility_result_t FACILITY_LIBRARY = 0x0040;
static constexpr facility_result_t FACILITY_THREAD = 0x0080;
static constexpr facility_result_t FACILITY_INTERRUPT = 0x0090;
static constexpr facility_result_t FACILITY_KERNEL = 0x00A0;

static constexpr code_result_t CODE_NONE = 0x0000;
static constexpr code_result_t CODE_UNSUPPORTED = 0x0001;
static constexpr code_result_t CODE_UNKNOWN = 0x0002;
static constexpr code_result_t CODE_OUT_OF_MEM = 0x0003;
static constexpr code_result_t CODE_NOT_EQUAL = 0x0004;
static constexpr code_result_t CODE_NOT_FOUND = 0x0005;
static constexpr code_result_t CODE_INVALID = 0x0006;
static constexpr code_result_t CODE_FULL = 0x0007;
static constexpr code_result_t CODE_READ_ERROR = 0x0008;
static constexpr code_result_t CODE_WRITE_ERROR = 0x0009;
static constexpr code_result_t CODE_NOT_AVAILABLE = 0x000A;
static constexpr code_result_t CODE_NOT_ENOUGH = 0x000B;
static constexpr code_result_t CODE_TEST_FAILED = 0x000C;
static constexpr code_result_t CODE_RESET_FAILED = 0x000D;
static constexpr code_result_t CODE_TIME_OUT = 0x000E;
static constexpr code_result_t CODE_NOT_IMPLEMENTED = 0x000F;
static constexpr code_result_t CODE_NOT_INITIALIZED = 0x0010;
static constexpr code_result_t CODE_OUT_OF_BOUNDS = 0x00A0;

static constexpr result_t RES_SUCCESS = MakeResult(false, FACILITY_NONE, CODE_NONE);

//This is not an error code. It states that the action cannot be completed right now.
static constexpr result_t RES_NOT_NOW = MakeResult(false, FACILITY_NONE, CODE_INVALID);
static constexpr result_t RES_NOT_IMPLEMENTED = MakeResult(true, FACILITY_NONE, CODE_NOT_IMPLEMENTED);

static constexpr result_t RES_UNSUPPORTED = MakeResult(true, FACILITY_NONE, CODE_UNSUPPORTED);
static constexpr result_t RES_OUT_OF_MEMORY = MakeResult(true, FACILITY_MEMORY, CODE_OUT_OF_MEM);
static constexpr result_t RES_NOT_FOUND = MakeResult(true, FACILITY_MEMORY, CODE_NOT_FOUND);
static constexpr result_t RES_INVALID_ARG = MakeResult(true, FACILITY_LIBRARY, CODE_UNSUPPORTED);
static constexpr result_t RES_TOO_SMALL = MakeResult(true, FACILITY_MEMORY, CODE_NOT_ENOUGH);

static constexpr result_t RES_READ_ERROR = MakeResult(true, FACILITY_FILE, CODE_READ_ERROR);
static constexpr result_t RES_WRITE_ERROR = MakeResult(true, FACILITY_FILE, CODE_WRITE_ERROR);
static constexpr result_t RES_FILE_NOT_FOUND = MakeResult(true, FACILITY_FILE, CODE_NOT_FOUND);
static constexpr result_t RES_FILE_SEEK_INVALID = MakeResult(true, FACILITY_FILE, CODE_OUT_OF_BOUNDS);

static constexpr result_t RES_ARRAY_FULL = MakeResult(true, FACILITY_MEMORY, CODE_FULL);
static constexpr result_t RES_NOT_AVAILABLE = MakeResult(true, FACILITY_MEMORY, CODE_NOT_AVAILABLE);

static constexpr result_t RES_PAGE_NOT_ALLOCATED = MakeResult(true, FACILITY_MEMORY, CODE_NOT_INITIALIZED);

static constexpr result_t RES_UNKNOWN_LIB_ERROR = MakeResult(true, FACILITY_LIBRARY, CODE_UNKNOWN);
static constexpr result_t RES_UNKNOWN_DEVICE_ERROR = MakeResult(true, FACILITY_DEVICE, CODE_UNKNOWN);
static constexpr result_t RES_UNKNOWN_NETWORK_ERROR = MakeResult(true, FACILITY_NETWORK, CODE_UNKNOWN);
static constexpr result_t RES_UNKNOWN_FILE_ERROR = MakeResult(true, FACILITY_FILE, CODE_UNKNOWN);
static constexpr result_t RES_UNKNOWN_THREAD_ERROR = MakeResult(true, FACILITY_THREAD, CODE_UNKNOWN);


#define _DEV_SET_
#define _DEV_DO_NOT_SET_

//Indicates a design by contract function or parameter. The design by contract target does no 
//error checking on its parameters. It is the caller's responsibility to make sure the passed
//parameters obey the contract.
#define _DBC_

//The variable is an input parameter.
#define _IN_

//The variable is an input parameter. It can be a null pointer.
#define _IN_CAN_BE_NULL_

//The variable is an optional input parameter.
#define _IN_OPT_

//The variable is a size_t used as an input parameter.
#define _IN_SIZE_
//The variable is  asize_t array used as an input parameter.
#define _IN_SIZE_ARR_

//The variable is an enumeration used as an input parameter.
#define _IN_ENUM_
//The variable is an enumeration array used as an input parameter.
#define _IN_ENUM_ARR_
//The variable is an enumeration array with a zero at the end and is used as an input parameter.
#define _IN_ENUM_ARR_Z_

//The variable is an array used as an input parameter.
#define _IN_ARR_
//The variable is a zero terminated array used as an input parameter.
#define _IN_ARR_Z_

//The variable is a bool used as an input parameter.
#define _IN_BOOL_
//The variable is an array of bools used as an input parameter.
#define _IN_BOOL_ARR_
//The variable is a zero terminated bool array used as an input parameter.
#define _IN_BOOL_ARR_Z_

//The variable is a byte used as an input parameter.
#define _IN_BYTE_
//The variable is an array of bytes used as an input parameter.
#define _IN_BYTE_ARR_
//The variable is a zero terminated byte array used as an input parameter.
#define _IN_BYTE_ARR_Z_

//The variable is a char used as an input parameter.
#define _IN_CHAR_
//The variable is an array of characters used as an input parameter.
#define _IN_CHAR_ARR_
//The variable is a zero terminated char array used as an input parameter.
#define _IN_CHAR_ARR_Z_

//The variable is a short used as an input parameter.
#define _IN_SHORT_
//The variable is an array of shorts used as an input parameter.
#define _IN_SHORT_ARR_
//The variable is a zero terminated short array used as an input parameter.
#define _IN_SHORT_ARR_Z_

//The variable is a wchar_t used as an input parameter.
#define _IN_WCHAR_
//The variable is an array of wide characters used as an input parameter.
#define _IN_WCHAR_ARR_
//The variable is a zero terminated wchar_t array used as an input parameter.
#define _IN_WCHAR_ARR_Z_

//The variable is a wide character string used as an input parameter.
#define _IN_WSTR_
//The variable is a zero terminated wide character string used as an input parameter.
#define _IN_WSTR_Z_
//The variable is an array of zero terminated wide character strings used as an input parameter.
#define _IN_WSTR_Z_POINTER
//The variable is an array of wide character strings used as an input parameter.
#define _IN_WSTR_ARR_
//The variable is a zero terminated wchar_t* array used as an input parameter.
#define _IN_WSTR_ARR_Z_

//The variable is an int used as an input parameter.
#define _IN_INT_
//The variable is an array of ints used as an input parameter.
#define _IN_INT_ARR_
//The variable is a zero terminated int array used as an input parameter.
#define _IN_INT_ARR_Z_

//The variable is a float used as an input parameter.
#define _IN_FLOAT_
//The variable is an array of floats used as an input parameter.
#define _IN_FLOAT_ARR_
//The variable is a zero terminated float array used as an input parameter.
#define _IN_FLOAT_ARR_Z_

//The variable is a long used as an input parameter.
#define _IN_LONG_
//The variable is an array of longs used as an input parameter.
#define _IN_LONG_ARR_
//The variable is a zero terminated long array used as an input parameter.
#define _IN_LONG_ARR_Z_

//The variable is a double used as an input parameter.
#define _IN_DOUBLE_
//The variable is an array of doubles used as an input parameter.
#define _IN_DOUBLE_ARR_
//The variable is a zero terminated double array used as an input parameter.
#define _IN_DOUBLE_ARR_Z_

//The variable is a string used as an input parameter.
#define _IN_STR_
//The variable is a zero terminated string used as an input parameter.
#define _IN_STR_Z_
//The variable is an array of strings used as an input parameter.
#define _IN_STR_ARR_
//The variable is a zero terminated string array used as an input parameter.
#define _IN_STR_ARR_Z_

//The variable is a pointer to a variable used as an input parameter.
#define _IN_POINTER_
//The variable is an array of pointers used as an input parameter.
#define _IN_POINTER_ARR_
//The variable is a zero terminated pointer array used as an input parameter.
#define _IN_POINTER_ARR_Z_

//The variable is a callback used as an input parameter.
#define _IN_CALLBACK_
//The variable is an array of callbacks used as an input parameter.
#define _IN_CALLBACK_ARR_
//The variable is a zero terminated function pointer array used as an input parameter.
#define _IN_CALLBACK_ARR_Z_

//The variable is a function used as an input parameter.
#define _IN_FUNCTION_
//The variable is an array of functions used as an input parameter.
#define _IN_FUNCTION_ARR_
//The variable is a zero terminated function pointer array used as an input parameter.
#define _IN_FUNCTION_ARR_Z_

#define _IN_HANDLE_


//The variable is an output parameter.
#define _OUT_

//The variable is an output parameter. It can be a null pointer.
#define _OUT_CAN_BE_NULL_

//The variable is an optional output parameter.
#define _OUT_OPTIONAL_

//The variable is a size_t used as an output parameter.
#define _OUT_SIZE_
//The variable is  asize_t array used as an output parameter.
#define _OUT_SIZE_ARR_

//The variable is an enumeration used as an output parameter.
#define _OUT_ENUM_
//The variable is an enumeration array used as an output parameter.
#define _OUT_ENUM_ARR_
//The variable is an enumeration array with a zero at the end and is used as an output parameter.
#define _OUT_ENUM_ARR_Z_

//The variable is an array used as an output parameter.
#define _OUT_ARR_
//The variable is a zero terminated array used as an output parameter.
#define _OUT_ARR_Z_

//The variable is a bool used as an output parameter.
#define _OUT_BOOL_
//The variable is an array of bools used as an output parameter.
#define _OUT_BOOL_ARR_
//The variable is a zero terminated bool array used as an output parameter.
#define _OUT_BOOL_ARR_Z_

//The variable is a byte used as an output parameter.
#define _OUT_BYTE_
//The variable is an array of bytes used as an output parameter.
#define _OUT_BYTE_ARR_
//The variable is a zero terminated byte array used as an output parameter.
#define _OUT_BYTE_ARR_Z_

//The variable is a char used as an output parameter.
#define _OUT_CHAR_
//The variable is an array of characters used as an output parameter.
#define _OUT_CHAR_ARR_
//The variable is a zero terminated char array used as an output parameter.
#define _OUT_CHAR_ARR_Z_

//The variable is a short used as an output parameter.
#define _OUT_SHORT_
//The variable is an array of shorts used as an output parameter.
#define _OUT_SHORT_ARR_
//The variable is a zero terminated short array used as an output parameter.
#define _OUT_SHORT_ARR_Z_

//The variable is a wchar_t used as an output parameter.
#define _OUT_WCHAR_
//The variable is an array of wide characters used as an output parameter.
#define _OUT_WCHAR_ARR_
//The variable is a zero terminated wchar_t array used as an output parameter.
#define _OUT_WCHAR_ARR_Z_

//The variable is a wide character string used as an output parameter.
#define _OUT_WSTR_
//The variable is a zero terminated wide character string used as an output parameter.
#define _OUT_WSTR_Z_
//The variable is a pointer to wide character zero terminated string. 
#define _OUT_WSTR_Z_POINTER_
//The variable is an array of wide character strings used as an output parameter.
#define _OUT_WSTR_ARR_
//The variable is a zero terminated wchar_t* array used as an output parameter.
#define _OUT_WSTR_ARR_Z_

//The variable is an int used as an output parameter.
#define _OUT_INT_
//The variable is an array of ints used as an output parameter.
#define _OUT_INT_ARR_
//The variable is a zero terminated int array used as an output parameter.
#define _OUT_INT_ARR_Z_

//The variable is a float used as an output parameter.
#define _OUT_FLOAT_
//The variable is an array of floats used as an output parameter.
#define _OUT_FLOAT_ARR_
//The variable is a zero terminated float array used as an output parameter.
#define _OUT_FLOAT_ARR_Z_

//The variable is a long used as an output parameter.
#define _OUT_LONG_
//The variable is an array of longs used as an output parameter.
#define _OUT_LONG_ARR_
//The variable is a zero terminated long array used as an output parameter.
#define _OUT_LONG_ARR_Z_

//The variable is a double used as an output parameter.
#define _OUT_DOUBLE_
//The variable is an array of doubles used as an output parameter.
#define _OUT_DOUBLE_ARR_
//The variable is a zero terminated double array used as an output parameter.
#define _OUT_DOUBLE_ARR_Z_

//The variable is a string used as an output parameter.
#define _OUT_STR_
//The variable is a zero terminated string used as an output parameter.
#define _OUT_STR_Z_
//The variable is an array of strings used as an output parameter.
#define _OUT_STR_ARR_
//The variable is a zero terminated string array used as an output parameter.
#define _OUT_STR_ARR_Z_

//The variable is a pointer to a variable used as an output parameter.
#define _OUT_POINTER_
//The variable is an array of pointers used as an output parameter.
#define _OUT_POINTER_ARR_
//The variable is a zero terminated pointer array used as an output parameter.
#define _OUT_POINTER_ARR_Z_

//The variable is a callback used as an output parameter.
#define _OUT_CALLBACK_
//The variable is an array of callbacks used as an output parameter.
#define _OUT_CALLBACK_ARR_
//The variable is a zero terminated function pointer array used as an output parameter.
#define _OUT_CALLBACK_ARR_Z_

//The variable is a function used as an output parameter.
#define _OUT_FUNCTION_
//The variable is an array of functions used as an output parameter.
#define _OUT_FUNCTION_ARR_
//The variable is a zero terminated function pointer array used as an output parameter.
#define _OUT_FUNCTION_ARR_Z_

#define _OUT_HANDLE_


//The variable is an input and output parameter.
#define _INOUT_

//The variable is an input and output parameter. It can be a null pointer.
#define _INOUT_CAN_BE_NULL_

//The variable is an optional input and output parameter.
#define _INOUT_OPTIONAL_

//The variable is a size_t used as an input and output parameter.
#define _INOUT_SIZE_
//The variable is  asize_t array used as an input and output parameter.
#define _INOUT_SIZE_ARR_

//The variable is an enumeration used as an input and output parameter.
#define _INOUT_ENUM_
//The variable is an enumeration array used as an input and output parameter.
#define _INOUT_ENUM_ARR_
//The variable is an enumeration array with a zero at the end and is used as an input and output parameter.
#define _INOUT_ENUM_ARR_Z_

//The variable is an array used as an input and output parameter.
#define _INOUT_ARR_
//The variable is a zero terminated array used as an input and output parameter.
#define _INOUT_ARR_Z_

//The variable is a bool used as an input and output parameter.
#define _INOUT_BOOL_
//The variable is an array of bools used as an input and output parameter.
#define _INOUT_BOOL_ARR_
//The variable is a zero terminated bool array used as an input and output parameter.
#define _INOUT_BOOL_ARR_Z_

//The variable is a byte used as an input and output parameter.
#define _INOUT_BYTE_
//The variable is an array of bytes used as an input and output parameter.
#define _INOUT_BYTE_ARR_
//The variable is a zero terminated byte array used as an input and output parameter.
#define _INOUT_BYTE_ARR_Z_

//The variable is a char used as an input and output parameter.
#define _INOUT_CHAR_
//The variable is an array of characters used as an input and output parameter.
#define _INOUT_CHAR_ARR_
//The variable is a zero terminated char array used as an input and output parameter.
#define _INOUT_CHAR_ARR_Z_

//The variable is a short used as an input and output parameter.
#define _INOUT_SHORT_
//The variable is an array of shorts used as an input and output parameter.
#define _INOUT_SHORT_ARR_
//The variable is a zero terminated short array used as an input and output parameter.
#define _INOUT_SHORT_ARR_Z_

//The variable is a wchar_t used as an input and output parameter.
#define _INOUT_WCHAR_
//The variable is an array of wide characters used as an input and output parameter.
#define _INOUT_WCHAR_ARR_
//The variable is a zero terminated wchar_t array used as an input and output parameter.
#define _INOUT_WCHAR_ARR_Z_

//The variable is a wide character string used as an input and output parameter.
#define _INOUT_WSTR_
//The variable is a zero terminated wide character string used as an input and output parameter.
#define _INOUT_WSTR_Z_
//The variable is an array of zero terminated wide character strings used as an input and output parameter.
#define _INOUT_WSTR_Z_POINTER
//The variable is an array of wide character strings used as an input and output parameter.
#define _INOUT_WSTR_ARR_
//The variable is a zero terminated wchar_t* array used as an input and output parameter.
#define _INOUT_WSTR_ARR_Z_

//The variable is an int used as an input and output parameter.
#define _INOUT_INT_
//The variable is an array of ints used as an input and output parameter.
#define _INOUT_INT_ARR_
//The variable is a zero terminated int array used as an input and output parameter.
#define _INOUT_INT_ARR_Z_

//The variable is a float used as an input and output parameter.
#define _INOUT_FLOAT_
//The variable is an array of floats used as an input and output parameter.
#define _INOUT_FLOAT_ARR_
//The variable is a zero terminated float array used as an input and output parameter.
#define _INOUT_FLOAT_ARR_Z_

//The variable is a long used as an input and output parameter.
#define _INOUT_LONG_
//The variable is an array of longs used as an input and output parameter.
#define _INOUT_LONG_ARR_
//The variable is a zero terminated long array used as an input and output parameter.
#define _INOUT_LONG_ARR_Z_

//The variable is a double used as an input and output parameter.
#define _INOUT_DOUBLE_
//The variable is an array of doubles used as an input and output parameter.
#define _INOUT_DOUBLE_ARR_
//The variable is a zero terminated double array used as an input and output parameter.
#define _INOUT_DOUBLE_ARR_Z_

//The variable is a string used as an input and output parameter.
#define _INOUT_STR_
//The variable is a zero terminated string used as an input and output parameter.
#define _INOUT_STR_Z_
//The variable is an array of strings used as an input and output parameter.
#define _INOUT_STR_ARR_
//The variable is a zero terminated string array used as an input and output parameter.
#define _INOUT_STR_ARR_Z_

//The variable is a pointer to a variable used as an input and output parameter.
#define _INOUT_POINTER_
//The variable is an array of pointers used as an input and output parameter.
#define _INOUT_POINTER_ARR_
//The variable is a zero terminated pointer array used as an input and output parameter.
#define _INOUT_POINTER_ARR_Z_

//The variable is a callback used as an input and output parameter.
#define _INOUT_CALLBACK_
//The variable is an array of callbacks used as an input and output parameter.
#define _INOUT_CALLBACK_ARR_
//The variable is a zero terminated function pointer array used as an input and output parameter.
#define _INOUT_CALLBACK_ARR_Z_

//The variable is a function used as an input and output parameter.
#define _INOUT_FUNCTION_
//The variable is an array of functions used as an input and output parameter.
#define _INOUT_FUNCTION_ARR_
//The variable is a zero terminated function pointer array used as an input and output parameter.
#define _INOUT_FUNCTION_ARR_Z_


template<typename T>
INLINE_K constexpr T RoundUp(_IN_ const T val, _IN_ const T nearest)
{
	T remainder = val % nearest;
	return remainder == 0 ? val : val + nearest - remainder;
}

template<typename T>
inline constexpr T GetMin(T value)
{
	return value;
}

template<typename T, typename... Tlist>
inline constexpr T GetMin(T value, Tlist... list)
{
	T min = GetMin<T>(list...);
	return value < min ? value : min;
}