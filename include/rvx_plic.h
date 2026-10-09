// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_PLIC_H
#define __RVX_PLIC_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Number of interrupt sources managed by the PLIC.
#define RVX_PLIC_SOURCE_COUNT 16U

/// Maximum priority value that can be assigned to an interrupt source.
#define RVX_PLIC_MAX_PRIORITY 15U

/// Bit index of the "interrupt pending" flag within the PLIC claim register.
#define RVX_PLIC_CLAIM_PENDING_BIT 4U

/**
 * @brief Structure representing the PLIC controller registers.
 *
 * The fields are laid out in the same order as the hardware registers, allowing direct access through a pointer.
 *
 * For example:
 *
 * ```c
 * // Macro RVX_PLIC0 expands to the base address of PLIC0: ((RvxPlic *)0x40005000U).
 * rvx_plic_enable_source(RVX_PLIC0, 2);
 * ```
 */
typedef struct RVX_ALIGNED RvxPlic
{
  volatile uint32_t RVX_PLIC_PRIORITY_REG[RVX_PLIC_SOURCE_COUNT]; ///< RVX PLIC Priority Registers (one per source).
  volatile uint32_t RVX_PLIC_ENABLE_REG;                          ///< RVX PLIC Enable Register.
  volatile uint32_t RVX_PLIC_PENDING_REG;                         ///< RVX PLIC Pending Register.
  volatile uint32_t RVX_PLIC_CLAIM_REG;                           ///< RVX PLIC Claim Register.
} RvxPlic;

/**
 * @brief Set the priority of an interrupt source.
 *
 * Valid `priority` values range from 0 to `RVX_PLIC_MAX_PRIORITY` (inclusive). A source with priority 0 never wins
 * arbitration, effectively disabling it regardless of the state of its enable bit.
 *
 * Example usage:
 * ```c
 * // Assign the highest priority to interrupt source 2.
 * // Macro RVX_PLIC0 expands to the base address of PLIC0: ((RvxPlic *)0x40005000U).
 * rvx_plic_set_priority(RVX_PLIC0, 2, RVX_PLIC_MAX_PRIORITY);
 * ```
 *
 * @param plic Pointer to the base address of the PLIC registers.
 * @param source_id Interrupt source index in the range 0 to `RVX_PLIC_SOURCE_COUNT - 1`.
 * @param priority Priority value in the range 0 to `RVX_PLIC_MAX_PRIORITY`. Invalid values are ignored.
 */
static inline void rvx_plic_set_priority(RvxPlic *plic, const uint8_t source_id, const uint8_t priority)
{
  if (source_id >= RVX_PLIC_SOURCE_COUNT || priority > RVX_PLIC_MAX_PRIORITY)
    return;
  plic->RVX_PLIC_PRIORITY_REG[source_id] = priority;
}

/**
 * @brief Read the priority currently assigned to an interrupt source.
 *
 * @param plic Pointer to the base address of the PLIC registers.
 * @param source_id Interrupt source index in the range 0 to `RVX_PLIC_SOURCE_COUNT - 1`.
 * @return The priority value (0-15) currently assigned to the source.
 */
static inline uint8_t rvx_plic_get_priority(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id >= RVX_PLIC_SOURCE_COUNT)
    return 0U;
  return (uint8_t)plic->RVX_PLIC_PRIORITY_REG[source_id];
}

/**
 * @brief Enable interrupt requests from a specific source.
 *
 * Example usage:
 * ```c
 * // Enable interrupt source 2.
 * // Macro RVX_PLIC0 expands to the base address of PLIC0: ((RvxPlic *)0x40005000U).
 * rvx_plic_enable_source(RVX_PLIC0, 2);
 * ```
 *
 * @param plic Pointer to the base address of the PLIC registers.
 * @param source_id Interrupt source index in the range 0 to `RVX_PLIC_SOURCE_COUNT - 1`.
 */
static inline void rvx_plic_enable_source(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id >= RVX_PLIC_SOURCE_COUNT)
    return;
  RVX_SET_BIT(plic->RVX_PLIC_ENABLE_REG, source_id);
}

/**
 * @brief Disable interrupt requests from a specific source.
 *
 * Example usage:
 * ```c
 * // Disable interrupt source 2.
 * // Macro RVX_PLIC0 expands to the base address of PLIC0: ((RvxPlic *)0x40005000U).
 * rvx_plic_disable_source(RVX_PLIC0, 2);
 * ```
 *
 * @param plic Pointer to the base address of the PLIC registers.
 * @param source_id Interrupt source index in the range 0 to `RVX_PLIC_SOURCE_COUNT - 1`.
 */
static inline void rvx_plic_disable_source(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id >= RVX_PLIC_SOURCE_COUNT)
    return;
  RVX_CLR_BIT(plic->RVX_PLIC_ENABLE_REG, source_id);
}

/**
 * @brief Return `true` if a specific interrupt source is enabled, or `false` otherwise.
 *
 * @param plic Pointer to the base address of the PLIC registers.
 * @param source_id Interrupt source index in the range 0 to `RVX_PLIC_SOURCE_COUNT - 1`.
 * @return `true` if the source is enabled, `false` otherwise.
 */
static inline bool rvx_plic_is_source_enabled(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id >= RVX_PLIC_SOURCE_COUNT)
    return false;
  return RVX_READ_BIT(plic->RVX_PLIC_ENABLE_REG, source_id);
}

/**
 * @brief Identify the interrupt source that won PLIC arbitration.
 *
 * Example usage:
 * ```c
 * uint8_t source_id;
 * // Macro RVX_PLIC0 expands to the base address of PLIC0: ((RvxPlic *)0x40005000U).
 * if (rvx_plic_claim_source(RVX_PLIC0, &source_id))
 * {
 *   // Handle the interrupt raised by `source_id`.
 * }
 * ```
 *
 * @param plic Pointer to the base address of the PLIC registers.
 * @param source_id Set to the index (0-15) of the winning interrupt source if one is pending.
 * @return `true` if an interrupt source is pending, `false` otherwise (in which case `*source_id` is left
 * unmodified).
 */
static inline bool rvx_plic_claim_source(RvxPlic *plic, uint8_t *source_id)
{
  if (source_id == NULL)
    return false;

  uint32_t claim = plic->RVX_PLIC_CLAIM_REG;

  if (!RVX_READ_BIT(claim, RVX_PLIC_CLAIM_PENDING_BIT))
  {
    return false;
  }

  *source_id = (uint8_t)(claim & 0xFU);
  return true;
}

#endif // __RVX_PLIC_H
