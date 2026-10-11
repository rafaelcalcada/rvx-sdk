// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_H
#define __RVX_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_cause.h"
#include "rvx_csr.h"
#include "rvx_gpio.h"
#include "rvx_i2c.h"
#include "rvx_interrupt.h"
#include "rvx_macros.h"
#include "rvx_plic.h"
#include "rvx_setup.h"
#include "rvx_spi.h"
#include "rvx_timer.h"
#include "rvx_uart.h"

/// @name RVX Memory Map
/// @{
#define RVX_UART0_BASE ((RvxUart *)0x40000000U)   ///< Base address of UART0 controller register block.
#define RVX_TIMER0_BASE ((RvxTimer *)0x40001000U) ///< Base address of TIMER0 controller register block.
#define RVX_GPIO0_BASE ((RvxGpio *)0x40002000U)   ///< Base address of GPIO0 controller register block.
#define RVX_SPI0_BASE ((RvxSpi *)0x40003000U)     ///< Base address of SPI0 controller register block.
#define RVX_I2C0_BASE ((RvxI2c *)0x40004000U)     ///< Base address of I2C0 controller register block.
#define RVX_PLIC0_BASE ((RvxPlic *)0x40005000U)   ///< Base address of PLIC0 controller register block.
/// @}

#define RVX_PLIC0_SOURCE_UART0 1U  ///< Source ID for the UART0 interrupt.
#define RVX_PLIC0_SOURCE_TIMER0 2U ///< Source ID for the TIMER0 interrupt.

#endif // __RVX_H