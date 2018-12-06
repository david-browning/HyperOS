#include "drvr_serial.h"
#include "mem.h"

SerialDriver::SerialDriver(ioport_t comMem)
{
   _port = comMem;
}

result_t SerialDriver::Load(_IN_HANDLE_ handle_t driverHandle)
{
   //Clear out the buffer
   MemorySet<uint8_t>(this->_buffer, 0, this->_BUFFER_SIZE);
   _buffHead = _buffTail = _buffer;

   IOWriteToPort<uint8_t>(_port + 1, 0x00);
   IOWriteToPort<uint8_t>(_port + 3, 0x80);
   IOWriteToPort<uint8_t>(_port + 0, 0x03);
   IOWriteToPort<uint8_t>(_port + 1, 0x00);
   IOWriteToPort<uint8_t>(_port + 3, 0x03);
   IOWriteToPort<uint8_t>(_port + 2, 0xC7);
   IOWriteToPort<uint8_t>(_port + 4, 0x0B);

   IOWriteToPort<uint8_t>(_port + 3, 0x00);
   IOWriteToPort<uint8_t>(_port + 1, 0b0000'0010);

   _isLoaded = true;
   return RES_SUCCESS;
}

result_t SerialDriver::Write(_IN_BYTE_ARR_ const uint8_t* buffer,
                             _IN_SIZE_ size_t bufLen,
                             _IN_SIZE_ size_t byteOffset)
{
   IDTManager::DisableInterrupts();
   result_t res = RES_SUCCESS;
   size_t bytesToWrite = bufLen;

   if (_buffTail + bufLen >= &_buffer[_BUFFER_SIZE - 1])
   {
      bytesToWrite = &_buffer[_BUFFER_SIZE - 1] - _buffTail;
   }

   MemoryCopy<uint8_t>(_buffTail, buffer, bytesToWrite);
   _buffTail += bytesToWrite;

   if (_buffTail >= &_buffer[_BUFFER_SIZE - 1])
   {
      _buffTail = &_buffer[0];
   }

   _hwWrite();

   IDTManager::EnableInterrupts();
   return res;
}

result_t SerialDriver::Read(_IN_SIZE_ size_t bufLen,
                            _OUT_BYTE_ARR_ uint8_t* buffer,
                            _IN_SIZE_ size_t byteOffset)
{
   return RES_NOT_AVAILABLE;
}

result_t SerialDriver::Unload()
{
   _isLoaded = false;
   return RES_SUCCESS;
}

result_t SerialDriver::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   return RES_NOT_IMPLEMENTED;
}

result_t SerialDriver::SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   return RES_NOT_IMPLEMENTED;
}

result_t SerialDriver::Interrupt(irqnumber_t irq)
{
   //Read IIR to determine cause of interrupt
   uint8_t iir = IOReadFromPort<uint8_t>(_port + 2);

   if (iir & 0b0000'0010)
   {
      if (_buffHead != _buffTail)
      {
         //Write next character to TX buffer.
         IOWriteToPort<uint8_t>(_port, *_buffHead);

         //Increment consumer buffer pointer.
         if (++_buffHead >= &_buffer[_BUFFER_SIZE - 1])
         {
            int x = 0;
            _buffHead = &_buffer[0];
         }
      }
   }

   return RES_SUCCESS;
}

void SerialDriver::_hwWrite()
{
   //Read state
   uint8_t lineReg = IOReadFromPort<uint8_t>(_port + 5);

   //If state is idle
   if ((lineReg & 0b0110'0000) != 0)
   {
      //Set state to not idle.


      //Write next character to TX buffer.
      IOWriteToPort<uint8_t>(_port, *_buffHead);

      //Increment consumer buffer pointer.
      if (++_buffHead >= &_buffer[_BUFFER_SIZE - 1])
      {
         int x = 0;
         _buffHead = &_buffer[0];
      }
   }
   else
   {
      //Check if the TX buffer is empty. If so, set state to idle.
      if (IOReadFromPort<uint8_t>(_port + 5) & 0x20)
      {

      }
   }
}

result_t SerialDeviceHandler(irqnumber_t irqNumber, uint32_t error, void* data_p)
{
   SerialDriver* sdp = (SerialDriver*)data_p;
   return sdp->Interrupt(irqNumber);
}
