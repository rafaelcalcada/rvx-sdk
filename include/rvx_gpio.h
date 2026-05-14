// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_GPIO_H
#define __RVX_GPIO_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// The direction of a GPIO pin.
typedef enum RvxGpioPinDirection
{
  RVX_GPIO_INPUT = 0, ///< Input direction.
  RVX_GPIO_OUTPUT = 1 ///< Output direction.
} RvxGpioPinDirection;

/// Provide access to the GPIO controller registers.
typedef struct RVX_ALIGNED RvxGpio
{
  volatile uint32_t RVX_GPIO_READ_REG;          ///< RVX GPIO Read Register.
  volatile uint32_t RVX_GPIO_OUTPUT_ENABLE_REG; ///< RVX GPIO Output Enable Register.
  volatile uint32_t RVX_GPIO_OUTPUT_REG;        ///< RVX GPIO Output Register.
  volatile uint32_t RVX_GPIO_CLEAR_REG;         ///< RVX GPIO Clear Register.
  volatile uint32_t RVX_GPIO_SET_REG;           ///< RVX GPIO Set Register.
} RvxGpio;

/**
 * @brief Set the direction of the GPIO pin specified by `pin_index`.
 *
 * Valid values for `direction` are `RVX_GPIO_INPUT` or `RVX_GPIO_OUTPUT`. If `direction` is invalid
 * or if `pin_index` refers to a non-existent pin (beyond the number of implemented GPIO pins), no
 * action is taken and no error is reported.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output and pin 1 as input.
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 1, RVX_GPIO_INPUT);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param pin_index Index of the GPIO pin to configure as input or output.
 * @param direction Desired pin direction as `RvxGpioPinDirection` (`RVX_GPIO_INPUT` or
 * `RVX_GPIO_OUTPUT`).
 */
static inline void rvx_gpio_set_direction(RvxGpio *gpio_address, const uint8_t pin_index, RvxGpioPinDirection direction)
{
  if (direction == RVX_GPIO_OUTPUT)
  {
    RVX_SET_BIT(gpio_address->RVX_GPIO_OUTPUT_ENABLE_REG, pin_index);
  }
  else if (direction == RVX_GPIO_INPUT)
  {
    RVX_CLR_BIT(gpio_address->RVX_GPIO_OUTPUT_ENABLE_REG, pin_index);
  }
}

/**
 * @brief Set the direction of all GPIO pins simultaneously.
 *
 * Sets the direction of all GPIO pins according to `direction_mask`. Each bit set to 1 configures
 * the corresponding pin as an output, and each bit set to 0 configures it as an input. Pins are
 * updated simultaneously in a single register write.
 *
 * Only the lower bits of `direction_mask` corresponding to implemented GPIO pins are used. Any
 * upper bits beyond the number of available pins are ignored by the hardware.
 *
 * Example usage:
 * ```c *
 * // Configure pins 1 and 3 as outputs, pins 0 and 2 as inputs (simultaneously).
 * rvx_gpio_set_direction_mask(RVX_GPIO_ADDRESS, 0b1010);
 * ```
 *
 * @note To set the direction of a single pin, use `rvx_gpio_set_direction()` instead.
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param direction_mask 32-bit bitmask specifying the direction of each pin:
 *                       0 = input, 1 = output.
 */
static inline void rvx_gpio_set_direction_mask(RvxGpio *gpio_address, const uint32_t direction_mask)
{
  gpio_address->RVX_GPIO_OUTPUT_ENABLE_REG = direction_mask;
}

/**
 * @brief Set a GPIO output pin to logic 1 (high).
 *
 * Drives the pin specified by `pin_index` to logic 1 (high) if it is configured as an output. If the pin
 * is configured as an input, the output latch is updated without changing the pin state. The new
 * value takes effect when the pin is later configured as an output.
 *
 * If `pin_index` refers to a non-existent pin (beyond the number of implemented GPIO pins), the
 * operation is ignored without error.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output.
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 0, RVX_GPIO_OUTPUT);
 *
 * // Set pin 0 to logic 1 (high).
 * rvx_gpio_set_high(RVX_GPIO_ADDRESS, 0);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param pin_index Index of the GPIO pin to set.
 */
static inline void rvx_gpio_set_high(RvxGpio *gpio_address, const uint8_t pin_index)
{
  gpio_address->RVX_GPIO_SET_REG = 0x1U << pin_index;
}

/**
 * @brief Set multiple GPIO output pins to logic 1 (high) simultaneously.
 *
 * Drives all GPIO pins corresponding to bits set to 1 in `bitmask` to logic 1 (high) if they are
 * configured as outputs. Pins configured as inputs have their output latch updated without changing
 * the pin state. The new value takes effect when the pin is later configured as an output.
 *
 * Bits corresponding to non-existent pins (beyond the number of implemented GPIO pins) are ignored.
 * Bits set to 0 leave the corresponding pins unchanged.
 *
 * Example usage:
 * ```c
 * // Configure pin directions (simultaneously).
 * // Pin 0 = input, pin 1 = output, pin 2 = output, pin 3 = output
 * rvx_gpio_set_direction_mask(RVX_GPIO_ADDRESS, 0b1110);
 *
 * // Set pins 1 and 2 to logic 1 (high) simultaneously.
 * // Pin 0 output latch is set to 1, but the pin state is not affected.
 * // Pin 3 remains unchanged.
 * rvx_gpio_set_high_mask(RVX_GPIO_ADDRESS, 0b0111);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param bitmask 32-bit bitmask specifying which pins to set:
 *                1 = set to logic 1 (high), 0 = leave unchanged.
 */
static inline void rvx_gpio_set_high_mask(RvxGpio *gpio_address, const uint32_t bitmask)
{
  gpio_address->RVX_GPIO_SET_REG = bitmask;
}

/**
 * @brief Set a GPIO output pin to logic 0 (low).
 *
 * Drives the pin specified by `pin_index` to logic 0 (low) if it is configured as an output. If the pin
 * is configured as an input, the output latch is updated without changing the pin state. The new
 * value takes effect when the pin is later configured as an output.
 *
 * If `pin_index` refers to a non-existent pin (beyond the number of implemented GPIO pins), the
 * operation is ignored without error.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output.
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 0, RVX_GPIO_OUTPUT);
 *
 * // Set pin 0 to logic 0 (low).
 * rvx_gpio_set_low(RVX_GPIO_ADDRESS, 0);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param pin_index Index of the GPIO pin to clear.
 */
static inline void rvx_gpio_set_low(RvxGpio *gpio_address, const uint8_t pin_index)
{
  gpio_address->RVX_GPIO_CLEAR_REG = 0x1U << pin_index;
}

/**
 * @brief Set multiple GPIO output pins to logic 0 (low) simultaneously.
 *
 * Drives all GPIO pins corresponding to bits set to 1 in `bitmask` to logic 0 (low) if they are
 * configured as outputs. Pins configured as inputs have their output latch updated without changing
 * the pin state. The new value takes effect when the pin is later configured as an output.
 *
 * Bits corresponding to non-existent pins (beyond the number of implemented GPIO pins) are ignored.
 * Bits set to 0 leave the corresponding pins unchanged.
 *
 * Example usage:
 * ```c
 * // Configure pin directions.
 * // Pin 0 = input, pin 1 = output, pin 2 = output, pin 3 = output
 * rvx_gpio_set_direction_mask(RVX_GPIO_ADDRESS, 0b1110);
 *
 * // Set pins 1 and 2 to logic 0 (low) simultaneously.
 * // Pin 0 output latch is set to 0, but the pin state is not affected.
 * // Pin 3 remains unchanged.
 * rvx_gpio_set_low_mask(RVX_GPIO_ADDRESS, 0b0111);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param bitmask 32-bit bitmask specifying which pins to clear:
 *                1 = set to logic 0 (low), 0 = leave unchanged.
 */
static inline void rvx_gpio_set_low_mask(RvxGpio *gpio_address, const uint32_t bitmask)
{
  gpio_address->RVX_GPIO_CLEAR_REG = bitmask;
}

/**
 * @brief Read the logic value of a GPIO pin.
 *
 * Returns the current logic value of the pin specified by `pin_index`. Both input and output pins
 * can be read.
 *
 * If `pin_index` refers to a non-existent pin (beyond the number of implemented GPIO pins), the
 * return value is `false`.
 *
 * Example usage:
 * ```c *
 * // Configure pin 0 as input.
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 0, RVX_GPIO_INPUT);
 *
 * // Read the state of pin 0.
 * bool pin0_value = rvx_gpio_read(RVX_GPIO_ADDRESS, 0);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param pin_index Index of the GPIO pin to read.
 * @return The pin logic state as `bool` (`true` or `false`).
 */
static inline bool rvx_gpio_read(RvxGpio *gpio_address, const uint8_t pin_index)
{
  return RVX_READ_BIT(gpio_address->RVX_GPIO_READ_REG, pin_index);
}

/**
 * @brief Read the logic values of all GPIO pins simultaneously.
 *
 * Returns the current logic levels of all pins in a single 32-bit value. Only the lower bits
 * corresponding to implemented pins are valid; higher bits are zero-padded.
 *
 * Example usage:
 * ```c *
 * // Read all pins simultaneously.
 * uint32_t pin_values = rvx_gpio_read_all(RVX_GPIO_ADDRESS);
 *
 * // Extract individual pin values.
 * bool pin0_value = (pin_values >> 0) & 1;
 * bool pin1_value = (pin_values >> 1) & 1;
 * bool pin2_value = (pin_values >> 2) & 1;
 * bool pin3_value = (pin_values >> 3) & 1;
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @return 32-bit value representing the logic states of all GPIO pins.
 *         Only lower bits corresponding to implemented pins are valid; higher bits are zero-padded.
 */
static inline uint32_t rvx_gpio_read_all(RvxGpio *gpio_address)
{
  return gpio_address->RVX_GPIO_READ_REG;
}

/**
 * @brief Write a logic value to a GPIO output pin.
 *
 * If `value` is `true`, the pin is set to logic 1 (high); if `false`, the pin is set to logic 0 (low).
 * Writing to input pins updates their output latch but does not affect the pin state until the pin
 * is reconfigured as output.
 *
 * If `pin_index` refers to a non-existent pin (beyond the number of implemented GPIO pins), the
 * operation is ignored without error.
 *
 * Example usage:
 * ```c *
 * // Configure pins 0 and 1 as outputs.
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_set_direction(RVX_GPIO_ADDRESS, 1, RVX_GPIO_OUTPUT);
 *
 * // Write logic 0 (low) to pin 0 and 1 (high) to pin 1.
 * rvx_gpio_write(RVX_GPIO_ADDRESS, 0, false);
 * rvx_gpio_write(RVX_GPIO_ADDRESS, 1, true);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param pin_index Index of the GPIO pin to write.
 * @param value Desired logic value (`true` for logic 1 or `false` for logic 0).
 */
static inline void rvx_gpio_write(RvxGpio *gpio_address, const uint8_t pin_index, bool value)
{
  if (value)
    gpio_address->RVX_GPIO_SET_REG = 0x1U << pin_index;
  else
    gpio_address->RVX_GPIO_CLEAR_REG = 0x1U << pin_index;
}

/**
 * @brief Write logic values to all GPIO output pins simultaneously.
 *
 * Each bit set to 1 in `value_mask` sets the corresponding GPIO output pin to logic 1 (high), and each
 * bit set to 0 sets the corresponding pin to logic 0 (low), in a single operation. Writing to pins
 * configured as inputs updates their output latch but does not affect the actual pin state until
 * the pin is reconfigured as an output.
 *
 * Bits corresponding to non-existent pins (beyond the number of implemented GPIO pins) are ignored
 * without error.
 *
 * Example usage:
 * ```c
 * // Configure pin directions.
 * // Pin 0 = input, pin 1 = output, pin 2 = output, pin 3 = output
 * rvx_gpio_set_direction_mask(RVX_GPIO_ADDRESS, 0b1110);
 *
 * // The output pins (1, 2 and 3) are updated simultaneously according to the value mask.
 * // Pin 0 (input) output latch is updated to 1, but pin state remains unaffected.
 * rvx_gpio_write_mask(RVX_GPIO_ADDRESS, 0b0101);
 * ```
 *
 * @param gpio_address Base address of the GPIO controller.
 * @param value_mask 32-bit bitmask specifying the logic values for the pins:
 *                   1 = set to logic 1 (high), 0 = set to logic 0 (low).
 */
static inline void rvx_gpio_write_mask(RvxGpio *gpio_address, const uint32_t value_mask)
{
  gpio_address->RVX_GPIO_OUTPUT_REG = value_mask;
}

#endif // __RVX_GPIO_H