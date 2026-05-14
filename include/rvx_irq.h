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
#define RVX_IRQ_FAST_BITMASK(n) (1U << (16U + (n))) ///< Bitmask for the Fast Interrupt `n` enable/pending bit.
/// @}

/// Define a M-mode interrupt handler function.
#define RVX_TRAP_HANDLER_M(vector) __attribute__((interrupt("machine"))) void vector(void)

/// @brief Privilege levels in the RISC-V architecture.
typedef enum RvxPrivilegeLevel
{
  RVX_PRIVILEGE_LEVEL_U = 0, ///< User mode privilege level.
  RVX_PRIVILEGE_LEVEL_S = 1, ///< Supervisor mode privilege level.
  RVX_PRIVILEGE_LEVEL_M = 3  ///< Machine mode privilege level.
} RvxPrivilegeLevel;

/// @brief Interrupt modes in the RISC-V architecture.
typedef enum RvxInterruptMode
{
  RVX_INTERRUPT_MODE_DIRECT = 0,   ///< Direct interrupt mode.
  RVX_INTERRUPT_MODE_VECTORED = 1, ///< Vectored interrupt mode.
} RvxInterruptMode;

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
  RVX_TRAP_CAUSE_SOFTWARE_IRQ = (int)((1U << 31) | 3U), ///< Trap cause: M-mode Software Interrupt.
  RVX_TRAP_CAUSE_EXTERNAL_IRQ = (int)((1U << 31) | 7U), ///< Trap cause: M-mode External Interrupt.
  RVX_TRAP_CAUSE_TIMER_IRQ = (int)((1U << 31) | 11U),   ///< Trap cause: M-mode Timer Interrupt.
  RVX_TRAP_CAUSE_FAST_IRQ_0 = (int)((1U << 31) | 16U),  ///< Trap cause: Fast-Interrupt 0
  RVX_TRAP_CAUSE_FAST_IRQ_1 = (int)((1U << 31) | 17U),  ///< Trap cause: Fast-Interrupt 1
  RVX_TRAP_CAUSE_FAST_IRQ_2 = (int)((1U << 31) | 18U),  ///< Trap cause: Fast-Interrupt 2
  RVX_TRAP_CAUSE_FAST_IRQ_3 = (int)((1U << 31) | 19U),  ///< Trap cause: Fast-Interrupt 3
  RVX_TRAP_CAUSE_FAST_IRQ_4 = (int)((1U << 31) | 20U),  ///< Trap cause: Fast-Interrupt 4
  RVX_TRAP_CAUSE_FAST_IRQ_5 = (int)((1U << 31) | 21U),  ///< Trap cause: Fast-Interrupt 5
  RVX_TRAP_CAUSE_FAST_IRQ_6 = (int)((1U << 31) | 22U),  ///< Trap cause: Fast-Interrupt 6
  RVX_TRAP_CAUSE_FAST_IRQ_7 = (int)((1U << 31) | 23U),  ///< Trap cause: Fast-Interrupt 7
  RVX_TRAP_CAUSE_FAST_IRQ_8 = (int)((1U << 31) | 24U),  ///< Trap cause: Fast-Interrupt 8
  RVX_TRAP_CAUSE_FAST_IRQ_9 = (int)((1U << 31) | 25U),  ///< Trap cause: Fast-Interrupt 9
  RVX_TRAP_CAUSE_FAST_IRQ_10 = (int)((1U << 31) | 26U), ///< Trap cause: Fast-Interrupt 10
  RVX_TRAP_CAUSE_FAST_IRQ_11 = (int)((1U << 31) | 27U), ///< Trap cause: Fast-Interrupt 11
  RVX_TRAP_CAUSE_FAST_IRQ_12 = (int)((1U << 31) | 28U), ///< Trap cause: Fast-Interrupt 12
  RVX_TRAP_CAUSE_FAST_IRQ_13 = (int)((1U << 31) | 29U), ///< Trap cause: Fast-Interrupt 13
  RVX_TRAP_CAUSE_FAST_IRQ_14 = (int)((1U << 31) | 30U), ///< Trap cause: Fast-Interrupt 14
  RVX_TRAP_CAUSE_FAST_IRQ_15 = (int)((1U << 31) | 31U)  ///< Trap cause: Fast-Interrupt 15
} RvxTrapCauseCode;

/**
 * @brief Enable a specific interrupt by setting the corresponding bit in the appropriate interrupt enable CSR.
 *
 * The `privilege_level` parameter specifies the privilege level at which to enable the interrupt(s). For RVX, only
 * machine-level interrupts are implemented, so `privilege_level` must be set to `RVX_PRIVILEGE_LEVEL_M`. If a different
 * privilege level is specified, no action is taken and no error is reported.
 *
 * The `bitmask` parameter can be a combination of the following values, depending on which interrupts you want to
 * enable:
 *
 * - `RVX_IRQ_SOFTWARE_BITMASK`: Enable software interrupts.
 *
 * - `RVX_IRQ_TIMER_BITMASK`: Enable timer interrupts.
 *
 * - `RVX_IRQ_EXTERNAL_BITMASK`: Enable external interrupts.
 *
 * - `RVX_IRQ_FAST_BITMASK(n)`: Enable the fast interrupt with index `n`
 *
 * Example usage:
 * ```c
 * // Enable timer interrupt and fast interrupt 0 in M-mode
 * rvx_irq_enable(RVX_PRIVILEGE_LEVEL_M, RVX_IRQ_TIMER_BITMASK | RVX_IRQ_FAST_BITMASK(0));
 * ```
 *
 * @note An interrupt will only cause a trap if the global interrupt enable bit for the corresponding privilege level is
 * also set in the Status CSR. This function does not modify the global interrupt enable bit. You must call
 * `rvx_irq_enable_global()` to globally enable interrupts at the desired privilege level.
 *
 * @param privilege_level The privilege level at which to enable the interrupt(s).
 * @param bitmask Bit mask indicating which interrupt(s) to enable.
 */
static inline void rvx_irq_enable(RvxPrivilegeLevel privilege_level, uint32_t bitmask)
{
  if (privilege_level == RVX_PRIVILEGE_LEVEL_M)
    RVX_CSR_SET(RVX_CSR_MIE_ADDR, bitmask);
}

/**
 * @brief Globally enable interrupts at the specified privilege level by setting the appropriate global interrupt enable
 * bit in the Status CSR.
 *
 * The `privilege_level` parameter specifies the privilege level at which to globally enable interrupts. For RVX, only
 * machine-level interrupts are implemented, so `privilege_level` must be set to `RVX_PRIVILEGE_LEVEL_M`. If a different
 * privilege level is specified, no action is taken and no error is reported.
 *
 * Example usage:
 * ```c
 * // Globally enable interrupts in M-mode
 * rvx_irq_enable_global(RVX_PRIVILEGE_LEVEL_M);
 * ```
 *
 * @note A given interrupt will only cause a trap if its specific enable bit is set in the appropriate interrupt enable
 * CSR. You can call `rvx_irq_enable()` to do this.
 *
 * @param privilege_level The privilege level at which to globally enable interrupts.
 */
static inline void rvx_irq_enable_global(RvxPrivilegeLevel privilege_level)
{
  if (privilege_level == RVX_PRIVILEGE_LEVEL_M)
    RVX_CSR_SET(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

/**
 * @brief Disable a specific interrupt by clearing the corresponding bit in the appropriate interrupt enable CSR.
 *
 * The `privilege_level` parameter specifies the privilege level at which to disable the interrupt(s). For RVX, only
 * machine-level interrupts are implemented, so `privilege_level` must be set to `RVX_PRIVILEGE_LEVEL_M`. If a different
 * privilege level is specified, no action is taken and no error is reported.
 *
 * The `bitmask` parameter can be a combination of the following values, depending on which interrupts you want to
 * disable:
 *
 * - `RVX_IRQ_SOFTWARE_BITMASK`: Disable software interrupts.
 *
 * - `RVX_IRQ_TIMER_BITMASK`: Disable timer interrupts.
 *
 * - `RVX_IRQ_EXTERNAL_BITMASK`: Disable external interrupts.
 *
 * - `RVX_IRQ_FAST_BITMASK(n)`: Disable the fast interrupt with index `n`
 *
 * Example usage:
 * ```c
 * // Disable timer interrupt and fast interrupt 0 in M-mode
 * rvx_irq_disable(RVX_PRIVILEGE_LEVEL_M, RVX_IRQ_TIMER_BITMASK | RVX_IRQ_FAST_BITMASK(0));
 * ```
 *
 * @note Disabling a specific interrupt does not affect the global interrupt enable bit for the corresponding privilege
 * level, which will remain unchanged.
 */
static inline void rvx_irq_disable(RvxPrivilegeLevel privilege_level, uint32_t bitmask)
{
  if (privilege_level == RVX_PRIVILEGE_LEVEL_M)
    RVX_CSR_CLEAR(RVX_CSR_MIE_ADDR, bitmask);
}

/**
 * @brief Globally disable interrupts at the specified privilege level by clearing the appropriate global interrupt
 * enable bit in the Status CSR.
 *
 * The `privilege_level` parameter specifies the privilege level at which to globally disable interrupts. For RVX, only
 * machine-level interrupts are implemented, so `privilege_level` must be set to `RVX_PRIVILEGE_LEVEL_M`. If a different
 * privilege level is specified, no action is taken and no error is reported.
 *
 * Example usage:
 * ```c
 * // Globally disable interrupts in M-mode
 * rvx_irq_disable_global(RVX_PRIVILEGE_LEVEL_M);
 * ```
 *
 * @note Globally disabling interrupts will not clear any specific interrupt enable bits, but will prevent interrupts
 * from causing traps to the corresponding privilege level until interrupts are globally re-enabled.
 */
static inline void rvx_irq_disable_global(RvxPrivilegeLevel privilege_level)
{
  if (privilege_level == RVX_PRIVILEGE_LEVEL_M)
    RVX_CSR_CLEAR(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

/**
 * @brief Set the interrupt mode for the specified privilege level by setting the MODE field of the appropriate Trap
 * Handler CSR.
 *
 * The `privilege_level` parameter specifies the privilege level to configure the interrupt mode for. For RVX, only
 * machine-level interrupts are implemented, so `privilege_level` must be set to `RVX_PRIVILEGE_LEVEL_M`. If a different
 * privilege level is specified, no action is taken and no error is reported.
 *
 * The `interrupt_mode` parameter specifies the desired interrupt mode. The following modes are supported:
 *
 * - `RVX_INTERRUPT_MODE_DIRECT`: In this mode, all interrupts cause a trap to the same handler address specified in the
 * BASE field of the Trap Handler CSR. The specific interrupt cause can be determined by reading the Trap Cause CSR
 * within the handler.
 *
 * - `RVX_INTERRUPT_MODE_VECTORED`: In this mode, interrupts cause a trap to an address computed as the sum of the BASE
 * field of the Trap Handler CSR and four times the interrupt cause code. This allows different interrupt causes to have
 * separate handlers.
 *
 * @param privilege_level The privilege level to configure the interrupt mode for.
 * @param interrupt_mode The desired interrupt mode to set.
 */
static inline void rvx_irq_set_mode(RvxPrivilegeLevel privilege_level, RvxInterruptMode interrupt_mode)
{
  if (privilege_level == RVX_PRIVILEGE_LEVEL_M)
  {
    if (interrupt_mode == RVX_INTERRUPT_MODE_VECTORED)
    {
      uint32_t base;
      RVX_CSR_READ(RVX_CSR_MTVEC_ADDR, base);
      base = base & RVX_CSR_MTVEC_BASE_BITMASK;
      RVX_CSR_WRITE(RVX_CSR_MTVEC_ADDR, base | 1);
    }
    else if (interrupt_mode == RVX_INTERRUPT_MODE_DIRECT)
    {
      RVX_CSR_CLEAR(RVX_CSR_MTVEC_ADDR, RVX_CSR_MTVEC_MODE_BITMASK);
    }
  }
}

/**
 * @brief Busy-wait indefinitely until an interrupt occurs.
 *
 * This function executes an infinite loop that can only be exited by an interrupt. It is typically
 * used in the main loop of a program that relies on interrupts for event handling, allowing the CPU
 * to remain idle until an interrupt is received.
 */
static inline void rvx_wait_for_interrupt()
{
  __asm__ volatile("j .");
}

#endif // __RVX_IRQ_H