#pragma once

#include "annotations.h"
#include "kernel_interrupt.h"

typedef uint32_t syscallnumber_t;

typedef result_t(*kernel_system_call_function)(int unused1,
                                               int unused2,
                                               _IN_INT_ syscallnumber_t num);

extern result_t MakeSystemCall(_IN_INT_ syscallnumber_t num);

