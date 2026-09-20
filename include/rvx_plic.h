// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_PLIC_H
#define __RVX_PLIC_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Base address of the PLIC controller registers.
#define RVX_PLIC_CONTROLLER_ADDRESS 0x40005000U

/// Number of interrupt sources managed by the PLIC.
#define RVX_PLIC_SOURCE_COUNT 16U

/// Maximum priority value that can be assigned to an interrupt source.
#define RVX_PLIC_MAX_PRIORITY 15U

/// Provide access to the PLIC controller registers.
typedef struct RVX_ALIGNED RvxPlicRegs
{
  volatile uint32_t RVX_PLIC_PRIORITY_REG[RVX_PLIC_SOURCE_COUNT]; ///< RVX PLIC Priority Registers (one per source).
  volatile uint32_t RVX_PLIC_ENABLE_REG;                          ///< RVX PLIC Enable Register.
  volatile uint32_t RVX_PLIC_PENDING_REG;                         ///< RVX PLIC Pending Register.
  volatile uint32_t RVX_PLIC_CLAIM_REG;                           ///< RVX PLIC Claim Register.
} RvxPlicRegs;

/**
 * @brief Set the priority of an interrupt source.
 *
 * Valid `priority` values range from 0 to `RVX_PLIC_MAX_PRIORITY` (inclusive). A source with priority 0 never wins
 * arbitration, effectively disabling it regardless of the state of its enable bit.
 *
 * Example usage:
 * ```c
 * // Pointer to the PLIC controller registers.
 * RvxPlicRegs *plic_controller = (RvxPlicRegs *)RVX_PLIC_CONTROLLER_ADDRESS;
 *
 * // Assign the highest priority to interrupt source 2.
 * rvx_plic_set_priority(plic_controller, 2, RVX_PLIC_MAX_PRIORITY);
 * ```
 *
 * @param plic_controller Pointer to the PLIC controller registers.
 * @param source_id Index of the interrupt source (0-15).
 * @param priority Priority value to assign to the source (0-15).
 */
static inline void rvx_plic_set_priority(RvxPlicRegs *plic_controller, const uint8_t source_id, const uint8_t priority)
{
  plic_controller->RVX_PLIC_PRIORITY_REG[source_id] = priority;
}

/**
 * @brief Read the priority currently assigned to an interrupt source.
 *
 * @param plic_controller Pointer to the PLIC controller registers.
 * @param source_id Index of the interrupt source (0-15).
 * @return The priority value (0-15) currently assigned to the source.
 */
static inline uint8_t rvx_plic_get_priority(RvxPlicRegs *plic_controller, const uint8_t source_id)
{
  return (uint8_t)plic_controller->RVX_PLIC_PRIORITY_REG[source_id];
}

/**
 * @brief Enable interrupt requests from a specific source.
 *
 * Example usage:
 * ```c
 * // Pointer to the PLIC controller registers.
 * RvxPlicRegs *plic_controller = (RvxPlicRegs *)RVX_PLIC_CONTROLLER_ADDRESS;
 *
 * // Enable interrupt source 2.
 * rvx_plic_enable_source(plic_controller, 2);
 * ```
 *
 * @param plic_controller Pointer to the PLIC controller registers.
 * @param source_id Index of the interrupt source to enable (0-15).
 */
static inline void rvx_plic_enable_source(RvxPlicRegs *plic_controller, const uint8_t source_id)
{
  RVX_SET_BIT(plic_controller->RVX_PLIC_ENABLE_REG, source_id);
}

/**
 * @brief Disable interrupt requests from a specific source.
 *
 * Example usage:
 * ```c
 * // Pointer to the PLIC controller registers.
 * RvxPlicRegs *plic_controller = (RvxPlicRegs *)RVX_PLIC_CONTROLLER_ADDRESS;
 *
 * // Disable interrupt source 2.
 * rvx_plic_disable_source(plic_controller, 2);
 * ```
 *
 * @param plic_controller Pointer to the PLIC controller registers.
 * @param source_id Index of the interrupt source to disable (0-15).
 */
static inline void rvx_plic_disable_source(RvxPlicRegs *plic_controller, const uint8_t source_id)
{
  RVX_CLR_BIT(plic_controller->RVX_PLIC_ENABLE_REG, source_id);
}

/**
 * @brief Return `true` if a specific interrupt source is enabled, or `false` otherwise.
 *
 * @param plic_controller Pointer to the PLIC controller registers.
 * @param source_id Index of the interrupt source to query (0-15).
 * @return `true` if the source is enabled, `false` otherwise.
 */
static inline bool rvx_plic_is_source_enabled(RvxPlicRegs *plic_controller, const uint8_t source_id)
{
  return RVX_READ_BIT(plic_controller->RVX_PLIC_ENABLE_REG, source_id);
}

/// Bit index of the "interrupt pending" flag within the PLIC claim register.
#define RVX_PLIC_CLAIM_PENDING_BIT 4U

/**
 * @brief Identify the interrupt source that won PLIC arbitration.
 *
 * Example usage:
 * ```c
 * // Pointer to the PLIC controller registers.
 * RvxPlicRegs *plic_controller = (RvxPlicRegs *)RVX_PLIC_CONTROLLER_ADDRESS;
 *
 * uint8_t source_id;
 * if (rvx_plic_claim_source(plic_controller, &source_id))
 * {
 *   // Handle the interrupt raised by `source_id`.
 * }
 * ```
 *
 * @param plic_controller Pointer to the PLIC controller registers.
 * @param source_id Set to the index (0-15) of the winning interrupt source if one is pending.
 * @return `true` if an interrupt source is pending, `false` otherwise (in which case `*source_id` is left
 * unmodified).
 */
static inline bool rvx_plic_claim_source(RvxPlicRegs *plic_controller, uint8_t *source_id)
{
  uint32_t claim = plic_controller->RVX_PLIC_CLAIM_REG;

  if (!RVX_READ_BIT(claim, RVX_PLIC_CLAIM_PENDING_BIT))
  {
    return false;
  }

  *source_id = (uint8_t)(claim & 0xFU);
  return true;
}

#endif // __RVX_PLIC_H
