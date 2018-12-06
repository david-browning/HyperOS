#include "kernel_pic.h"
#include "kernel_cpu.h"

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

static constexpr ioport_t PIC1_COMMAND_PORT = 0x0020;
static constexpr ioport_t PIC2_COMMAND_PORT = 0x00A0;
static constexpr ioport_t PIC1_DATA_PORT = 0x0021;
static constexpr ioport_t PIC2_DATA_PORT = 0x00A1;
static constexpr uint8_t PIC_EOI = 0x20;

void PICDriver::SendEOI(uint8_t irq)
{
   if (irq >= 8)
   {
      IOWriteToPort<uint8_t>(PIC2_COMMAND_PORT, PIC_EOI);
   }

   IOWriteToPort<uint8_t>(PIC1_COMMAND_PORT, PIC_EOI);
}

void PICDriver::Remap(uint8_t offset1, uint8_t offset2)
{
   _off1 = offset1;
   _off2 = offset2;
   uint8_t pic1Mask = IOReadFromPort<uint8_t>(PIC1_DATA_PORT);
   uint8_t pic2Mask = IOReadFromPort<uint8_t>(PIC2_DATA_PORT);

   //Start initialization sequence
   IOWriteToPort<uint8_t>(PIC1_COMMAND_PORT, ICW1_INIT + ICW1_ICW4);
   IOWait();
   IOWriteToPort<uint8_t>(PIC2_COMMAND_PORT, ICW1_INIT + ICW1_ICW4);
   IOWait();
   IOWriteToPort<uint8_t>(PIC1_DATA_PORT, offset1);
   IOWait();
   IOWriteToPort<uint8_t>(PIC2_DATA_PORT, offset2);
   IOWait();
   IOWriteToPort<uint8_t>(PIC1_DATA_PORT, 4);
   IOWait();
   IOWriteToPort<uint8_t>(PIC2_DATA_PORT, 2);
   IOWait();

   IOWriteToPort<uint8_t>(PIC1_DATA_PORT, ICW4_8086);
   IOWait();
   IOWriteToPort<uint8_t>(PIC2_DATA_PORT, ICW4_8086);
   IOWait();

   IOWriteToPort<uint8_t>(PIC1_DATA_PORT, 0xFF);
   IOWait();
   IOWriteToPort<uint8_t>(PIC2_DATA_PORT, 0xFF);
   IOWait();
}

KERNEL_FUNC result_t PICDriver::EnableAPIC()
{
   return RES_UNSUPPORTED;

   if (!APICSupported())
   {
      return RES_NOT_AVAILABLE;
   }

   uint32_t eax, edx;
   CPUGetMSR(0x1B, &eax, &edx);
   uintptr_t base = (eax & 0xfffff000);

   edx = 0;
   eax = (base & 0xfffff000) | 0x800;

   CPUSetMSR(0x1B, eax, edx);




   return RES_SUCCESS;
}

void PICDriver::SetMask(uint8_t irqLine)
{
   ioport_t picPort = PIC1_DATA_PORT;
   if (irqLine >= 8)
   {
      picPort = PIC2_DATA_PORT;
      irqLine -= 8;
   }

   uint8_t mask = uint8_t(IOReadFromPort<uint8_t>(picPort) | (1 << irqLine));
   IOWriteToPort(picPort, mask);
}

void PICDriver::ClearMask(uint8_t irqLine)
{
   ioport_t picPort = PIC1_DATA_PORT;
   if (irqLine >= 8)
   {
      picPort = PIC2_DATA_PORT;
      irqLine -= 8;
   }

   uint8_t mask = uint8_t(IOReadFromPort<uint8_t>(picPort) & ~(1 << irqLine));
   IOWriteToPort(picPort, mask);
}

uint16_t PICDriver::_getreg(uint8_t ocw3)
{
   IOWriteToPort<uint8_t>(PIC1_COMMAND_PORT, ocw3);
   IOWriteToPort<uint8_t>(PIC2_COMMAND_PORT, ocw3);

   return uint16_t(uint16_t(IOReadFromPort<uint8_t>(PIC2_COMMAND_PORT << 8)) |
                   uint16_t(IOReadFromPort<uint8_t>(PIC1_COMMAND_PORT)));
}

bool APICSupported()
{
   uint32_t eax, ebx, ecx, edx;
   CPUID(1, &eax, &edx);
   return edx & 0x200;
}
