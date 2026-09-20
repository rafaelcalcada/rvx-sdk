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
typedef enum RvxTrapCauseCode
{
  // Exception codes
  RVX_TRAP_CAUSE_INSTRUCTION_ADDRESS_MISALIGNED = 0U, ///< Trap cause: Instruction address misaligned.
  RVX_TRAP_CAUSE_INSTRUCTION_ACCESS_FAULT = 1U,       ///< Trap cause: Instruction access fault.
  RVX_TRAP_CAUSE_ILLEGAL_INSTRUCTION = 2U,            ///< Trap cause: Illegal instruction.
  RVX_TRAP_CAUSE_BREAKPOINT = 3U,                     ///< Trap cause: Breakpoint exception.
  RVX_TRAP_CAUSE_LOAD_ADDRESS_MISALIGNED = 4U,        ///< Trap cause: Load address misaligned.
  RVX_TRAP_CAUSE_LOAD_ACCESS_FAULT = 5U,              ///< Trap cause: Load access fault.
  RVX_TRAP_CAUSE_STORE_AMO_ADDRESS_MISALIGNED = 6U,   ///< Trap cause: Store/AMO address misaligned.
  RVX_TRAP_CAUSE_STORE_AMO_ACCESS_FAULT = 7U,         ///< Trap cause: Store/AMO access fault.
  RVX_TRAP_CAUSE_ENVIRONMENT_CALL_FROM_U_MODE = 8U,   ///< Trap cause: Environment call from U-mode.
  RVX_TRAP_CAUSE_ENVIRONMENT_CALL_FROM_S_MODE = 9U,   ///< Trap cause: Environment call from S-mode.
  RVX_TRAP_CAUSE_ENVIRONMENT_CALL_FROM_M_MODE = 11U,  ///< Trap cause: Environment call from M-mode.
  RVX_TRAP_CAUSE_INSTRUCTION_PAGE_FAULT = 12U,        ///< Trap cause: Instruction page fault.
  RVX_TRAP_CAUSE_LOAD_PAGE_FAULT = 13U,               ///< Trap cause: Load page fault.
  RVX_TRAP_CAUSE_STORE_AMO_PAGE_FAULT = 15U,          ///< Trap cause: Store/AMO page fault.
  RVX_TRAP_CAUSE_DOUBLE_TRAP = 16U,                   ///< Trap cause: Double trap.
  RVX_TRAP_CAUSE_SOFTWARE_CHECK = 18U,                ///< Trap cause: Software check.
  RVX_TRAP_CAUSE_HARDWARE_ERROR = 19U,                ///< Trap cause: Hardware error.

  // Interrupt codes (MSB set)
  RVX_TRAP_CAUSE_SOFTWARE_IRQ_S = (int)((1U << 31) | 1U),        ///< Trap cause: S-mode Software Interrupt.
  RVX_TRAP_CAUSE_SOFTWARE_IRQ_M = (int)((1U << 31) | 3U),        ///< Trap cause: M-mode Software Interrupt.
  RVX_TRAP_CAUSE_TIMER_IRQ_S = (int)((1U << 31) | 5U),           ///< Trap cause: S-mode Timer Interrupt.
  RVX_TRAP_CAUSE_TIMER_IRQ_M = (int)((1U << 31) | 7U),           ///< Trap cause: M-mode Timer Interrupt.
  RVX_TRAP_CAUSE_EXTERNAL_IRQ_S = (int)((1U << 31) | 9U),        ///< Trap cause: S-mode External Interrupt.
  RVX_TRAP_CAUSE_EXTERNAL_IRQ_M = (int)((1U << 31) | 11U),       ///< Trap cause: M-mode External Interrupt.
  RVX_TRAP_CAUSE_COUNTER_OVERFLOW_IRQ = (int)((1U << 31) | 13U), ///< Trap cause: Counter Overflow Interrupt.
} RvxTrapCauseCode;

/**
 * @brief Read the cause of the trap from the MCAUSE CSR.
 *
 * @return The cause code (see RISC-V privileged ISA specification) as `int`.
 */
static inline int rvx_trap_get_cause_m(void)
{
  int cause;
  RVX_CSR_READ(RVX_CSR_MCAUSE_ADDR, cause);
  return cause;
}

/**
 * @brief Return from an M-mode trap by executing the MRET instruction.
 */
static inline void rvx_trap_return_m(void)
{
  __asm__ volatile("mret");
}

/**
 * @brief Returns `true` if the given cause code represents an exception, `false` otherwise.
 *
 * @param cause The cause code to check.
 * @return true
 * @return false
 */
static inline bool rvx_trap_cause_is_exception(const int cause)
{
  return (cause & (1U << 31)) == 0;
}

/**
 * @brief Returns `true` if the given cause code represents an interrupt, `false` otherwise.
 *
 * @param cause The cause code to check.
 * @return true
 * @return false
 */
static inline bool rvx_trap_cause_is_interrupt(const int cause)
{
  return (cause & (1U << 31)) != 0;
}

#endif // __RVX_TRAP_H