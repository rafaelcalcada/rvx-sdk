// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

.global rvx_startup
.section ".init.rvx_startup"
.option norvc;

rvx_startup:
  la gp, __global_pointer$
  la sp, __stack_top
  la t0, __bss_start
  la t1, __bss_end
1:
  bgeu t0, t1, 2f
  sw zero, 0(t0)
  addi t0, t0, 4
  j 1b
2:
  la t0, rvx_trap_vector
  csrw mtvec, t0

#ifdef RVX_BARE_METAL
  call main
#else
  call _start
#endif
  j .

.section ".init.rvx_trap_vector"
.global rvx_trap_vector
rvx_trap_vector:
  .option push
  .option norvc
  j rvx_trap_handler_default          // Default trap handler
  j .
  j .
  j rvx_trap_handler_software_m       // Software Interrupt from M-mode
  j .
  j .
  j .
  j rvx_trap_handler_timer_m          // Timer Interrupt from M-mode
  j .
  j .
  j .
  j rvx_trap_handler_external_m       // External Interrupt from M-mode
  j .
  j .
  j .
  j .
  j rvx_trap_handler_uart_m           // RVX UART Interrupt from M-mode
  j rvx_trap_handler_i2c_m            // RVX I2C Interrupt from M-mode
  j .
  j .
  j .
  j .
  j .
  j .
  j rvx_trap_handler_fast_irq_0       // Fast Interrupt 0
  j rvx_trap_handler_fast_irq_1       // Fast Interrupt 1
  j rvx_trap_handler_fast_irq_2       // Fast Interrupt 2
  j rvx_trap_handler_fast_irq_3       // Fast Interrupt 3
  j rvx_trap_handler_fast_irq_4       // Fast Interrupt 4
  j rvx_trap_handler_fast_irq_5       // Fast Interrupt 5
  j rvx_trap_handler_fast_irq_6       // Fast Interrupt 6
  j rvx_trap_handler_fast_irq_7       // Fast Interrupt 7
  .option pop

// Default trap handler: simply return from trap
.weak rvx_trap_handler_default
rvx_trap_handler_default:
  mret

.weak rvx_trap_handler_software_m
.weak rvx_trap_handler_timer_m
.weak rvx_trap_handler_external_m
.weak rvx_trap_handler_fast_irq_0
.weak rvx_trap_handler_fast_irq_1
.weak rvx_trap_handler_fast_irq_2
.weak rvx_trap_handler_fast_irq_3
.weak rvx_trap_handler_fast_irq_4
.weak rvx_trap_handler_fast_irq_5
.weak rvx_trap_handler_fast_irq_6
.weak rvx_trap_handler_fast_irq_7
.weak rvx_trap_handler_fast_irq_8
.weak rvx_trap_handler_fast_irq_9
.weak rvx_trap_handler_fast_irq_10
.weak rvx_trap_handler_fast_irq_11
.weak rvx_trap_handler_fast_irq_12
.weak rvx_trap_handler_fast_irq_13
.weak rvx_trap_handler_fast_irq_14
.weak rvx_trap_handler_fast_irq_15
.set rvx_trap_handler_software_m, rvx_trap_handler_default
.set rvx_trap_handler_timer_m, rvx_trap_handler_default
.set rvx_trap_handler_external_m, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_0, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_1, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_2, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_3, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_4, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_5, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_6, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_7, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_8, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_9, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_10, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_11, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_12, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_13, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_14, rvx_trap_handler_default
.set rvx_trap_handler_fast_irq_15, rvx_trap_handler_default

// Bootloader symbol for GDB debugging
// Any address from 0x00000000 to 0x00000FFF will be shown
// in GDB as 'rvx_bootloader'.

.section .rvx_bootloader, "a"
.globl rvx_bootloader
.type rvx_bootloader, @object
.size rvx_bootloader, 0x1000

rvx_bootloader:
    .byte 0