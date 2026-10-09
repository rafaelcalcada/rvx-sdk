# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 RVX Project Contributors

if(NOT DEFINED RISCV_TOOLCHAIN_PREFIX)
  set(RISCV_TOOLCHAIN_PREFIX_CANDIDATE "riscv32-unknown-elf-")
else()  
  set(RISCV_TOOLCHAIN_PREFIX_CANDIDATE "${RISCV_TOOLCHAIN_PREFIX}")
endif()

find_program(RVX_C_COMPILER NAMES "${RISCV_TOOLCHAIN_PREFIX_CANDIDATE}gcc")

if(NOT RVX_C_COMPILER)
  unset(RISCV_TOOLCHAIN_PREFIX CACHE)
  message(FATAL_ERROR
    "Unable to find the RISC-V GNU Toolchain binaries in your PATH.\n"
    "RISCV_TOOLCHAIN_PREFIX is set to '${RISCV_TOOLCHAIN_PREFIX_CANDIDATE}'.\n"
    "Add the RISC-V toolchain binaries to your PATH or call CMake with -DRISCV_TOOLCHAIN_PREFIX=<prefix> to specify the toolchain prefix.\n"
    "For example, if the RISC-V toolchain binaries are in /opt/riscv/bin, you have to call CMake with:\n"
    "-DRISCV_TOOLCHAIN_PREFIX=/opt/riscv/bin/riscv32-unknown-elf-\n"
  )  
else()
  set(RISCV_TOOLCHAIN_PREFIX "${RISCV_TOOLCHAIN_PREFIX_CANDIDATE}" CACHE STRING "RISC-V toolchain prefix" FORCE)
endif()

find_program(RVX_OBJCOPY NAMES "${RISCV_TOOLCHAIN_PREFIX}objcopy")
find_program(RVX_OBJDUMP NAMES "${RISCV_TOOLCHAIN_PREFIX}objdump")
find_program(RVX_SIZE NAMES "${RISCV_TOOLCHAIN_PREFIX}size")

if(NOT RVX_OBJCOPY OR NOT RVX_OBJDUMP OR NOT RVX_SIZE)
  message(FATAL_ERROR
    "Unable to find the full RISC-V binutils set for prefix ${RISCV_TOOLCHAIN_PREFIX} in PATH. "
    "Expected ${RISCV_TOOLCHAIN_PREFIX}objcopy, ${RISCV_TOOLCHAIN_PREFIX}objdump, and ${RISCV_TOOLCHAIN_PREFIX}size."
  )
endif()

set(CMAKE_C_COMPILER "${RVX_C_COMPILER}" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_ASM_COMPILER "${RVX_C_COMPILER}" CACHE FILEPATH "ASM compiler" FORCE)
set(CMAKE_OBJCOPY "${RVX_OBJCOPY}" CACHE FILEPATH "objcopy tool" FORCE)
set(CMAKE_OBJDUMP "${RVX_OBJDUMP}" CACHE FILEPATH "objdump tool" FORCE)
set(CMAKE_SIZE "${RVX_SIZE}" CACHE FILEPATH "size tool" FORCE)

set(CMAKE_SYSTEM_NAME Generic CACHE STRING "Target system name" FORCE)
set(CMAKE_SYSTEM_PROCESSOR riscv32 CACHE STRING "Target processor" FORCE)

set(CMAKE_C_COMPILER_FORCED TRUE CACHE BOOL "Skip compiler detection" FORCE)
set(CMAKE_ASM_COMPILER_FORCED TRUE CACHE BOOL "Skip compiler detection" FORCE)