// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_IRQ_H
#define __RVX_IRQ_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_macros.h"

/// @name Bit masks for MIE and MIP interrupt enable/pending bits.
/// @{
#define RVX_IRQ_SOFTWARE_BITMASK (1U << 3U)         ///< Bitmask for the Software Interrupt enable/pending bit.
#define RVX_IRQ_TIMER_BITMASK (1U << 7U)            ///< Bitmask for the Timer Interrupt enable/pending bit.
#define RVX_IRQ_EXTERNAL_BITMASK (1U << 11U)        ///< Bitmask for the External Interrupt enable/pending bit.
#define RVX_IRQ_UART_BITMASK (1U << 16U)            ///< Bitmask for the UART Interrupt enable/pending bit.
#define RVX_IRQ_I2C_BITMASK (1U << 17U)             ///< Bitmask for the I2C Interrupt enable/pending bit.
#define RVX_IRQ_FAST_BITMASK(n) (1U << (24U + (n))) ///< Bitmask for the Fast Interrupt `n` enable/pending bit.
/// @}

/// Define a M-mode trap handler function.
#define RVX_TRAP_HANDLER_M(vector) __attribute__((interrupt("machine"))) void vector(void)

/// @brief Interrupt modes in the RISC-V architecture.
typedef enum RvxIrqMode
{
  RVX_IRQ_MODE_DIRECT = 0,   ///< Direct interrupt mode.
  RVX_IRQ_MODE_VECTORED = 1, ///< Vectored interrupt mode.
} RvxIrqMode;

/// @brief Standard RISC-V trap cause codes for the MCAUSE CSR.
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

  // Interrupt codes (MSB set)
  RVX_TRAP_CAUSE_SOFTWARE_IRQ_M = (int)((1U << 31) | 3U), ///< Trap cause: M-mode Software Interrupt.
  RVX_TRAP_CAUSE_EXTERNAL_IRQ_M = (int)((1U << 31) | 7U), ///< Trap cause: M-mode External Interrupt.
  RVX_TRAP_CAUSE_TIMER_IRQ_M = (int)((1U << 31) | 11U),   ///< Trap cause: M-mode Timer Interrupt.
  RVX_TRAP_CAUSE_UART_IRQ_M = (int)((1U << 31) | 16U),    ///< Trap cause: M-mode UART Interrupt
  RVX_TRAP_CAUSE_I2C_IRQ_M = (int)((1U << 31) | 17U),     ///< Trap cause: M-mode I2C Interrupt
  RVX_TRAP_CAUSE_FAST_0_IRQ_M = (int)((1U << 31) | 24U),  ///< Trap cause: M-mode Fast-Interrupt 0
  RVX_TRAP_CAUSE_FAST_1_IRQ_M = (int)((1U << 31) | 25U),  ///< Trap cause: M-mode Fast-Interrupt 1
  RVX_TRAP_CAUSE_FAST_2_IRQ_M = (int)((1U << 31) | 26U),  ///< Trap cause: M-mode Fast-Interrupt 2
  RVX_TRAP_CAUSE_FAST_3_IRQ_M = (int)((1U << 31) | 27U),  ///< Trap cause: M-mode Fast-Interrupt 3
  RVX_TRAP_CAUSE_FAST_4_IRQ_M = (int)((1U << 31) | 28U),  ///< Trap cause: M-mode Fast-Interrupt 4
  RVX_TRAP_CAUSE_FAST_5_IRQ_M = (int)((1U << 31) | 29U),  ///< Trap cause: M-mode Fast-Interrupt 5
  RVX_TRAP_CAUSE_FAST_6_IRQ_M = (int)((1U << 31) | 30U),  ///< Trap cause: M-mode Fast-Interrupt 6
  RVX_TRAP_CAUSE_FAST_7_IRQ_M = (int)((1U << 31) | 31U)   ///< Trap cause: M-mode Fast-Interrupt 7
} RvxTrapCauseCode;

/**
 * @brief Enable specific M-mode interrupts by setting the corresponding bits in the MIE CSR.
 *
 * The provided `bitmask` will be or'ed with the current value of the MIE CSR, enabling the specified interrupts.
 *
 * Example usage:
 * ```c
 * // Enable timer and UART interrupts
 * rvx_irq_enable_m(RVX_IRQ_TIMER_BITMASK | RVX_IRQ_UART_BITMASK);
 * ```
 *
 * @note This function does not globally enable interrupts at the M-mode privilege level. To do that, you must call
 * `rvx_irq_enable_global_m`.
 *
 * @param bitmask Bit mask indicating which interrupt(s) to enable.
 */
static inline void rvx_irq_enable_m(uint32_t bitmask)
{
  RVX_CSR_SET(RVX_CSR_MIE_ADDR, bitmask);
}

/**
 * @brief Globally enable interrupts in M-mode by setting the MIE bit in the MSTATUSL CSR.
 *
 * Example usage:
 * ```c
 * // Globally enable interrupts in M-mode
 * rvx_irq_enable_global_m();
 * ```
 *
 * @note A given M-mode interrupt will only cause a trap if it is also enabled in the MIE CSR. You can call
 * `rvx_irq_enable_m()` to enable specific M-mode interrupts.
 *
 */
static inline void rvx_irq_enable_global_m()
{
  RVX_CSR_SET(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

/**
 * @brief Disable a specific M-mode interrupt by clearing the corresponding bit in the MIE CSR.
 *
 * The provided `bitmask` will be inverted and and'ed with the current value of the MIE CSR, disabling the specified
 * interrupts.
 *
 * Example usage:
 * ```c
 * // Disable timer and UART interrupts
 * rvx_irq_disable_m(RVX_IRQ_TIMER_BITMASK | RVX_IRQ_UART_BITMASK);
 * ```
 *
 * @note This function does not globally disable interrupts at the M-mode privilege level. To do that, you must call
 * `rvx_irq_disable_global_m`.
 */
static inline void rvx_irq_disable_m(uint32_t bitmask)
{
  RVX_CSR_CLEAR(RVX_CSR_MIE_ADDR, bitmask);
}

/**
 * @brief Globally disable interrupts in M-mode by clearing the MIE bit in the MSTATUSL CSR.
 *
 * Example usage:
 * ```c
 * // Globally disable interrupts in M-mode
 * rvx_irq_disable_global_m();
 * ```
 *
 * @note Globally disabling interrupts will not clear any specific interrupt enable bits, but will prevent interrupts
 * from causing traps to the corresponding privilege level until interrupts are globally re-enabled.
 */
static inline void rvx_irq_disable_global_m()
{
  RVX_CSR_CLEAR(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

/**
 * @brief Set the interrupt mode for M-mode by setting the MODE field of the MTVEC CSR.
 *
 * The `irq_mode` can be set to either `RVX_IRQ_MODE_DIRECT` or `RVX_IRQ_MODE_VECTORED`. In direct mode, all M-mode
 * interrupts will cause a trap to the same address specified in the BASE field of the MTVEC CSR. In vectored mode, each
 * M-mode interrupt will cause a trap to a different address calculated from the BASE field and the interrupt cause
 * code as follows:
 *
 * `trap_address = BASE + 4 * cause_code` (where `cause_code` is the interrupt cause code with the MSB cleared).
 *
 * @param irq_mode The desired interrupt mode to set for M-mode interrupts.
 */
static inline void rvx_irq_set_mode_m(RvxIrqMode irq_mode)
{
  if (irq_mode == RVX_IRQ_MODE_VECTORED)
  {
    uint32_t base;
    RVX_CSR_READ(RVX_CSR_MTVEC_ADDR, base);
    base = base & RVX_CSR_MTVEC_BASE_BITMASK;
    RVX_CSR_WRITE(RVX_CSR_MTVEC_ADDR, base | 1);
  }
  else if (irq_mode == RVX_IRQ_MODE_DIRECT)
  {
    RVX_CSR_CLEAR(RVX_CSR_MTVEC_ADDR, RVX_CSR_MTVEC_MODE_BITMASK);
  }
}

#endif // __RVX_IRQ_H