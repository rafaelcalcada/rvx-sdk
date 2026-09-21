// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_IRQ_H
#define __RVX_IRQ_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_macros.h"
#include "rvx_plic.h"

/// @name Bit masks for MIE and MIP interrupt enable/pending bits.
/// @{
#define RVX_IRQ_SOFTWARE_BITMASK (1U << 3U)  ///< Bitmask for the Software Interrupt enable/pending bit.
#define RVX_IRQ_TIMER_BITMASK (1U << 7U)     ///< Bitmask for the Timer Interrupt enable/pending bit.
#define RVX_IRQ_EXTERNAL_BITMASK (1U << 11U) ///< Bitmask for the External Interrupt enable/pending bit.
/// @}

/// Mark a function as an M-mode interrupt handler, which adds a prologue and epilogue that properly save and restore
/// the context of the interrupted code.
#define RVX_IRQ_HANDLER_M __attribute__((interrupt("machine")))

/**
 * @brief Enable specific M-mode interrupts by setting the corresponding bits in the MIE CSR.
 *
 * The provided `bitmask` is or'ed with the current value of the MIE CSR, enabling the specified interrupts.
 *
 * Example usage:
 * ```c
 * // Enable timer and external interrupts
 * rvx_irq_enable_m(RVX_IRQ_TIMER_BITMASK | RVX_IRQ_EXTERNAL_BITMASK);
 * ```
 *
 * @note This function only sets the per-interrupt enable bits in the MIE CSR. It does not globally enable interrupts at
 * the M-mode privilege level. To do that, you must call `rvx_irq_global_enable_m()`.
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
 * rvx_irq_global_enable_m();
 * ```
 *
 * @note A given M-mode interrupt will only cause a trap if its specific enable bit is also set in the MIE CSR. Call
 * `rvx_irq_enable_m()` to set the per-interrupt enable bits in the MIE CSR.
 *
 */
static inline void rvx_irq_global_enable_m()
{
  RVX_CSR_SET(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

/**
 * @brief Disable specific M-mode interrupts by clearing the corresponding bit in the MIE CSR.
 *
 * The provided `bitmask` is inverted and and'ed with the current value of the MIE CSR, disabling the specified
 * interrupts.
 *
 * Example usage:
 * ```c
 * // Disable timer and external interrupts
 * rvx_irq_disable_m(RVX_IRQ_TIMER_BITMASK | RVX_IRQ_EXTERNAL_BITMASK);
 * ```
 *
 * @note This function only clears the per-interrupt enable bits in the MIE CSR. It does not globally disable interrupts
 * at the M-mode privilege level. To do that, you must call `rvx_irq_global_disable_m()`.
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
 * rvx_irq_global_disable_m();
 * ```
 *
 * @note Globally disabling interrupts will not clear any specific interrupt enable bits, but will prevent interrupts
 * from causing traps to the corresponding privilege level until interrupts are globally re-enabled.
 */
static inline void rvx_irq_global_disable_m()
{
  RVX_CSR_CLEAR(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

#endif // __RVX_IRQ_H