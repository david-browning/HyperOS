#pragma once
#include "drvr_vga.h"

#if defined(__cplusplus)
extern "C"
#endif
extern void WaitForDebugger(_IN_POINTER_ VGADriver* const vgaDrvr = nullptr);