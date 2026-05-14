// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_TIMER_H
#define __RVX_TIMER_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Provide access to the Timer module registers.
typedef struct RVX_ALIGNED RvxTimer
{
  volatile uint32_t RVX_TIMER_COUNTER_ENABLE; ///< RVX Timer Counter Enable Register.
  volatile uint32_t RVX_TIMER_COUNTERL;       ///< Lower 32-bits of the RVX Timer Counter Register.
  volatile uint32_t RVX_TIMER_COUNTERH;       ///< Upper 32-bits of the RVX Timer Counter Register.
  volatile uint32_t RVX_TIMER_COMPAREL;       ///< Lower 32-bits of the RVX Timer Compare Register.
  volatile uint32_t RVX_TIMER_COMPAREH;       ///< Upper 32-bits of the RVX Timer Compare Register.
} RvxTimer;

/**
 * @brief Start the timer, incrementing the timer counter on every clock rising edge.
 *
 * @param timer_address Base address of the Timer module.
 */
static inline void rvx_timer_start(RvxTimer *timer_address)
{
  timer_address->RVX_TIMER_COUNTER_ENABLE = 1;
}

/**
 * @brief Stop the timer, retaining the current counter value until the timer is started again.
 *
 * @param timer_address Base address of the Timer module.
 */
static inline void rvx_timer_stop(RvxTimer *timer_address)
{
  timer_address->RVX_TIMER_COUNTER_ENABLE = 0;
}

/**
 * @brief Return `true` if the timer is currently running (counting), or `false` if it is stopped.
 *
 * @param timer_address Base address of the Timer module.
 * @return `true` if the timer is running, `false` otherwise.
 */
static inline bool rvx_timer_is_running(RvxTimer *timer_address)
{
  return timer_address->RVX_TIMER_COUNTER_ENABLE;
}

/**
 * @brief Set the value of the 64-bit timer counter.
 *
 * The timer counter can be updated irrespective of whether the timer is currently running or stopped.
 *
 * @param timer_address Base address of the Timer module.
 * @param new_value 64-bit value to assign to the timer counter.
 */
static inline void rvx_timer_set_counter(RvxTimer *timer_address, uint64_t new_value)
{
  timer_address->RVX_TIMER_COUNTERL = 0x00000000U; // Temporarily set lower 32 bits to 0
  timer_address->RVX_TIMER_COUNTERH = (uint32_t)(new_value >> 32);
  timer_address->RVX_TIMER_COUNTERL = (uint32_t)(new_value & 0xFFFFFFFFU);
}

/**
 * @brief Read the current value of the 64-bit timer counter.
 *
 * The timer counter can be read irrespective of whether the timer is currently running or stopped.
 *
 * @param timer_address Base address of the Timer module.
 * @return 64-bit value representing the current timer count.
 */
static inline uint64_t rvx_timer_get_counter(RvxTimer *timer_address)
{
  uint32_t hi1, hi2, lo;
  do
  {
    hi1 = timer_address->RVX_TIMER_COUNTERH;
    lo = timer_address->RVX_TIMER_COUNTERL;
    hi2 = timer_address->RVX_TIMER_COUNTERH;
  } while (hi1 != hi2);
  return ((uint64_t)hi2 << 32) | lo;
}

/**
 * @brief Set the value of the 64-bit timer compare register.
 *
 * @note A timer interrupt is triggered when the timer counter is equal to or greater than the
 * compare register.
 *
 * @param timer_address Base address of the Timer module.
 * @param new_value 64-bit value to assign to the compare register.
 */
static inline void rvx_timer_set_compare(RvxTimer *timer_address, uint64_t new_value)
{
  timer_address->RVX_TIMER_COMPAREL = 0xFFFFFFFFU;
  timer_address->RVX_TIMER_COMPAREH = new_value >> 32;
  timer_address->RVX_TIMER_COMPAREL = new_value & 0xFFFFFFFFU;
}

/**
 * @brief Read the current value of the 64-bit timer compare register.
 *
 * @param timer_address Base address of the Timer module.
 * @return The current value of the timer compare register.
 */
static inline uint64_t rvx_timer_get_compare(RvxTimer *timer_address)
{
  return ((uint64_t)timer_address->RVX_TIMER_COMPAREH << 32) | timer_address->RVX_TIMER_COMPAREL;
}

#endif // __RVX_TIMER_H