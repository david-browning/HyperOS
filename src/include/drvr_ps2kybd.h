#pragma once

#include "annotations.h"
#include "kernel_device_driver.h"
#include "kernel_io.h"
#include "key_codes.h"
#include "drv_block_dev.h"
#include "queue.h"

class PS2KeyboardDriver : public IDeviceDriver
{
   public:
   PS2KeyboardDriver();

   virtual ~PS2KeyboardDriver();

   virtual result_t Load(_IN_HANDLE_ handle_t driverHandle);

   virtual result_t Write(_IN_BYTE_ARR_ const uint8_t* buffer, _IN_SIZE_ size_t bufLen, _IN_SIZE_ size_t byteOffset);

   virtual result_t Read(_IN_SIZE_ size_t bufLen, _OUT_BYTE_ARR_ uint8_t* buffer, _IN_SIZE_ size_t byteOffset);

   virtual result_t Unload();

   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

   virtual result_t Interrupt(_IN_INT_ irqnumber_t irq);

   inline bool IsDualChannel()
   {
      return this->_isDualChannel;
   }

   /*
    Waits until a virtual key is available, then returns it.
    */
   VIRTUAL_KEYS GetNextVirtualKey();

   /*
    Returns the scan code set the driver is using.
    */
   result_t SetScanCodeSet(_IN_ENUM_ KEYBOARD_SCAN_SETS set);

   inline KEYBOARD_SCAN_SETS GetScanCodeSet();

   private:
   /*
   Number of virtual keys that stay in the keyboard buffer.
   More keys means more memory, but a lower change a key will get lost if the system locks up.
   */
   static constexpr uint8_t KEYBOARD_BACK_BUFFER_COUNT = 64;
   /*
   Used for reading and writing data that was received from a PS2 device or the controller itself.
   */
   static constexpr ioport_t KEYBOARD_DATA_PORT = 0x60;

   /*
   Contains flags that indicate the state of the of the controller.
   Also used to set commands to the controller.
   */
   static constexpr ioport_t KEYBAORD_COMMAND_PORT = 0x64;

   /*
   Number of times to retry a keyboard command before giving up.
   */
   static constexpr uint8_t KEYBOARD_RETRY_COUNT = 3;

   static constexpr uint8_t KEYBOARD_COMMAND_ACKNOWLEDGED = 0xFA;
   static constexpr uint8_t KEYBOARD_COMMAND_RESEND = 0xFE;

   /*
   Waits for a scan code to be available then returns it.
   */
   inline uint8_t _getScanCode()
   {
      return _readDataReg();
   }

   uint8_t _readDataReg()
   {
      return IOReadFromPort<uint8_t>(KEYBOARD_DATA_PORT);
   }

   uint8_t _readStatusReg()
   {
      return _readDataReg();
   }

   void _waitForOutputEmpty()
   {
      while ((_readStatusReg() & 0b0000'0001) != 0);
   }

   void _waitForOutputFull()
   {
      while ((_readStatusReg() & 0b0000'0001) == 0);
   }

   void _waitForInputFull()
   {
      while ((_readStatusReg() & 0b0000'0010) == 0);
   }

   void _waitForInputEmpty()
   {
      while ((_readStatusReg() & 0b0000'0010) != 0);
   }

   //True if the PS2 is dual channel
   bool _isDualChannel;

   //Buffer of keys pressed.
   Queue<uint8_t> _kbBuffer;

   void _writeDataRegister(uint8_t data);

   //Pointer to the scan code mapping the driver is using.
   const VIRTUAL_KEYS* _scanCodeSet;
};

extern result_t KBHandler(irqnumber_t irqNumber, uint32_t error, void* data_p);

extern PS2KeyboardDriver* GetKeyboardDriverPtr();