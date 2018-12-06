#pragma once

#include <type_traits>
#include <limits>
#include "annotations.h"
#include "kernel_video.h"
#include "mem.h"
#include "drvr_vga.h"

#if defined(__cplusplus)
extern "C"
#endif
extern void PrintResult(result_t res);

/*
Walks the string looking for integers. If something besides an integer is found, the list of
integers is formatted into T and returned.
*/
template<typename T>
T StringToT(_IN_ const char* str)
{
   T val = 0;
   for (int i = 0; str[i] >= '0' && str[i] <= '9'; ++i)
   {
      val = 10 * val + (str[i] - '0');
   }

   return val;
}

template<class T>
typename std::enable_if<std::is_signed<T>::value, void>::type
TToString(_IN_ T val, _INOUT_STR_Z_ char* str)
{
   //Got this from http://www.cplusplus.com/articles/D9j2Nwbp/
   size_t strOffset = 0;
   bool isNeg = false;
   if (isNeg = (val < 0))
   {
      val = -val;
   }

   do
   {
      str[strOffset++] = (char)(((int)val % 10) + '0');
   }
   while ((val /= 10) > 0);

   if (isNeg)
   {
      str[strOffset++] = '-';
   }

   str[strOffset] = '\0';
   MemoryReverse<char>(str, strOffset);
}

template<class T>
typename std::enable_if<std::is_unsigned<T>::value, void>::type
TToString(_IN_ T val, _INOUT_STR_Z_ char* str)
{
   //Got this from http://www.cplusplus.com/articles/D9j2Nwbp/
   size_t strOffset = 0;

   do
   {
      str[strOffset++] = (char)(((int)val % 10) + '0');
   }
   while ((val /= 10) > 0);

   str[strOffset] = '\0';
   MemoryReverse<char>(str, strOffset);
}


extern LIB_EXPORT size_t FormatString(const char* fmt, char* const str, ...) __attribute__((format(printf, 1, 3)));

extern void ClearScreen(_IN_ENUM_ const VGA_MODES mode,
                        _IN_ENUM_ const VGA_COLORS background);

extern void PrintCharacter(_IN_INT_ const uint8_t vgaAttr,
                           _IN_ const char character,
                           uint16_t x, uint16_t y);

static size_t GetScreenHeight()
{
   return GetVGADriverPtr()->GetHeight();
}

static size_t GetScreenWidth()
{
   return GetVGADriverPtr()->GetWidth();
}
