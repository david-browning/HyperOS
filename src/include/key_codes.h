#pragma once

#include "annotations.h"


inline constexpr int16_t MakeVirtualKey(const bool isPrintable,
                                        const bool isPressed,
                                        const uint8_t section,
                                        const uint8_t row,
                                        const uint8_t code)
{
   return (int16_t)(
      (isPressed ? (int16_t)(1 << 15) : 0) |
      ((row & 0x07) << 12) |
      ((section & 0x07) << 9) |
      (isPrintable ? (1 << 8) : 0) |
      code
      );
}

/*
Virtual Keys have the following structure:
bit 0-7 Key Code. Often this can be printed to the VGA console.
bit 8 Is printable.
bit 9-11 Keyboard Section
0 Is Main Section
1 Is Function Keys
2 Is Arrow Keys
3 Is Number Pad
4 Is Special Keys
5 Is Multimedia keys.
7 Is Mouse
bit 12-14 Keyboard row within the section.
bit 15 1 indicates key pressed
*/
enum VIRTUAL_KEYS : int16_t
{
   KEY_NONE = MakeVirtualKey(false, false, 0, 0, 0x00),

   //Mouse PS Virtual Keys
   KEY_MOUSE_L = MakeVirtualKey(false, true, 7, 0, 0x01),
   KEY_MOUSE_R = MakeVirtualKey(false, true, 7, 0, 0x02),
   KEY_MOUSE_MIDDLE = MakeVirtualKey(false, true, 7, 0, 0x03),
   KEY_MOUSE_1 = MakeVirtualKey(false, true, 7, 0, 0x04),
   KEY_MOUSE_2 = MakeVirtualKey(false, true, 7, 0, 0x05),
   //Additional mouse keys

   //Movement Keys
   KEY_PAGE_UP = MakeVirtualKey(false, true, 4, 1, 0x08),
   KEY_PAGE_DOWN = MakeVirtualKey(false, true, 4, 2, 0x09),
   KEY_END = MakeVirtualKey(false, true, 4, 2, 0x0A),
   KEY_HOME = MakeVirtualKey(false, true, 4, 1, 0x0B),
   KEY_ARROW_LEFT = MakeVirtualKey(false, true, 2, 1, 0x0C),
   KEY_ARROW_RIGHT = MakeVirtualKey(false, true, 2, 1, 0x0D),
   KEY_ARROW_UP = MakeVirtualKey(false, true, 2, 0, 0x0E),
   KEY_ARROW_DOWN = MakeVirtualKey(false, true, 2, 1, 0x0F),

   //Modifier Keys
   KEY_SHIFT_L = MakeVirtualKey(false, true, 0, 3, 0x10),
   KEY_SHIFT_R = MakeVirtualKey(false, true, 0, 3, 0x11),
   KEY_WINDOWS_L = MakeVirtualKey(false, true, 0, 4, 0x12),
   KEY_WINDOWS_R = MakeVirtualKey(false, true, 0, 4, 0x13),
   KEY_ALT_L = MakeVirtualKey(false, true, 0, 4, 0x14),
   KEY_ALT_R = MakeVirtualKey(false, true, 0, 4, 0x15),
   KEY_CTRL_L = MakeVirtualKey(false, true, 0, 4, 0x16),
   KEY_CTRL_R = MakeVirtualKey(false, true, 0, 4, 0x17),
   //Additional modifier keys

   //Printable characters
   KEY_SPACE = MakeVirtualKey(true, true, 0, 4, 0x20),
   KEY_BACKSPACE = 0x21,
   KEY_TAB = MakeVirtualKey(true, true, 0, 1, 0x09),
   KEY_RETURN = MakeVirtualKey(true, true, 0, 2, 0x0A),
   KEY_DELETE = 0x24,

   //0x25, 0x26

   KEY_QUOTE = MakeVirtualKey(true, true, 0, 2, 0x27),
   KEY_COMMA = MakeVirtualKey(true, true, 0, 3, 0x2C),

   //0x28, 0x29, 0x2a, 0x2b

   KEY_MINUS = MakeVirtualKey(true, true, 0, 0, 0x2D),
   KEY_PERIOD = MakeVirtualKey(true, true, 0, 3, 0x2E),
   KEY_FORWARD_SLASH = MakeVirtualKey(true, true, 0, 3, 0x2F),

   KEY_0 = MakeVirtualKey(true, true, 0, 0, 0x30),
   KEY_1 = MakeVirtualKey(true, true, 0, 0, 0x31),
   KEY_2 = MakeVirtualKey(true, true, 0, 0, 0x32),
   KEY_3 = MakeVirtualKey(true, true, 0, 0, 0x33),
   KEY_4 = MakeVirtualKey(true, true, 0, 0, 0x34),
   KEY_5 = MakeVirtualKey(true, true, 0, 0, 0x35),
   KEY_6 = MakeVirtualKey(true, true, 0, 0, 0x36),
   KEY_7 = MakeVirtualKey(true, true, 0, 0, 0x37),
   KEY_8 = MakeVirtualKey(true, true, 0, 0, 0x38),
   KEY_9 = MakeVirtualKey(true, true, 0, 0, 0x39),

   KEY_COLON = MakeVirtualKey(true, true, 0, 2, 0x3B),
   //0x3c
   KEY_EQUALS = MakeVirtualKey(true, true, 0, 0, 0x3D),
   //0x3e, 0x3f

   //Number pad
   KEY_NUM_0 = MakeVirtualKey(true, true, 3, 4, 0x30),
   KEY_NUM_1 = MakeVirtualKey(true, true, 3, 3, 0x31),
   KEY_NUM_2 = MakeVirtualKey(true, true, 3, 3, 0x32),
   KEY_NUM_3 = MakeVirtualKey(true, true, 3, 3, 0x33),
   KEY_NUM_4 = MakeVirtualKey(true, true, 3, 2, 0x34),
   KEY_NUM_5 = MakeVirtualKey(true, true, 3, 2, 0x35),
   KEY_NUM_6 = MakeVirtualKey(true, true, 3, 2, 0x36),
   KEY_NUM_7 = MakeVirtualKey(true, true, 3, 1, 0x37),
   KEY_NUM_8 = MakeVirtualKey(true, true, 3, 1, 0x38),
   KEY_NUM_9 = MakeVirtualKey(true, true, 3, 1, 0x39),
   KEY_NUM_MUL = MakeVirtualKey(true, true, 3, 0, 0x2A),
   KEY_NUM_ADD = MakeVirtualKey(true, true, 3, 1, 0x2B),
   KEY_NUM_SUB = MakeVirtualKey(true, true, 3, 0, 0x2D),
   KEY_NUM_PERIOD = MakeVirtualKey(true, true, 3, 4, 0x2E),
   KEY_NUM_DIV = MakeVirtualKey(true, true, 3, 0, 0x2F),
   KEY_NUM_RETURN = 0x4e,
   //Additional potential num pad keys

   //Locking Keys
   KEY_FUNCTION_LOCK = MakeVirtualKey(false, true, 4, 0, 0x57),
   KEY_SCROLL_LOCK = MakeVirtualKey(false, true, 4, 0, 0x58),
   KEY_NUM_LOCK = MakeVirtualKey(false, true, 3, 0, 0x59),
   KEY_CAPS_LOCK = MakeVirtualKey(false, true, 0, 2, 0x5A),

   //More Printable keys
   KEY_BRACE_L = MakeVirtualKey(true, true, 0, 1, 0x5B),
   KEY_BACK_SLASH = MakeVirtualKey(true, true, 0, 1, 0x5C),
   KEY_BRACE_R = MakeVirtualKey(true, true, 0, 1, 0x71),
   KEY_BACK_TICK = MakeVirtualKey(true, true, 0, 0, 0x60),

   KEY_A = MakeVirtualKey(true, true, 0, 2, 0x61),
   KEY_B = MakeVirtualKey(true, true, 0, 3, 0x62),
   KEY_C = MakeVirtualKey(true, true, 0, 3, 0x63),
   KEY_D = MakeVirtualKey(true, true, 0, 2, 0x64),
   KEY_E = MakeVirtualKey(true, true, 0, 1, 0x65),
   KEY_F = MakeVirtualKey(true, true, 0, 2, 0x66),
   KEY_G = MakeVirtualKey(true, true, 0, 2, 0x67),
   KEY_H = MakeVirtualKey(true, true, 0, 2, 0x68),
   KEY_I = MakeVirtualKey(true, true, 0, 1, 0x69),
   KEY_J = MakeVirtualKey(true, true, 0, 2, 0x6A),
   KEY_K = MakeVirtualKey(true, true, 0, 2, 0x6B),
   KEY_L = MakeVirtualKey(true, true, 0, 2, 0x6C),
   KEY_M = MakeVirtualKey(true, true, 0, 3, 0x6D),
   KEY_N = MakeVirtualKey(true, true, 0, 3, 0x6E),
   KEY_O = MakeVirtualKey(true, true, 0, 1, 0x6F),
   KEY_P = MakeVirtualKey(true, true, 0, 1, 0x70),
   KEY_Q = MakeVirtualKey(true, true, 0, 1, 0x71),
   KEY_R = MakeVirtualKey(true, true, 0, 1, 0x72),
   KEY_S = MakeVirtualKey(true, true, 0, 2, 0x73),
   KEY_T = MakeVirtualKey(true, true, 0, 1, 0x74),
   KEY_U = MakeVirtualKey(true, true, 0, 1, 0x75),
   KEY_V = MakeVirtualKey(true, true, 0, 3, 0x76),
   KEY_W = MakeVirtualKey(true, true, 0, 1, 0x77),
   KEY_X = MakeVirtualKey(true, true, 0, 3, 0x78),
   KEY_Y = MakeVirtualKey(true, true, 0, 1, 0x79),
   KEY_Z = MakeVirtualKey(true, true, 0, 3, 0x7A),
   //End printable keys.

   KEY_PRINT_SCREEN = 0x0080,
   KEY_PAUSE = 0x0081,
   KEY_MENU = 0x0082,
   KEY_ESCAPE = 0x0083,
   KEY_INSERT = 0x0084,
   //Additional special keys

   //Function Keys
   KEY_F1 = 0x90,
   KEY_F2 = 0x91,
   KEY_F3 = 0x92,
   KEY_F4 = 0x93,
   KEY_F5 = 0x94,
   KEY_F6 = 0x95,
   KEY_F7 = 0x96,
   KEY_F8 = 0x97,
   KEY_F9 = 0x98,
   KEY_F10 = 0x99,
   KEY_F11 = 0x9a,
   KEY_F12 = 0x9e,

   //Multimedia Keys
   KEY_MM_SEARCH = 0x00A0,
   KEY_MM_FAVORITES = 0x00A1,
   KEY_MM_REFRESH = 0x00A2,
   KEY_MM_STOP = 0x00A3,
   KEY_MM_FORWARD = 0x00A4,
   KEY_MM_PLAY = 0x00A5,
   KEY_MM_BACK = 0x00A6,
   KEY_MM_MY_COMPUTER = 0x00A7,
   KEY_MM_EMAIL = 0x00A8,
   KEY_MM_SELECT = 0x00A9,
   KEY_MM_PREVIOUS = 0x00AA,
   KEY_MM_VOLUME_DOWN = 0x00AB,
   KEY_MM_NEXT = 0x00AC,
   KEY_MM_VOLUME_UP = 0x00AD,
   KEY_MM_HOME = 0x00AE,
   KEY_MM_APPS = 0x00AE,
   KEY_MM_CALCULATOR = 0x00AF,
   KEY_MM_MUTE = 0x00B0,

   //ACPI Keys?
   KEY_WAKE = 0x00C0,
   KEY_SLEEP = 0x00C1,
   KEY_POWER = 0x00C2,
};

enum class KEYBOARD_SCAN_SETS
{
   SCAN_1 = 1,
   SCAN_2 = 2,
   SCAN_3 = 4,
};

inline constexpr VIRTUAL_KEYS operator&(const VIRTUAL_KEYS k, const int mask)
{
   return (VIRTUAL_KEYS)((int16_t)k & mask);
}

inline constexpr VIRTUAL_KEYS operator|(const VIRTUAL_KEYS k, const int mask)
{
   return (VIRTUAL_KEYS)((int16_t)k | mask);
}

/*
Returns true if the virtual key has a character representation such as a number or letter.
*/
inline constexpr bool VirtualKeyIsPrintable(const VIRTUAL_KEYS k)
{
   return (int)(k & (0b00000001'00000000)) != 0;
}

/*
Returns true if the virtual key has its "pressed" flag set.
*/
inline constexpr bool VirtualKeyIsPressed(const VIRTUAL_KEYS k)
{
   return (int16_t)k < 0;
}

/*
Return the virtual key code with its "pressed" flag set.
*/
inline constexpr VIRTUAL_KEYS PressVirtualKey(const VIRTUAL_KEYS k)
{
   return (VIRTUAL_KEYS)(k | 0b1000'0000'0000'0000);
}

/*
Returns the virtual key code with its "pressed" flag cleared.
*/
inline constexpr VIRTUAL_KEYS ReleaseVirtualKey(const VIRTUAL_KEYS k)
{
   return (VIRTUAL_KEYS)(k & ~0b1000'0000'0000'0000);
}

/*
Returns the character representation of a virtual key.
Keys that do not have a character representation will return '!'
*/
inline constexpr char VirtualKeyToCharacter(const VIRTUAL_KEYS k)
{
   return VirtualKeyIsPrintable(k) ? (char)(k & 0x00FF) : '!';
}


//Maps the scan code for scan code set 2 to a key.
//Index 0 is scan code zero and should return the virtual key.
//Example: index 0x29 should contain the space virtual key.
constexpr VIRTUAL_KEYS KEYBOARD_MAP_SET_1[256] =
{
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_ESCAPE,
   VIRTUAL_KEYS::KEY_1,
   VIRTUAL_KEYS::KEY_2,
   VIRTUAL_KEYS::KEY_3,
   VIRTUAL_KEYS::KEY_4,
   VIRTUAL_KEYS::KEY_5,
   VIRTUAL_KEYS::KEY_6,
   VIRTUAL_KEYS::KEY_7,
   VIRTUAL_KEYS::KEY_8,
   VIRTUAL_KEYS::KEY_9,
   VIRTUAL_KEYS::KEY_0,
   VIRTUAL_KEYS::KEY_MINUS,
   VIRTUAL_KEYS::KEY_EQUALS,
   VIRTUAL_KEYS::KEY_BACKSPACE,
   VIRTUAL_KEYS::KEY_TAB,
   VIRTUAL_KEYS::KEY_Q,
   VIRTUAL_KEYS::KEY_W,
   VIRTUAL_KEYS::KEY_E,
   VIRTUAL_KEYS::KEY_R,
   VIRTUAL_KEYS::KEY_T,
   VIRTUAL_KEYS::KEY_Y,
   VIRTUAL_KEYS::KEY_U,
   VIRTUAL_KEYS::KEY_I,
   VIRTUAL_KEYS::KEY_O,
   VIRTUAL_KEYS::KEY_P,
   VIRTUAL_KEYS::KEY_BRACE_L,
   VIRTUAL_KEYS::KEY_BRACE_R,
   VIRTUAL_KEYS::KEY_RETURN,
   VIRTUAL_KEYS::KEY_CTRL_L,
   VIRTUAL_KEYS::KEY_A,
   VIRTUAL_KEYS::KEY_S,
   VIRTUAL_KEYS::KEY_D,
   VIRTUAL_KEYS::KEY_F,
   VIRTUAL_KEYS::KEY_G,
   VIRTUAL_KEYS::KEY_H,
   VIRTUAL_KEYS::KEY_J,
   VIRTUAL_KEYS::KEY_K,
   VIRTUAL_KEYS::KEY_L,
   VIRTUAL_KEYS::KEY_COLON,
   VIRTUAL_KEYS::KEY_QUOTE,
   VIRTUAL_KEYS::KEY_BACK_TICK,
   VIRTUAL_KEYS::KEY_SHIFT_L,
   VIRTUAL_KEYS::KEY_BACK_SLASH,
   VIRTUAL_KEYS::KEY_Z,
   VIRTUAL_KEYS::KEY_X,
   VIRTUAL_KEYS::KEY_C,
   VIRTUAL_KEYS::KEY_V,
   VIRTUAL_KEYS::KEY_B,
   VIRTUAL_KEYS::KEY_N,
   VIRTUAL_KEYS::KEY_M,
   VIRTUAL_KEYS::KEY_COMMA,
   VIRTUAL_KEYS::KEY_PERIOD,
   VIRTUAL_KEYS::KEY_FORWARD_SLASH,
   VIRTUAL_KEYS::KEY_SHIFT_R,
   VIRTUAL_KEYS::KEY_NUM_MUL,
   VIRTUAL_KEYS::KEY_ALT_L,
   VIRTUAL_KEYS::KEY_SPACE,
   VIRTUAL_KEYS::KEY_CAPS_LOCK,
   VIRTUAL_KEYS::KEY_F1,
   VIRTUAL_KEYS::KEY_F2,
   VIRTUAL_KEYS::KEY_F3,
   VIRTUAL_KEYS::KEY_F4,
   VIRTUAL_KEYS::KEY_F5,
   VIRTUAL_KEYS::KEY_F6,
   VIRTUAL_KEYS::KEY_F7,
   VIRTUAL_KEYS::KEY_F8,
   VIRTUAL_KEYS::KEY_F9,
   VIRTUAL_KEYS::KEY_F10,
   VIRTUAL_KEYS::KEY_NUM_LOCK,
   VIRTUAL_KEYS::KEY_SCROLL_LOCK,
   VIRTUAL_KEYS::KEY_NUM_7,
   VIRTUAL_KEYS::KEY_NUM_8,
   VIRTUAL_KEYS::KEY_NUM_9,
   VIRTUAL_KEYS::KEY_NUM_SUB,
   VIRTUAL_KEYS::KEY_NUM_4,
   VIRTUAL_KEYS::KEY_NUM_5,
   VIRTUAL_KEYS::KEY_NUM_6,
   VIRTUAL_KEYS::KEY_NUM_ADD,
   VIRTUAL_KEYS::KEY_NUM_1,
   VIRTUAL_KEYS::KEY_NUM_2,
   VIRTUAL_KEYS::KEY_NUM_3,
   VIRTUAL_KEYS::KEY_NUM_0,
   VIRTUAL_KEYS::KEY_NUM_PERIOD,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_F11,
   VIRTUAL_KEYS::KEY_F12,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
};

constexpr VIRTUAL_KEYS KEYBOARD_MAP_SET_2[256] =
{
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_F9,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_F5,
   VIRTUAL_KEYS::KEY_F3,
   VIRTUAL_KEYS::KEY_F1,
   VIRTUAL_KEYS::KEY_F2,
   VIRTUAL_KEYS::KEY_F12,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_F10,
   VIRTUAL_KEYS::KEY_F8,
   VIRTUAL_KEYS::KEY_F6,
   VIRTUAL_KEYS::KEY_F4,
   VIRTUAL_KEYS::KEY_TAB,
   VIRTUAL_KEYS::KEY_BACK_TICK,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_ALT_L,
   VIRTUAL_KEYS::KEY_SHIFT_L,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_CTRL_L,
   VIRTUAL_KEYS::KEY_Q,
   VIRTUAL_KEYS::KEY_1,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_Z,
   VIRTUAL_KEYS::KEY_S,
   VIRTUAL_KEYS::KEY_A,
   VIRTUAL_KEYS::KEY_W,
   VIRTUAL_KEYS::KEY_2,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_C,
   VIRTUAL_KEYS::KEY_X,
   VIRTUAL_KEYS::KEY_D,
   VIRTUAL_KEYS::KEY_E,
   VIRTUAL_KEYS::KEY_4,
   VIRTUAL_KEYS::KEY_3,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_SPACE,
   VIRTUAL_KEYS::KEY_V,
   VIRTUAL_KEYS::KEY_F,
   VIRTUAL_KEYS::KEY_T,
   VIRTUAL_KEYS::KEY_R,
   VIRTUAL_KEYS::KEY_5,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_N,
   VIRTUAL_KEYS::KEY_B,
   VIRTUAL_KEYS::KEY_H,
   VIRTUAL_KEYS::KEY_G,
   VIRTUAL_KEYS::KEY_Y,
   VIRTUAL_KEYS::KEY_6,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_M,
   VIRTUAL_KEYS::KEY_J,
   VIRTUAL_KEYS::KEY_U,
   VIRTUAL_KEYS::KEY_7,
   VIRTUAL_KEYS::KEY_8,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_COMMA,
   VIRTUAL_KEYS::KEY_K,
   VIRTUAL_KEYS::KEY_I,
   VIRTUAL_KEYS::KEY_O,
   VIRTUAL_KEYS::KEY_0,
   VIRTUAL_KEYS::KEY_9,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_PERIOD,
   VIRTUAL_KEYS::KEY_FORWARD_SLASH,
   VIRTUAL_KEYS::KEY_L,
   VIRTUAL_KEYS::KEY_COLON,
   VIRTUAL_KEYS::KEY_P,
   VIRTUAL_KEYS::KEY_MINUS,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_QUOTE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_BRACE_L,
   VIRTUAL_KEYS::KEY_EQUALS,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_CAPS_LOCK,
   VIRTUAL_KEYS::KEY_SHIFT_R,
   VIRTUAL_KEYS::KEY_RETURN,
   VIRTUAL_KEYS::KEY_BRACE_R,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_BACK_SLASH,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_BACKSPACE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NUM_1,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NUM_4,
   VIRTUAL_KEYS::KEY_NUM_7,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NUM_0,
   VIRTUAL_KEYS::KEY_NUM_PERIOD,
   VIRTUAL_KEYS::KEY_NUM_2,
   VIRTUAL_KEYS::KEY_NUM_5,
   VIRTUAL_KEYS::KEY_NUM_6,
   VIRTUAL_KEYS::KEY_NUM_8,
   VIRTUAL_KEYS::KEY_ESCAPE,
   VIRTUAL_KEYS::KEY_NUM_LOCK,
   VIRTUAL_KEYS::KEY_F11,
   VIRTUAL_KEYS::KEY_NUM_ADD,
   VIRTUAL_KEYS::KEY_NUM_3,
   VIRTUAL_KEYS::KEY_NUM_SUB,
   VIRTUAL_KEYS::KEY_NUM_MUL,
   VIRTUAL_KEYS::KEY_NUM_9,
   VIRTUAL_KEYS::KEY_SCROLL_LOCK,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_F7,
   VIRTUAL_KEYS::KEY_MM_SEARCH,
   VIRTUAL_KEYS::KEY_ALT_R,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_CTRL_R,
   VIRTUAL_KEYS::KEY_MM_PREVIOUS,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_FAVORITES,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_WINDOWS_L,
   VIRTUAL_KEYS::KEY_MM_REFRESH,
   VIRTUAL_KEYS::KEY_MM_VOLUME_DOWN,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_MUTE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_WINDOWS_R,
   VIRTUAL_KEYS::KEY_MM_STOP,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_CALCULATOR,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_APPS,
   VIRTUAL_KEYS::KEY_MM_FORWARD,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_VOLUME_UP,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_PLAY,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_POWER,
   VIRTUAL_KEYS::KEY_MM_BACK,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_HOME,
   VIRTUAL_KEYS::KEY_MM_STOP,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_SLEEP,
   VIRTUAL_KEYS::KEY_MM_MY_COMPUTER,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_EMAIL,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NUM_DIV,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_NEXT,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_MM_SELECT,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NUM_RETURN,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_WAKE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_END,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_ARROW_LEFT,
   VIRTUAL_KEYS::KEY_HOME,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_INSERT,
   VIRTUAL_KEYS::KEY_DELETE,
   VIRTUAL_KEYS::KEY_ARROW_DOWN,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_ARROW_RIGHT,
   VIRTUAL_KEYS::KEY_ARROW_UP,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_PAGE_DOWN,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_PAGE_UP,
   VIRTUAL_KEYS::KEY_NONE,
   VIRTUAL_KEYS::KEY_NONE,
};