#pragma once
#include "annotations.h"

typedef int irqnumber_t;

static constexpr irqnumber_t INT_DIV_ERROR = 0;
static constexpr irqnumber_t INT_DEBUG_EXCEP = 1;
static constexpr irqnumber_t INT_NON_MASKABLE_INTERRUPT = 2;
static constexpr irqnumber_t INT_BREAKPOINT = 3;
static constexpr irqnumber_t INT_OVERFLOW = 4;
static constexpr irqnumber_t INT_BOUND_RANGE_EXCEPTION = 5;
static constexpr irqnumber_t INT_INVALID_OPCODE = 6;
static constexpr irqnumber_t INT_NO_DEVICE = 7;
static constexpr irqnumber_t INT_DOUBLE_FAULT = 8;
static constexpr irqnumber_t INT_COPROCESSOR_OVERRUN = 9;
static constexpr irqnumber_t INT_INVALID_TSS = 10;
static constexpr irqnumber_t INT_SEGMENT_NOT_PRESENT = 11;
static constexpr irqnumber_t INT_STACK_SEGMENT_FAULT = 12;
static constexpr irqnumber_t INT_GENERAL_PROTECTION = 13;
static constexpr irqnumber_t INT_PAGE_FAULT = 14;
static constexpr irqnumber_t INT_RESERVED = 15;
static constexpr irqnumber_t INT_FPU_ERROR = 16;
static constexpr irqnumber_t INT_ALIGNMENT_CHECK = 17;
static constexpr irqnumber_t INT_MACHINE_CHECK = 18;
static constexpr irqnumber_t INT_SIMD_EXCEPTION = 19;
static constexpr irqnumber_t INT_VIRTUALIZATION_EXCEPTION = 20;

static constexpr irqnumber_t INT_SYSTEM_CALL = 0x69;
static constexpr irqnumber_t INT_PROC_YIELD = 0x70;
static constexpr irqnumber_t INT_PROC_EXIT = 0x71;
