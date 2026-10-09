// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_TIMER_H
#define __RVX_TIMER_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/**
 * @brief Structure representing the timer controller registers.
 *
 * The fields are laid out in the same order as the hardware registers, allowing direct access through a pointer.
 *
 * For example:
 *
 * ```c
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * rvx_timer_start_counter(RVX_TIMER0);
 * ```
 */
typedef struct RVX_ALIGNED RvxTimer
{
  volatile uint32_t RVX_TIMER_COUNTER_ENABLE_REG; ///< RVX Timer Counter Enable Register.
  volatile uint32_t RVX_TIMER_COUNTERL_REG;       ///< Lower 32 bits of the RVX Timer Counter Register.
  volatile uint32_t RVX_TIMER_COUNTERH_REG;       ///< Upper 32 bits of the RVX Timer Counter Register.
  volatile uint32_t RVX_TIMER_COMPAREL_REG;       ///< Lower 32 bits of the RVX Timer Compare Register.
  volatile uint32_t RVX_TIMER_COMPAREH_REG;       ///< Upper 32 bits of the RVX Timer Compare Register.
} RvxTimer;

/**
 * @brief Start the timer counter, incrementing on every clock rising edge.
 *
 * Example usage:
 *
 * ```c
 * // Start the TIMER0 counter.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * rvx_timer_start_counter(RVX_TIMER0);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 */
static inline void rvx_timer_start_counter(RvxTimer *timer)
{
  timer->RVX_TIMER_COUNTER_ENABLE_REG = 1U;
}

/**
 * @brief Stop the timer counter.
 *
 * Example usage:
 *
 * ```c
 * // Stop the TIMER0 counter.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * rvx_timer_stop_counter(RVX_TIMER0);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 */
static inline void rvx_timer_stop_counter(RvxTimer *timer)
{
  timer->RVX_TIMER_COUNTER_ENABLE_REG = 0U;
}

/**
 * @brief Return `true` if the timer counter is running (counting), or `false` otherwise.
 *
 * Example usage:
 *
 * ```c
 * // Check whether the TIMER0 counter is running.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * bool timer_is_running = rvx_timer_is_counting(RVX_TIMER0);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 * @return `true` if the timer counter is running (counting), `false` otherwise.
 */
static inline bool rvx_timer_is_counting(RvxTimer *timer)
{
  return timer->RVX_TIMER_COUNTER_ENABLE_REG != 0U;
}

/**
 * @brief Set the timer counter to a new 64-bit value.
 *
 * The timer counter can be updated irrespective of whether the timer is currently running or stopped.
 *
 * Example usage:
 *
 * ```c
 * // Set the TIMER0 counter to one million.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * rvx_timer_set_counter(RVX_TIMER0, 1000000U);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 * @param counter_value 64-bit value to assign to the timer counter.
 */
static inline void rvx_timer_set_counter(RvxTimer *timer, uint64_t counter_value)
{
  timer->RVX_TIMER_COUNTERL_REG = 0x00000000U; // Temporarily set lower 32 bits to 0.
  timer->RVX_TIMER_COUNTERH_REG = (uint32_t)(counter_value >> 32);
  timer->RVX_TIMER_COUNTERL_REG = (uint32_t)counter_value;
}

/**
 * @brief Reset the timer counter to zero.
 *
 * The timer counter can be reset irrespective of whether the timer is currently running or stopped.
 *
 * Example usage:
 *
 * ```c
 * // Reset the TIMER0 counter to zero.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * rvx_timer_reset_counter(RVX_TIMER0);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 */
static inline void rvx_timer_reset_counter(RvxTimer *timer)
{
  rvx_timer_set_counter(timer, 0U);
}

/**
 * @brief Read the current value of the timer counter.
 *
 * The timer counter can be read irrespective of whether the timer is currently running or stopped.
 *
 * Example usage:
 *
 * ```c
 * // Read the current TIMER0 counter value.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * uint64_t counter_value = rvx_timer_get_counter(RVX_TIMER0);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 * @return 64-bit value representing the current timer count.
 */
static inline uint64_t rvx_timer_get_counter(RvxTimer *timer)
{
  uint32_t hi1, hi2, lo;
  do
  {
    hi1 = timer->RVX_TIMER_COUNTERH_REG;
    lo = timer->RVX_TIMER_COUNTERL_REG;
    hi2 = timer->RVX_TIMER_COUNTERH_REG;
  } while (hi1 != hi2);
  return ((uint64_t)hi2 << 32) | lo;
}

/**
 * @brief Set the timer compare register to a new 64-bit value.
 *
 * A timer interrupt is triggered when the timer counter is equal to or greater than the
 * compare register.
 *
 * Example usage:
 *
 * ```c
 * // Trigger the timer interrupt when TIMER0 reaches twelve million clock cycles.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * rvx_timer_set_compare(RVX_TIMER0, 12000000U);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 * @param compare_value 64-bit value to assign to the compare register.
 */
static inline void rvx_timer_set_compare(RvxTimer *timer, uint64_t compare_value)
{
  timer->RVX_TIMER_COMPAREL_REG = 0xFFFFFFFFU;
  timer->RVX_TIMER_COMPAREH_REG = (uint32_t)(compare_value >> 32);
  timer->RVX_TIMER_COMPAREL_REG = (uint32_t)compare_value;
}

/**
 * @brief Read the current value of the timer compare register.
 *
 * Example usage:
 *
 * ```c
 * // Read the TIMER0 compare value.
 * // Macro RVX_TIMER0 expands to the base address of TIMER0: ((RvxTimer *)0x40001000U).
 * uint64_t compare_value = rvx_timer_get_compare(RVX_TIMER0);
 * ```
 *
 * @param timer Pointer to the base address of the timer registers.
 * @return 64-bit value representing the current timer compare value.
 */
static inline uint64_t rvx_timer_get_compare(RvxTimer *timer)
{
  return ((uint64_t)timer->RVX_TIMER_COMPAREH_REG << 32) | timer->RVX_TIMER_COMPAREL_REG;
}

#endif // __RVX_TIMER_H