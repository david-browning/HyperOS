#pragma once
#include "annotations.h"

struct bootdevice_t
{
   uint32_t BIOSDevice;
   uint32_t Partition;
   uint32_t SubPartition;
   bool IsEFI;
};