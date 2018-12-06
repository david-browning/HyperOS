#pragma once

#include "annotations.h"
#include "kernel_device_driver.h"
#include "kernel_video.h"

class VGADriver : public IDeviceDriver
{
   public:

   result_t Load(_IN_HANDLE_ handle_t driverHandle);

   result_t Write(_IN_BYTE_ARR_ const uint8_t* buffer, _IN_SIZE_ size_t bufLen, _IN_SIZE_ size_t byteOffset);

   result_t Read(_IN_SIZE_ size_t bufLen, _OUT_BYTE_ARR_ uint8_t* buffer, _IN_SIZE_ size_t byteOffset);

   result_t Unload();

   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

   result_t Interrupt(irqnumber_t irq);

   /*
   Clears the screen with the given color and outputs a VGAInfo struct which is used to put
   characters on the screen. vgaInfo cannot be null.
   */
   void Clear(_IN_ENUM_ const VGA_MODES mode,
              _IN_ENUM_ const VGA_COLORS background);


   /*
   Writes the character to the screen at the screen's current position.
   Advances the vgaInfo buffer by 1 character.
   */
   void WriteCharacter(_IN_INT_ const uint8_t vgaAttr,
                       _IN_ const char character);

   /*
   Writes the string to the screen at the screen's current position.
   Advances the vgaInfo buffer by the number characters in the string.
   Returns the number of characters written.
   */
   size_t WriteString(_IN_INT_ const uint8_t vgaAttr,
                      _IN_STR_Z_ const char* st);

   /*
   Puts the character at the given row and column on the screen.
   This does not modify the vgaInfo.
   */
   void PutCharacter(_IN_INT_ const uint16_t row,
                     _IN_INT_ const uint16_t col,
                     _IN_ const VGAChar character);

   inline size_t GetHeight()
   {
      return _height;
   }

   inline size_t GetWidth()
   {
      return _width;
   }

   void ScrollDown();

   inline size_t GetBufferSize()
   {
      return this->_width * this->_height * sizeof(VGAChar);
   }

   private:
   uint16_t _cursorRow;
   uint16_t _cursorCol;
   uint16_t _width;
   uint16_t _height;

   uint16_t _vOffset;
   uint16_t _hOffset;

   VGA_MODES _mode;
};

extern VGADriver* GetVGADriverPtr();
