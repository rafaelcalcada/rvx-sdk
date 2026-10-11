/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2020-2026 RVX Project Contributors */

OUTPUT_ARCH("riscv")
ENTRY(__rvx_startup)

MEMORY
{
  BOOTLOADER (rx) : ORIGIN = 0x00000000, LENGTH = 4K
  RVX_RAM (wx)    : ORIGIN = 0x00001000, LENGTH = __rvx_ram_size
}

SECTIONS
{

  __rvx_min_free_space = DEFINED(__rvx_min_free_space) ? __rvx_min_free_space : 0;

  .rvx_bootloader ORIGIN(BOOTLOADER) : ALIGN(4)
  {
    KEEP(*(.rvx_bootloader))
    . = 0x1000;
  }

  .boot_header : ALIGN(4)
  {
    LONG(0x52565830)                /* RVX magic number: "RVX0" */
    LONG(__rvx_startup);            /* Entry point address */    
    LONG(__rvx_boot_image_size);    /* Boot image size in bytes */    
    LONG(0x52563332);               /* RISC-V magic number: "RV32" */
  } > RVX_RAM

  .text :
  {
    *(.init.rvx_startup)
    *(.init.rvx_trap_vector)
    *(.text.rvx_irq_handler_*)
    *(.text.rvx_irq_handler)
    *(.text.rvx_exception_handler)
    *(SORT_NONE(.init))
    *(SORT_NONE(.fini))

    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    *(.preinit_array*)
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    *(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*))
    *(.init_array .ctors)
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    *(.text*)
    *(.gnu.linkonce.t.*)

    . = ALIGN(4);
    *(.rodata*)

    . = ALIGN(4);
    *(.srodata*)
  } > RVX_RAM

  .data : ALIGN(4)
  {
    *(.data*)
    *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    __sdata_start = .;
    PROVIDE(__global_pointer$ = __sdata_start + 0x800);
    *(.sdata*)
    *(.gnu.linkonce.s.*)
    __sdata_end = .;
  } > RVX_RAM

  .bss (NOLOAD) : ALIGN(4)
  {
    PROVIDE(__bss_start = .);
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);
    PROVIDE(__bss_end = .);
  }

  /DISCARD/ :
  {
    *(.comment)
    *(.riscv.attributes)
  }

}

PROVIDE( __stack_top                 = ORIGIN(RVX_RAM) + LENGTH(RVX_RAM));
PROVIDE( __heap_start                = ALIGN(ADDR(.bss) + SIZEOF(.bss), 8));
PROVIDE( __free_ram                  = __stack_top - __heap_start);
PROVIDE( __rvx_boot_image_size       = ADDR(.data) + SIZEOF(.data) - ORIGIN(RVX_RAM));
PROVIDE( __freertos_irq_stack_top    = __stack_top);

ASSERT(__free_ram >= __rvx_min_free_space, "ERROR: Program does not fit in RVX RAM: minimum free memory space requirement cannot be satisfied. Reduce program size or increase RVX RAM size.");