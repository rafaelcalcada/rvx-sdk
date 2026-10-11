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

/**
 * @brief Structure representing the PLIC controller register block.
 *
 * The fields are laid out in the same order as the hardware registers, allowing direct access through a pointer.
 *
 * For example:
 *
 * ```c
 * RvxPlic *rvx_plic0_base = (RvxPlic *)0x40005000U;
 * rvx_plic_enable_source(rvx_plic0_base, 2); // Enable interrupt source 2.
 * ```
 */
typedef struct RVX_ALIGNED RvxPlic
{
  volatile uint32_t RVX_PLIC_PRIORITY_REG[RVX_PLIC_SOURCE_COUNT]; ///< Priority registers for each interrupt source
  volatile uint32_t RVX_PLIC_ENABLE_REG;                          ///< Enable register
  volatile uint32_t RVX_PLIC_PENDING_REG;                         ///< Pending register
  volatile uint32_t RVX_PLIC_CLAIM_REG;                           ///< Claim register
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
 * rvx_plic_set_priority(RVX_PLIC0_BASE, 2, RVX_PLIC_MAX_PRIORITY);
 * ```
 *
 * @param plic Base address of the PLIC controller register block.
 * @param source_id Interrupt source ID in the range 1 to `RVX_PLIC_SOURCE_COUNT`.
 * @param priority Priority value in the range 0 to `RVX_PLIC_MAX_PRIORITY`. Invalid values are ignored.
 */
static inline void rvx_plic_set_priority(RvxPlic *plic, const uint8_t source_id, const uint8_t priority)
{
  if (source_id < 1U || source_id > RVX_PLIC_SOURCE_COUNT || priority > RVX_PLIC_MAX_PRIORITY)
    return;
  plic->RVX_PLIC_PRIORITY_REG[source_id - 1U] = priority;
}

/**
 * @brief Read the priority currently assigned to an interrupt source.
 *
 * @param plic Base address of the PLIC controller register block.
 * @param source_id Interrupt source ID in the range 1 to `RVX_PLIC_SOURCE_COUNT`.
 * @return The priority value (0-15) currently assigned to the source.
 */
static inline uint8_t rvx_plic_get_priority(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id < 1U || source_id > RVX_PLIC_SOURCE_COUNT)
    return 0U;
  return (uint8_t)plic->RVX_PLIC_PRIORITY_REG[source_id - 1U];
}

/**
 * @brief Enable interrupt requests from a specific source.
 *
 * Example usage:
 * ```c
 * // Enable interrupt source 2.
 * rvx_plic_enable_source(RVX_PLIC0_BASE, 2);
 * ```
 *
 * @param plic Base address of the PLIC controller register block.
 * @param source_id Interrupt source ID in the range 1 to `RVX_PLIC_SOURCE_COUNT`.
 */
static inline void rvx_plic_enable_source(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id < 1U || source_id > RVX_PLIC_SOURCE_COUNT)
    return;
  RVX_SET_BIT(plic->RVX_PLIC_ENABLE_REG, source_id - 1U);
}

/**
 * @brief Disable interrupt requests from a specific source.
 *
 * Example usage:
 * ```c
 * // Disable interrupt source 2.
 * rvx_plic_disable_source(RVX_PLIC0_BASE, 2);
 * ```
 *
 * @param plic Base address of the PLIC controller register block.
 * @param source_id Interrupt source ID in the range 1 to `RVX_PLIC_SOURCE_COUNT`.
 */
static inline void rvx_plic_disable_source(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id < 1U || source_id > RVX_PLIC_SOURCE_COUNT)
    return;
  RVX_CLR_BIT(plic->RVX_PLIC_ENABLE_REG, source_id - 1U);
}

/**
 * @brief Return `true` if a specific interrupt source is enabled, or `false` otherwise.
 *
 * @param plic Base address of the PLIC controller register block.
 * @param source_id Interrupt source ID in the range 1 to `RVX_PLIC_SOURCE_COUNT`.
 * @return `true` if the source is enabled, `false` otherwise.
 */
static inline bool rvx_plic_is_source_enabled(RvxPlic *plic, const uint8_t source_id)
{
  if (source_id < 1U || source_id > RVX_PLIC_SOURCE_COUNT)
    return false;
  return RVX_READ_BIT(plic->RVX_PLIC_ENABLE_REG, source_id - 1U);
}

/**
 * @brief Claim the interrupt source that won PLIC arbitration.
 *
 * Example usage:
 * ```c
 * const uint8_t source_id = rvx_plic_claim_source(RVX_PLIC0_BASE);
 * if (source_id != 0U)
 * {
 *   // Handle the interrupt raised by `source_id`.
 * }
 * ```
 *
 * @param plic Base address of the PLIC controller register block.
 * @return The winning source ID (1-16), or 0 if no enabled source is pending.
 */
static inline uint8_t rvx_plic_claim_source(RvxPlic *plic)
{
  return (uint8_t)(plic->RVX_PLIC_CLAIM_REG & 0x1FU);
}

#endif // __RVX_PLIC_H
