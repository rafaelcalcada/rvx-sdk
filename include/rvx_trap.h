// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_TRAP_H
#define __RVX_TRAP_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_macros.h"

/// @brief Standard RISC-V trap cause codes.
// Exception codes
#define RVX_TRAP_CAUSE_INSTRUCTION_ADDRESS_MISALIGNED 0U ///< Trap cause: Instruction address misaligned.
#define RVX_TRAP_CAUSE_INSTRUCTION_ACCESS_FAULT 1U       ///< Trap cause: Instruction access fault.
#define RVX_TRAP_CAUSE_ILLEGAL_INSTRUCTION 2U            ///< Trap cause: Illegal instruction.
#define RVX_TRAP_CAUSE_BREAKPOINT 3U                     ///< Trap cause: Breakpoint exception.
#define RVX_TRAP_CAUSE_LOAD_ADDRESS_MISALIGNED 4U        ///< Trap cause: Load address misaligned.
#define RVX_TRAP_CAUSE_LOAD_ACCESS_FAULT 5U              ///< Trap cause: Load access fault.
#define RVX_TRAP_CAUSE_STORE_AMO_ADDRESS_MISALIGNED 6U   ///< Trap cause: Store/AMO address misaligned.
#define RVX_TRAP_CAUSE_STORE_AMO_ACCESS_FAULT 7U         ///< Trap cause: Store/AMO access fault.
#define RVX_TRAP_CAUSE_ENVIRONMENT_CALL_FROM_U_MODE 8U   ///< Trap cause: Environment call from U-mode.
#define RVX_TRAP_CAUSE_ENVIRONMENT_CALL_FROM_S_MODE 9U   ///< Trap cause: Environment call from S-mode.
#define RVX_TRAP_CAUSE_ENVIRONMENT_CALL_FROM_M_MODE 11U  ///< Trap cause: Environment call from M-mode.
#define RVX_TRAP_CAUSE_INSTRUCTION_PAGE_FAULT 12U        ///< Trap cause: Instruction page fault.
#define RVX_TRAP_CAUSE_LOAD_PAGE_FAULT 13U               ///< Trap cause: Load page fault.
#define RVX_TRAP_CAUSE_STORE_AMO_PAGE_FAULT 15U          ///< Trap cause: Store/AMO page fault.
#define RVX_TRAP_CAUSE_DOUBLE_TRAP 16U                   ///< Trap cause: Double trap.
#define RVX_TRAP_CAUSE_SOFTWARE_CHECK 18U                ///< Trap cause: Software check.
#define RVX_TRAP_CAUSE_HARDWARE_ERROR 19U                ///< Trap cause: Hardware error.

// Interrupt codes (MSB set)
#define RVX_TRAP_CAUSE_SOFTWARE_IRQ_S ((uint32_t)((1U << 31) | 1U))        ///< Trap cause: S-mode Software Interrupt.
#define RVX_TRAP_CAUSE_SOFTWARE_IRQ_M ((uint32_t)((1U << 31) | 3U))        ///< Trap cause: M-mode Software Interrupt.
#define RVX_TRAP_CAUSE_TIMER_IRQ_S ((uint32_t)((1U << 31) | 5U))           ///< Trap cause: S-mode Timer Interrupt.
#define RVX_TRAP_CAUSE_TIMER_IRQ_M ((uint32_t)((1U << 31) | 7U))           ///< Trap cause: M-mode Timer Interrupt.
#define RVX_TRAP_CAUSE_EXTERNAL_IRQ_S ((uint32_t)((1U << 31) | 9U))        ///< Trap cause: S-mode External Interrupt.
#define RVX_TRAP_CAUSE_EXTERNAL_IRQ_M ((uint32_t)((1U << 31) | 11U))       ///< Trap cause: M-mode External Interrupt.
#define RVX_TRAP_CAUSE_COUNTER_OVERFLOW_IRQ ((uint32_t)((1U << 31) | 13U)) ///< Trap cause: Counter Overflow Interrupt.

#endif // __RVX_TRAP_H