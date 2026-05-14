// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_H
#define __RVX_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_gpio.h"
#include "rvx_i2c.h"
#include "rvx_irq.h"
#include "rvx_macros.h"
#include "rvx_spi.h"
#include "rvx_timer.h"
#include "rvx_uart.h"

#endif // __RVX_H