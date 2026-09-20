// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_MACROS_H
#define __RVX_MACROS_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include <stdbool.h> // for bool type
#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint32_t, uint16_t, uint8_t types

/// Mark a function or variable as weak, allowing it to be overridden by other definitions.
#define RVX_WEAK __attribute__((weak))

/// Define a function without prologue/epilogue code.
#define RVX_NAKED __attribute__((naked))

/// Create an alias for a function.
#define RVX_ALIAS(weak_alias) __attribute__((alias(weak_alias)))

/// Mark a function as weak and define it as an alias for another.
/// This allows for default function implementations that can be overridden by a strong definition
/// elsewhere.
#define RVX_WEAK_ALIAS(weak_alias) __attribute__((weak, alias(weak_alias)))

/// Enforce 4-byte alignment on variables or structs.
#define RVX_ALIGNED __attribute__((aligned(4)))

/// Set the specified bits in a Memory-Mapped IO register given a bitmask.
#define RVX_SET_MASK(register, bitmask) ((register) |= (bitmask))

/// Clear the specified bits in a Memory-Mapped IO register given a bitmask.
#define RVX_CLR_MASK(register, bitmask) ((register) &= ~(bitmask))

/// Set a specific bit in a Memory-Mapped IO register.
#define RVX_SET_BIT(register, bit_index) ((register) |= (1U << (bit_index)))

/// Clear a specific bit in a Memory-Mapped IO register.
#define RVX_CLR_BIT(register, bit_index) ((register) &= ~(1U << (bit_index)))

/// Read a specific bit from a Memory-Mapped IO register.
#define RVX_READ_BIT(register, bit_index) (((register) >> (bit_index)) & 1U)

#endif // __RVX_MACROS_H