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
  call __libc_init_array
  call main
  call __libc_fini_array
  j .

.weak __libc_init_array
__libc_init_array:
  ret
.weak __libc_fini_array
__libc_fini_array:
  ret
