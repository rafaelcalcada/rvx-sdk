// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#include "rvx_uart.h"

__attribute__((weak)) void rvx_exception_handler(const uint32_t cause)
{
  (void)cause;
  rvx_uart_println((RvxUart *)0x40000000U, "ERROR: rvx_exception_handler not implemented.");
  while (1)
    ;
}

__attribute__((weak)) void rvx_interrupt_handler(const uint32_t cause)
{
  (void)cause;
  rvx_uart_println((RvxUart *)0x40000000U, "ERROR: rvx_interrupt_handler not implemented.");
  while (1)
    ;
}

__attribute__((weak, interrupt("machine"))) void rvx_trap_handler(void)
{
  uint32_t cause;
  asm volatile("csrr %0, mcause" : "=r"(cause));
  if (cause & 0x80000000)
    rvx_interrupt_handler(cause);
  else
    rvx_exception_handler(cause);
}
