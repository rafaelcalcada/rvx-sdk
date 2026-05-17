// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_TIMER_H
#define __RVX_TIMER_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Base address of the timer controller registers.
#define RVX_TIMER_CONTROLLER_ADDRESS 0x40001000U

/// Provide access to the timer controller registers.
typedef struct RVX_ALIGNED RvxTimerRegs
{
  volatile uint32_t RVX_TIMER_COUNTER_ENABLE; ///< RVX Timer Counter Enable Register.
  volatile uint32_t RVX_TIMER_COUNTERL;       ///< Lower 32-bits of the RVX Timer Counter Register.
  volatile uint32_t RVX_TIMER_COUNTERH;       ///< Upper 32-bits of the RVX Timer Counter Register.
  volatile uint32_t RVX_TIMER_COMPAREL;       ///< Lower 32-bits of the RVX Timer Compare Register.
  volatile uint32_t RVX_TIMER_COMPAREH;       ///< Upper 32-bits of the RVX Timer Compare Register.
} RvxTimerRegs;

/**
 * @brief Start the timer counter, incrementing on every clock rising edge.
 *
 * @param timer_controller Pointer to the timer controller registers.
 */
static inline void rvx_timer_start_counter(RvxTimerRegs *timer_controller)
{
  timer_controller->RVX_TIMER_COUNTER_ENABLE = 1;
}

/**
 * @brief Stop the timer counter.
 *
 * @param timer_controller Pointer to the timer controller registers.
 */
static inline void rvx_timer_stop_counter(RvxTimerRegs *timer_controller)
{
  timer_controller->RVX_TIMER_COUNTER_ENABLE = 0;
}

/**
 * @brief Return `true` if the timer counter is running (counting), or `false` otherwise.
 *
 * @param timer_controller Pointer to the timer controller registers.
 * @return `true` if the timer counter is running (counting), `false` otherwise.
 */
static inline bool rvx_timer_is_counting(RvxTimerRegs *timer_controller)
{
  return timer_controller->RVX_TIMER_COUNTER_ENABLE;
}

/**
 * @brief Set the timer counter to a new 64-bit value.
 *
 * The timer counter can be updated irrespective of whether the timer is currently running or stopped.
 *
 * @param timer_controller Pointer to the timer controller registers.
 * @param new_value 64-bit value to assign to the timer counter.
 */
static inline void rvx_timer_set_counter(RvxTimerRegs *timer_controller, uint64_t new_value)
{
  timer_controller->RVX_TIMER_COUNTERL = 0x00000000U; // Temporarily set lower 32 bits to 0
  timer_controller->RVX_TIMER_COUNTERH = (uint32_t)(new_value >> 32);
  timer_controller->RVX_TIMER_COUNTERL = (uint32_t)(new_value & 0xFFFFFFFFU);
}

/**
 * @brief Reset the timer counter to zero.
 *
 * The timer counter can be reset irrespective of whether the timer is currently running or stopped.
 *
 * @param timer_controller Pointer to the timer controller registers.
 */
static inline void rvx_timer_reset_counter(RvxTimerRegs *timer_controller)
{
  rvx_timer_set_counter(timer_controller, 0);
}

/**
 * @brief Read the current value of the timer counter.
 *
 * The timer counter can be read irrespective of whether the timer is currently running or stopped.
 *
 * @param timer_controller Pointer to the timer controller registers.
 * @return 64-bit value representing the current timer count.
 */
static inline uint64_t rvx_timer_get_counter(RvxTimerRegs *timer_controller)
{
  uint32_t hi1, hi2, lo;
  do
  {
    hi1 = timer_controller->RVX_TIMER_COUNTERH;
    lo = timer_controller->RVX_TIMER_COUNTERL;
    hi2 = timer_controller->RVX_TIMER_COUNTERH;
  } while (hi1 != hi2);
  return ((uint64_t)hi2 << 32) | lo;
}

/**
 * @brief Set the timer compare register to a new 64-bit value.
 *
 * A timer interrupt is triggered when the timer counter is equal to or greater than the
 * compare register.
 *
 * @param timer_controller Pointer to the timer controller registers.
 * @param new_value 64-bit value to assign to the compare register.
 */
static inline void rvx_timer_set_compare(RvxTimerRegs *timer_controller, uint64_t new_value)
{
  timer_controller->RVX_TIMER_COMPAREL = 0xFFFFFFFFU;
  timer_controller->RVX_TIMER_COMPAREH = new_value >> 32;
  timer_controller->RVX_TIMER_COMPAREL = new_value & 0xFFFFFFFFU;
}

/**
 * @brief Read the current value of the timer compare register.
 *
 * @param timer_controller Pointer to the timer controller registers.
 * @return 64-bit value representing the current timer compare value.
 */
static inline uint64_t rvx_timer_get_compare(RvxTimerRegs *timer_controller)
{
  return ((uint64_t)timer_controller->RVX_TIMER_COMPAREH << 32) | timer_controller->RVX_TIMER_COMPAREL;
}

#endif // __RVX_TIMER_H