// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_SETUP_H
#define __RVX_SETUP_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// @brief Structure containing the setup configuration for RVX.
typedef struct RVX_ALIGNED RvxSetup
{
  // Frequency (in Hertz) of the clock signal driving RVX
  uint32_t clock_frequency;
} RvxSetup;

/**
 * @brief Initialize RVX with the provided setup configuration.
 *
 * This function initializes the RVX hardware based on the provided setup configuration, including setting the clock
 * frequency, trap handler, and interrupt mode (direct or vectored).
 *
 * This function must be called before calling any other functions to ensure the RVX hardware is properly initialized.
 *
 * @param rvx_setup Pointer to the setup configuration structure for RVX.
 */
static inline void rvx_init(const RvxSetup *rvx_setup)
{
  if (!rvx_setup)
    return;

  // Save the clock frequency in the Clock Frequency CSR
  RVX_CSR_WRITE(RVX_CSR_CLOCK_FREQUENCY_ADDR, rvx_setup->clock_frequency);
}

#endif // __RVX_SETUP_H