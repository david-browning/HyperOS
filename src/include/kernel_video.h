#pragma once
#include "annotations.h"

struct vbe_t
{
   uint16_t Mode;
   uint16_t InterfaceSegment;
   uint16_t InterfaceOffset;
   uint16_t InterfaceLength;
   uint8_t ControlInfo[512];
   uint8_t ModeInfo[256];
};

struct frame_buffer_info_t
{
   uint64_t PhysicalAddress;
   uint32_t Pitch;
   uint32_t WidthPixels;
   uint32_t HeightPixels;

   uint32_t WidthChars;
   uint32_t HeightChars;

   uint32_t CursorRow;
   uint32_t CursorCol;

   uint8_t BitsPerPixel;
};


enum class VGA_COLORS : uint8_t
{
   Black = 0x0,
   Blue = 0x1,
   Green = 0x2,
   Cyan = 0x3,
   Red = 0x4,
   Magenta = 0x5,
   Brown = 0x6,
   LightGray = 0x7,
   DarkGray = 0x8,
   LightBlue = 0x9,
   LightGreen = 0xa,
   LightCyan = 0xb,
   LightRed = 0xc,
   Pink = 0xd,
   Yellow = 0xe,
   White = 0xf,
};

enum class VGA_MODES : uint8_t
{
   VGA,
};

typedef uint16_t VGAChar;

static constexpr uint8_t operator&(const VGA_COLORS c, const int mask)
{
   return uint8_t(uint8_t(c) & uint8_t(mask));
}

constexpr uint8_t vgaMakeAttribute(_IN_ENUM_ const VGA_COLORS foreground,
                                   _IN_ENUM_ const VGA_COLORS background,
                                   _IN_BOOL_ const bool blink)
{
   return uint8_t(uint8_t(blink ? 0x80 : 0) | uint8_t((background & 0xf) << 4) | uint8_t(foreground & 0x0F));
}

constexpr VGAChar vgaMakeChar(_IN_CHAR_ const uint8_t c,
                              _IN_INT_ const uint8_t attribute)
{
   return uint16_t((uint16_t(attribute) << 8) | uint16_t(c));
}

/*
Background color of the kernel console.
*/
static constexpr VGA_COLORS CONSOLE_COLOR = VGA_COLORS::Blue;

/*
Foreground color of the kernel console.
*/
static constexpr VGA_COLORS CONSOLE_TEXT_COLOR = VGA_COLORS::White;

/*
The default attribute for console output.
*/
static constexpr uint8_t CONSOLE_DEFAULT_ATTRIBUTE = vgaMakeAttribute(CONSOLE_TEXT_COLOR,
                                                                      CONSOLE_COLOR,
                                                                      false);

static constexpr uint8_t CONSOLE_ERROR_ATTRIBUTE = vgaMakeAttribute(VGA_COLORS::White,
                                                                    VGA_COLORS::Red,
                                                                    true);

static constexpr uint8_t CONSOLE_SUCCESS_ATTRIBUTE = vgaMakeAttribute(VGA_COLORS::White,
                                                                      VGA_COLORS::Green,
                                                                      false);
