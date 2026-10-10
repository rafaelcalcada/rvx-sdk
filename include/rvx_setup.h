// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef RVX_SETUP_H
#define RVX_SETUP_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_macros.h"

extern void rvx_trap_handler(void);

#define RVX_DEFAULT_TRAP_HANDLER &rvx_trap_handler

/// @brief Structure representing the setup configuration for RVX.
typedef struct RVX_ALIGNED RvxSetup
{
  uint32_t clock_frequency;   ///< Frequency (in Hz) of the clock signal driving RVX
  void (*trap_handler)(void); ///< Address of the trap handler function
} RvxSetup;

/**
 * @brief Initialize RVX with the provided setup configuration.
 *
 * This function must be called before anything else to ensure the RVX hardware is properly initialized.
 *
 * @param rvx_setup Pointer to the setup configuration structure for RVX.
 */
static inline void rvx_init(const RvxSetup *rvx_setup)
{
  if (!rvx_setup)
    return;

  if (rvx_setup->clock_frequency)
    RVX_CSR_WRITE(RVX_CSR_CLOCK_FREQUENCY_ADDR, rvx_setup->clock_frequency);

  if (rvx_setup->trap_handler)
    RVX_CSR_WRITE(RVX_CSR_MTVEC_ADDR, (uint32_t)rvx_setup->trap_handler & ~0x3);
}

static inline uint32_t rvx_get_clock_frequency(void)
{
  uint32_t clock_frequency;
  RVX_CSR_READ(RVX_CSR_CLOCK_FREQUENCY_ADDR, clock_frequency);
  return clock_frequency;
}

#endif // RVX_SETUP_H