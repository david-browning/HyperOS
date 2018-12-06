#pragma once
#include "annotations.h"

static constexpr size_t MODULE_NAME_MAX_LEN = 128;

struct module_t
{
   //Start address of the boot module.
   uintptr_t StartAddress;

   //End address of the boot module.
   size_t Size;

   char Name[MODULE_NAME_MAX_LEN];
};