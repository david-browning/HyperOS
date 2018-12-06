#include "kernel_print.h"
#include "kernel_video.h"
#include <stdarg.h>
#include "mem.h"
#include "drvr_vga.h"
#include <type_traits>


static constexpr char HEX_CHARS[16] =
{
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'A', 'B', 'C', 'D', 'E', 'F'
};

static constexpr char HEX_CHARS_LOWER[16] =
{
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f'
};

/*
 Formats number and stores it in str.
 Returns the length of the formatted string.
 */
template<typename T>
size_t FormatInt(T number, char* const str, char nextFormat)
{
   size_t len = 0;
   switch (nextFormat)
   {
      case 'd':
      case 'i':
      {
         TToString<std::make_signed<T>::type>(number, str);
         len = MemoryLength<char>(str);
         break;
      }
      case 'u':
      {
         TToString<std::make_unsigned<T>::type>(number, str);
         len = MemoryLength<char>(str);
         break;
      }
      case 'o':
      case 'x':
      case 'p':
      case 'X':
      {
         const char* alphabet = (nextFormat == 'X' ? HEX_CHARS : HEX_CHARS_LOWER);
         str[len++] = '0';
         str[len++] = 'x';

         //Number of characters needed for the hex string
         constexpr size_t numChars = sizeof(T) * __CHAR_BIT__ / 4;
         for (int i = numChars - 1; i >= 0; i--)
         {
            str[len++] = alphabet[(number >> (i * 4)) & 0xF];
         }

         break;
      }
      default:
      {
         break;
      }
   }

   return len;
}

size_t FormatString(const char* fmt, char* const str, ...)
{
   size_t charsWritten = 0;
   va_list args;
   va_start(args, fmt);

   while (*fmt != '\0')
   {
      if (*fmt == '%')
      {
         fmt++;

         //Read the number of digits after the percent.
         int charsToPrint = 0;
         while (*fmt >= '0' && *fmt <= '9')
         {
            charsToPrint = 10 * charsToPrint + (*fmt - '0');
            fmt++;
         }
         if (*fmt >= '0' && *fmt <= '9')
         {
            fmt++;
         }

         switch (*fmt)
         {
            case 'd':
            case 'i':
            {
               char intFormatted[16];
               size_t stringLength = FormatInt<int>(va_arg(args, int), intFormatted, *fmt);
               MemoryCopy(str + charsWritten, intFormatted, stringLength);
               charsWritten += stringLength;
               break;
            }
            case 'p':
            case 'x':
            case 'X':
            case 'u':
            {
               char intFormatted[16];
               size_t stringLength = FormatInt<unsigned int>(va_arg(args, unsigned int), intFormatted, *fmt);
               MemoryCopy(str + charsWritten, intFormatted, stringLength);
               charsWritten += stringLength;
               break;
            }
            case 'l':
            {
               fmt++;
               char intFormatted[32];
               size_t stringLength = FormatInt<long int>(va_arg(args, long int), intFormatted, *fmt);
               MemoryCopy(str + charsWritten, intFormatted, stringLength);
               charsWritten += stringLength;
               break;
            }
            case 'q':
            {
               fmt++;
               char intFormatted[32];
               size_t stringLength = FormatInt<int64_t>(va_arg(args, int64_t), intFormatted, *fmt);
               MemoryCopy(str + charsWritten, intFormatted, stringLength);
               charsWritten += stringLength;
               break;
            }
            case 'z':
            {
               fmt++;
               char intFormatted[32];
               size_t stringLength = FormatInt<size_t>(va_arg(args, size_t), intFormatted, *fmt);
               MemoryCopy(str + charsWritten, intFormatted, stringLength);
               charsWritten += stringLength;
               break;
            }
            case 'h':
            {
               fmt++;
               char intFormatted[32];
               size_t stringLength = FormatInt<short>((short)va_arg(args, int), intFormatted, *fmt);
               MemoryCopy(str + charsWritten, intFormatted, stringLength);
               charsWritten += stringLength;
               break;
            }
            case 'c':
            {
               str[charsWritten++] = (char)va_arg(args, int);
               break;
            }
            case 's':
            {
               char* strArg = va_arg(args, char*);
               size_t stringLength = MemoryLength<char>(strArg);
               MemoryCopy(str + charsWritten, strArg, stringLength);
               charsWritten += stringLength;
               break;
            }
            default:
            {
               str[charsWritten++] = *fmt;
               break;
            }
         }
      }
      else
      {
         str[charsWritten++] = *fmt;
      }
      fmt++;

   }

   va_end(args);
   str[charsWritten] = '\0';
   return charsWritten;
}

void PrintResult(result_t res)
{
   char str[26];
   FormatString("Result: %qx", str, res);

   uint8_t attr = CONSOLE_SUCCESS_ATTRIBUTE;
   if (ResultFailed(res))
   {
      attr = CONSOLE_ERROR_ATTRIBUTE;
   }

   //Put the string on the second line of the display.
   static constexpr uint16_t* mem = (uint16_t*)0xB80A0;
   for (auto i = 0; i < 26; i++)
   {
      mem[i] = vgaMakeChar(str[i], attr);
   }
}

void ClearScreen(_IN_ENUM_ const VGA_MODES mode, _IN_ENUM_ const VGA_COLORS background)
{
   GetVGADriverPtr()->Clear(mode, background);
}

void PrintCharacter(_IN_INT_ const uint8_t vgaAttr, _IN_ const char character,
                    uint16_t x, uint16_t y)
{
   GetVGADriverPtr()->PutCharacter(y, x, vgaMakeChar(character, vgaAttr));
}

