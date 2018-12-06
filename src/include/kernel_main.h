#pragma once

#include "annotations.h"
#include "kernel_multiboot.h"

#if defined(__cplusplus)
extern "C" 
#endif
LIB_EXPORT result_t kernel_main(multiboot_start_tag_t* mbstart_p);