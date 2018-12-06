#include "drvr_vga.h"
#include "mem.h"

constexpr VGAChar* VGA_BUF_ADDR = (VGAChar*)0xB8000;
constexpr uint16_t VGA_WIDTH_DEFAULT = 80;
constexpr uint16_t VGA_HEIGHT_DEFAULT = 25;
static constexpr uint16_t VGA_TAB_STOPS = 3;

static VGADriver* __vga_driver__;

void VGADriver::Clear(_IN_ENUM_ const VGA_MODES mode, _IN_ENUM_ const VGA_COLORS background)
{
   this->_cursorCol = 0;
   this->_cursorRow = 0;
   this->_height = VGA_HEIGHT_DEFAULT;
   this->_width = VGA_WIDTH_DEFAULT;
   this->_hOffset = 0;
   this->_vOffset = 0;

   const uint8_t attr = vgaMakeAttribute(VGA_COLORS::White, background, false);
   const VGAChar setChar = vgaMakeChar('\0', attr);

   MemorySet<VGAChar>(VGA_BUF_ADDR, setChar, GetBufferSize());
}

void VGADriver::WriteCharacter(_IN_INT_ const uint8_t vgaAttr, _IN_ const char character)
{
   switch (character)
   {
      case '\n':
      {
         this->_cursorRow = uint16_t((this->_cursorRow + uint16_t(1)) % this->_height);
         this->_cursorCol = 0;
         break;
      }
      case '\t':
      {
         this->_cursorCol = uint16_t((this->_cursorCol + VGA_TAB_STOPS) % this->_width);
         break;
      }
      default:
      {
         //Write the character
         PutCharacter(this->_cursorRow, this->_cursorCol, vgaMakeChar(character, vgaAttr));

         if ((this->_cursorCol + uint16_t(1)) == this->_width)
         {
            this->_cursorRow = uint16_t((this->_cursorRow + uint16_t(1)) % this->_height);
            this->_cursorCol = 0;
         }
         else
         {
            this->_cursorCol = uint16_t((this->_cursorCol + uint16_t(1)) % this->_width);
         }

         break;
      }
   }
}

size_t VGADriver::WriteString(_IN_INT_ const uint8_t vgaAttr, _IN_STR_Z_ const char* str)
{
   const size_t strlength = MemoryLength<char>(str);
   for (int i = 0; i < strlength; i++)
   {
      WriteCharacter(vgaAttr, str[i]);
   }

   return strlength;
}

void VGADriver::PutCharacter(_IN_INT_ const uint16_t row, _IN_INT_ const uint16_t col, _IN_ const VGAChar character)
{
   VGA_BUF_ADDR[row * this->_width + col] = character;
}

void VGADriver::ScrollDown()
{
   MemoryCopy<VGAChar>(VGA_BUF_ADDR,
                       VGA_BUF_ADDR + this->_width,
                       this->GetBufferSize() - (this->_width * sizeof(VGAChar)));
}

result_t VGADriver::Load(_IN_HANDLE_ handle_t driverHandle)
{
   this->_cursorCol = 0;
   this->_cursorRow = 0;
   this->_height = VGA_HEIGHT_DEFAULT;
   this->_width = VGA_WIDTH_DEFAULT;
   this->_hOffset = 0;
   this->_vOffset = 0;
   return RES_SUCCESS;
}

result_t VGADriver::Write(_IN_BYTE_ARR_ const uint8_t * buffer,
                          _IN_SIZE_ size_t bufLen,
                          _IN_SIZE_ size_t byteOffset)
{
   return RES_SUCCESS;
}

result_t VGADriver::Read(_IN_SIZE_ size_t bufLen,
                         _OUT_BYTE_ARR_ uint8_t * buffer,
                         _IN_SIZE_ size_t byteOffset)
{
   return RES_UNSUPPORTED;
}

result_t VGADriver::Unload()
{
   return RES_SUCCESS;
}

result_t VGADriver::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   __vga_driver__ = this;
   return RES_SUCCESS;
}

result_t VGADriver::SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   return RES_SUCCESS;
}

result_t VGADriver::Interrupt(irqnumber_t irq)
{
   return RES_SUCCESS;
}

VGADriver* GetVGADriverPtr()
{
   return __vga_driver__;
}
