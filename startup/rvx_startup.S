// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

.global __rvx_startup
.section ".init.rvx_startup"
.option norvc;

__rvx_startup:
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
  li t1, 1
  or t0, t0, t1
  csrw mtvec, t0
  call __libc_init_array
  call main
  call __libc_fini_array
  j .

// Provide weak definitions for the C runtime initialization and finalization functions,
// so that linking works in case a program is compiled with -nostdlib or -nodefaultlibs.
.weak __libc_init_array
__libc_init_array:
  ret
.weak __libc_fini_array
__libc_fini_array:
  ret

.section ".init.rvx_trap_vector"
.global rvx_trap_vector
rvx_trap_vector:
  .option push
  .option norvc
  j rvx_trap_handler                  // Trap handler for all exceptions and also for interrupts if in direct mode
  j rvx_irq_handler_software_s        // IRQ handler for Software IRQ from S-mode (future proofing, not used now in RVX)
  j rvx_unimplemented_handler
  j rvx_irq_handler_software_m        // IRQ handler for Software IRQ from M-mode
  j rvx_unimplemented_handler
  j rvx_irq_handler_timer_s           // IRQ handler for Timer IRQ from S-mode (future proofing, not used now in RVX)
  j rvx_unimplemented_handler
  j rvx_irq_handler_timer_m           // IRQ handler for Timer IRQ from M-mode
  j rvx_unimplemented_handler
  j rvx_irq_handler_external_s        // IRQ handler for External IRQ from S-mode (future proofing, not used now in RVX)
  j rvx_unimplemented_handler
  j rvx_irq_handler_external_m        // IRQ handler for External IRQ from M-mode
  j rvx_unimplemented_handler
  j rvx_irq_handler_counter_overflow  // IRQ handler for Counter Overflow IRQ
  j rvx_unimplemented_handler
  j rvx_unimplemented_handler
  .option pop

// Expose relevant CSRs using integer file registers for debugging purposes. 
// Then halts execution.
rvx_unimplemented_handler:
  csrr t0, mcause
  csrr t1, mepc
  csrr t2, mtval
  csrr t3, mtvec
  csrr t4, mstatus
  csrr t5, mip
  csrr t6, mie
  j .

// Default trap handler: dispatches to the IRQ or exception handler based on mcause.
.weak rvx_trap_handler
rvx_trap_handler:
  csrr t0, mcause
  bltz t0, 1f
  j rvx_exception_handler
1:
  j rvx_irq_handler

// Default IRQ handler: dispatches to the interrupt-specific handler based on mcause.
.weak rvx_irq_handler
rvx_irq_handler:
  csrr t0, mcause
  slli t0, t0, 1
  srli t0, t0, 1 // Clear the interrupt bit, leaving only the interrupt code
  li t1, 1
  beq t0, t1, .Lirq_software_s
  li t1, 3
  beq t0, t1, .Lirq_software_m
  li t1, 5
  beq t0, t1, .Lirq_timer_s
  li t1, 7
  beq t0, t1, .Lirq_timer_m
  li t1, 9
  beq t0, t1, .Lirq_external_s
  li t1, 11
  beq t0, t1, .Lirq_external_m
  li t1, 13
  beq t0, t1, .Lirq_counter_overflow
  j rvx_unimplemented_handler
.Lirq_software_s:
  j rvx_irq_handler_software_s
.Lirq_software_m:
  j rvx_irq_handler_software_m
.Lirq_timer_s:
  j rvx_irq_handler_timer_s
.Lirq_timer_m:
  j rvx_irq_handler_timer_m
.Lirq_external_s:
  j rvx_irq_handler_external_s
.Lirq_external_m:
  j rvx_irq_handler_external_m
.Lirq_counter_overflow:
  j rvx_irq_handler_counter_overflow

.weak rvx_irq_handler_software_s
.weak rvx_irq_handler_software_m
.weak rvx_irq_handler_timer_s
.weak rvx_irq_handler_timer_m
.weak rvx_irq_handler_external_s
.weak rvx_irq_handler_external_m // Default implementation provided by rvx_plic.h, if included
.weak rvx_irq_handler_counter_overflow
.weak rvx_exception_handler
.set rvx_irq_handler_software_s, rvx_unimplemented_handler
.set rvx_irq_handler_software_m, rvx_unimplemented_handler
.set rvx_irq_handler_timer_s, rvx_unimplemented_handler
.set rvx_irq_handler_timer_m, rvx_unimplemented_handler
.set rvx_irq_handler_external_s, rvx_unimplemented_handler
.set rvx_irq_handler_counter_overflow, rvx_unimplemented_handler
.set rvx_exception_handler, rvx_unimplemented_handler