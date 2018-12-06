#pragma once

#include "annotations.h"
#include "kernel_device_driver.h"

static constexpr ioport_t COM1_PORT = 0x03F8;
static constexpr ioport_t COM2_PORT = 0x02F8;

#ifdef DEBUG
static void InterruptCOM()
{
   __asm
   {
      int 0x24;
   }
}
#endif

class SerialDriver : public IDeviceDriver
{
   public:
   SerialDriver(ioport_t comPort);

   result_t Load(_IN_HANDLE_ handle_t driverHandle);

   result_t Write(_IN_BYTE_ARR_ const uint8_t* buffer, _IN_SIZE_ size_t bufLen, _IN_SIZE_ size_t byteOffset);

   result_t Read(_IN_SIZE_ size_t bufLen, _OUT_BYTE_ARR_ uint8_t* buffer, _IN_SIZE_ size_t byteOffset);

   result_t Unload();

   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p);

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p);

   result_t Interrupt(irqnumber_t irq);

   private:
   static constexpr size_t _BUFFER_SIZE = 64;

   void _hwWrite();

   uint8_t _buffer[_BUFFER_SIZE];
   uint8_t* _buffHead;
   uint8_t* _buffTail;
   ioport_t _port;
};

extern result_t SerialDeviceHandler(irqnumber_t irqNumber, uint32_t error, void* data_p);