// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#include "rvx_uart.h"

__attribute__((weak)) void rvx_exception_handler(uint32_t exception_cause)
{
  (void)exception_cause;
  rvx_uart_println((RvxUart *)0x40000000U, "ERROR: rvx_exception_handler not implemented.");
  while (1)
    ;
}

__attribute__((weak)) void rvx_interrupt_handler(uint32_t interrupt_cause)
{
  (void)interrupt_cause;
  rvx_uart_println((RvxUart *)0x40000000U, "ERROR: rvx_interrupt_handler not implemented.");
  while (1)
    ;
}

__attribute__((weak, interrupt("machine"))) void rvx_trap_handler(void)
{
  uint32_t mcause;
  asm volatile("csrr %0, mcause" : "=r"(mcause));
  if (mcause & 0x80000000)
    rvx_interrupt_handler(mcause);
  else
    rvx_exception_handler(mcause);
}
