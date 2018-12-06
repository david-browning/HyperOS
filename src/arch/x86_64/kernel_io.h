#pragma once

#include "annotations.h"
#include <type_traits>

typedef uint16_t ioport_t;

inline void IOWait()
{
   __asm
   {
      push rax;
      out 0x80, ax;
      pop rax;
   }
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint8_t), void>::type
INLINE_K IOWriteToPort(_IN_ const ioport_t port,
                       _IN_ const T val)
{
   __asm
   {
      push rax;
      mov al, val;
      mov dx, port;
      out dx, al;
      pop rax;
   }
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), void>::type
INLINE_K IOWriteToPort(_IN_ const ioport_t port,
                       _IN_ const T val)
{
   __asm
   {
      push rax;
      mov ax, val;
      mov dx, port;
      out dx, ax;
      pop rax;
   }
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), void>::type
INLINE_K IOWriteToPort(_IN_ const ioport_t port,
                       _IN_ const T val)
{
   __asm
   {
      push rax;
      mov eax, val;
      mov dx, port;
      out dx, eax;
      pop rax;
   }
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint8_t), T>::type
INLINE_K IOReadFromPort(_IN_ const ioport_t port)
{
   __asm
   {
      mov dx, port;
      in al, dx;
   }
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
INLINE_K IOReadFromPort(_IN_ const ioport_t port)
{
   __asm
   {
      mov dx, port;
      in ax, dx;
   }
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
INLINE_K IOReadFromPort(_IN_ const ioport_t port)
{
   __asm
   {
      mov dx, port;
      in eax, dx;
   }
}