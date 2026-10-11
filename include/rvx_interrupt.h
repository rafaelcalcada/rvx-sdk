// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_INTERRUPT_H
#define __RVX_INTERRUPT_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_macros.h"
#include "rvx_plic.h"

/**
 * @brief Enable all M-mode interrupts (external, timer, and software).
 *
 * A call to this function also sets the global M-mode interrupt enable bit in the MSTATUSL CSR.
 *
 * Example usage:
 * ```c
 * // Enable all M-mode interrupts
 * rvx_interrupt_enable_m();
 * ```
 *
 */
static inline void rvx_interrupt_enable_m()
{
  RVX_CSR_SET(RVX_CSR_MIE_ADDR, 0x888);
  RVX_CSR_SET(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

/**
 * @brief Disable all M-mode interrupts (external, timer, and software).
 *
 * A call to this function also clears the global M-mode interrupt enable bit in the MSTATUSL CSR.
 *
 * Example usage:
 * ```c
 * // Disable all M-mode interrupts
 * rvx_interrupt_disable_m();
 * ```
 */
static inline void rvx_interrupt_disable_m()
{
  RVX_CSR_CLEAR(RVX_CSR_MIE_ADDR, 0x888);
  RVX_CSR_CLEAR(RVX_CSR_MSTATUSL_ADDR, RVX_CSR_MSTATUSL_MIE_BITMASK);
}

#endif // __RVX_INTERRUPT_H