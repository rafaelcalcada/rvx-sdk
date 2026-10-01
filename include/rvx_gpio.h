// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_GPIO_H
#define __RVX_GPIO_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// The direction (input/output) of a GPIO pin.
typedef enum RvxGpioPinDirection
{
  RVX_GPIO_INPUT = 0, ///< Pin direction: Input.
  RVX_GPIO_OUTPUT = 1 ///< Pin direction: Output.
} RvxGpioPinDirection;

/// The logic level (low/high) of a GPIO pin.
typedef enum RvxGpioPinLevel
{
  RVX_GPIO_LOW = 0, ///< Low logic level (boolean false).
  RVX_GPIO_HIGH = 1 ///< High logic level (boolean true).
} RvxGpioPinLevel;

/**
 * @brief Structure representing the GPIO controller registers.
 *
 * The fields of this structure are laid out in the same order as the hardware registers,
 * allowing direct access to them through a pointer.
 *
 * For example:
 *
 * ```c
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U)
 * uint32_t pin_values = rvx_gpio_port_read(RVX_GPIO0); // Read the current GPIO pin values.
 * ```
 */
typedef struct RVX_ALIGNED RvxGpio
{
  volatile uint32_t RVX_GPIO_READ_REG;          ///< RVX GPIO Read Register.
  volatile uint32_t RVX_GPIO_OUTPUT_ENABLE_REG; ///< RVX GPIO Output Enable Register.
  volatile uint32_t RVX_GPIO_OUTPUT_REG;        ///< RVX GPIO Output Register.
  volatile uint32_t RVX_GPIO_CLEAR_REG;         ///< RVX GPIO Clear Register.
  volatile uint32_t RVX_GPIO_SET_REG;           ///< RVX GPIO Set Register.
} RvxGpio;

/**
 * @brief Set the direction (input/output) of the GPIO pin specified by `pin_index`.
 *
 * Valid values for `pin_direction` are `RVX_GPIO_INPUT` and `RVX_GPIO_OUTPUT`.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output and pin 1 as input.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_pin_direction(RVX_GPIO0, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_pin_direction(RVX_GPIO0, 1, RVX_GPIO_INPUT);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param pin_index GPIO pin index in the range 0 to 31.
 * @param pin_direction Desired pin direction as `RvxGpioPinDirection` (`RVX_GPIO_INPUT` or `RVX_GPIO_OUTPUT`).
 */
static inline void rvx_gpio_pin_direction(RvxGpio *gpio, const uint8_t pin_index, RvxGpioPinDirection pin_direction)
{
  if (pin_index >= 32U)
    return;

  if (pin_direction == RVX_GPIO_OUTPUT)
  {
    RVX_SET_BIT(gpio->RVX_GPIO_OUTPUT_ENABLE_REG, pin_index);
  }
  else if (pin_direction == RVX_GPIO_INPUT)
  {
    RVX_CLR_BIT(gpio->RVX_GPIO_OUTPUT_ENABLE_REG, pin_index);
  }
}

/**
 * @brief Drive a GPIO output pin to high logic level (boolean true).
 *
 * If `pin_index` is configured as input, the internal output latch is updated but the pin logic level remains
 * unaffected.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output and set it to high logic level.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_pin_direction(RVX_GPIO0, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_pin_set(RVX_GPIO0, 0);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param pin_index GPIO pin index in the range 0 to 31.
 */
static inline void rvx_gpio_pin_set(RvxGpio *gpio, const uint8_t pin_index)
{
  if (pin_index >= 32U)
    return;

  gpio->RVX_GPIO_SET_REG = 0x1U << pin_index;
}

/**
 * @brief Drive a GPIO output pin to low logic level (boolean false).
 *
 * If `pin_index` is configured as input, the internal output latch is updated but the pin logic level remains
 * unaffected.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output and set it to low logic level.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_pin_direction(RVX_GPIO0, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_pin_clear(RVX_GPIO0, 0);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param pin_index GPIO pin index in the range 0 to 31.
 */
static inline void rvx_gpio_pin_clear(RvxGpio *gpio, const uint8_t pin_index)
{
  if (pin_index >= 32U)
    return;

  gpio->RVX_GPIO_CLEAR_REG = 0x1U << pin_index;
}

/**
 * @brief Read the logic level of a GPIO pin.
 *
 * Returns the current logic level of the pin specified by `pin_index`. Both input and output pins
 * can be read.
 *
 * Example usage:
 * ```c
 * // Read the logic level of pin 0.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * RvxGpioPinLevel pin0_level = rvx_gpio_pin_read(RVX_GPIO0, 0);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param pin_index GPIO pin index in the range 0 to 31.
 * @return The pin logic level as `RvxGpioPinLevel` (`RVX_GPIO_LOW` or `RVX_GPIO_HIGH`).
 */
static inline RvxGpioPinLevel rvx_gpio_pin_read(RvxGpio *gpio, const uint8_t pin_index)
{
  if (pin_index >= 32U)
    return RVX_GPIO_LOW;

  return RVX_READ_BIT(gpio->RVX_GPIO_READ_REG, pin_index) ? RVX_GPIO_HIGH : RVX_GPIO_LOW;
}

/**
 * @brief Drive a GPIO output pin to the specified logic level (low/high).
 *
 * If `pin_index` is configured as input, the internal output latch is updated but the pin logic level remains
 * unaffected.
 *
 * Valid values for `pin_level` are `RVX_GPIO_LOW` and `RVX_GPIO_HIGH`.
 *
 * Example usage:
 * ```c
 * // Configure pin 0 as output and drive it to high logic level (boolean true)
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_pin_direction(RVX_GPIO0, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_pin_write(RVX_GPIO0, 0, RVX_GPIO_HIGH);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param pin_index GPIO pin index in the range 0 to 31.
 * @param pin_level Desired logic level (`RVX_GPIO_HIGH` for boolean true or `RVX_GPIO_LOW` for boolean false).
 */
static inline void rvx_gpio_pin_write(RvxGpio *gpio, const uint8_t pin_index, RvxGpioPinLevel pin_level)
{
  if (pin_index >= 32U)
    return;

  if (pin_level == RVX_GPIO_HIGH)
    gpio->RVX_GPIO_SET_REG = 0x1U << pin_index;
  else if (pin_level == RVX_GPIO_LOW)
    gpio->RVX_GPIO_CLEAR_REG = 0x1U << pin_index;
}

/**
 * @brief Set the direction (input/output) of all GPIO pins simultaneously.
 *
 * The `direction_mask` parameter specifies the direction of each GPIO pin. Bit `n` in `direction_mask` corresponds to
 * GPIO pin `n`. Setting a bit to 1 configures the corresponding pin as an output, while setting it to 0 configures the
 * pin as an input. All pin directions are updated in a single register write.
 *
 * Bits beyond the number of available GPIO pins are ignored. For example, if the GPIO module is configured to have 4
 * pins, any bits set in `direction_mask` beyond bit 3 will be ignored.
 *
 * Example usage:
 * ```c
 * // Set pins 0 and 1 as inputs, and pins 2 and 3 as outputs.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_port_direction(RVX_GPIO0, 0b1100);
 * ```
 *
 * @note To set the direction of a single pin, use `rvx_gpio_pin_direction()` instead.
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param direction_mask Bitmask specifying the direction of each pin: 0 for input, 1 for output.
 */
static inline void rvx_gpio_port_direction(RvxGpio *gpio, const uint32_t direction_mask)
{
  gpio->RVX_GPIO_OUTPUT_ENABLE_REG = direction_mask;
}

/**
 * @brief Drive multiple GPIO output pins to high logic level (boolean true).
 *
 * The `bitmask` parameter specifies which GPIO pins to drive high. Bit `n` in `bitmask` corresponds to GPIO pin `n`.
 * Setting a bit to 1 drives the corresponding pin to high logic level, while setting it to 0 leaves the pin unchanged.
 *
 * If pin `n` is not configured as an output, setting the corresponding bit in `bitmask` updates its internal output
 * latch, but the pin logic level remains unaffected.
 *
 * Bits beyond the number of available GPIO pins are ignored. For example, if the GPIO module is configured to have 4
 * pins, any bits set in `bitmask` beyond bit 3 will be ignored.
 *
 * Example usage:
 * ```c
 * // Set pins 1 and 2 to high logic level, leaving all other pins unchanged.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_port_direction(RVX_GPIO0, 0b1110);
 * rvx_gpio_port_set(RVX_GPIO0, 0b0110);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param bitmask Bitmask specifying which pins to set to high logic level: 1 = high logic level (boolean true), 0 =
 * leave unchanged.
 */
static inline void rvx_gpio_port_set(RvxGpio *gpio, const uint32_t bitmask)
{
  gpio->RVX_GPIO_SET_REG = bitmask;
}

/**
 * @brief Drive multiple GPIO output pins to low logic level (boolean false).
 *
 * The `bitmask` parameter specifies which GPIO pins to drive low. Bit `n` in `bitmask` corresponds to GPIO pin `n`.
 * Setting a bit to 1 drives the corresponding pin to low logic level, while setting it to 0 leaves the pin unchanged.
 *
 * If pin `n` is not configured as an output, setting the corresponding bit in `bitmask` updates its internal output
 * latch, but the pin logic level remains unaffected.
 *
 * Bits beyond the number of available GPIO pins are ignored. For example, if the GPIO module is configured to have 4
 * pins, any bits set in `bitmask` beyond bit 3 will be ignored.
 *
 * Example usage:
 * ```c
 * // Set pins 0 and 3 to low logic level, leaving all other pins unchanged.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_port_direction(RVX_GPIO0, 0b1101);
 * rvx_gpio_port_clear(RVX_GPIO0, 0b1001);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param bitmask 32-bit bitmask specifying which pins to set to low logic level:
 *                1 = low logic level (boolean false), 0 = leave unchanged.
 */
static inline void rvx_gpio_port_clear(RvxGpio *gpio, const uint32_t bitmask)
{
  gpio->RVX_GPIO_CLEAR_REG = bitmask;
}

/**
 * @brief Read the logic levels of all GPIO pins simultaneously.
 *
 * The returned 32-bit value represents the logic levels of all GPIO pins. Bit `n` in the returned value corresponds to
 * GPIO pin `n`. A value of 1 indicates high logic level (boolean true), and a value of 0 indicates low logic level
 * (boolean false).
 *
 * The higher bits beyond the number of available GPIO pins are padded with zeros. For example, if the GPIO module is
 * configured to have 4 pins, bits 4 to 31 in the returned value will be zero.
 *
 * Example usage:
 * ```c
 * // Read the logic levels of all pins simultaneously.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * uint32_t pin_values = rvx_gpio_port_read(RVX_GPIO0);
 *
 * // Extract individual pin values.
 * RvxGpioPinLevel pin0_level = (pin_values >> 0) & 1;
 * RvxGpioPinLevel pin1_level = (pin_values >> 1) & 1;
 * RvxGpioPinLevel pin2_level = (pin_values >> 2) & 1;
 * RvxGpioPinLevel pin3_level = (pin_values >> 3) & 1;
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @return 32-bit value representing the logic levels of all GPIO pins. Only lower bits corresponding to implemented
 * pins are valid; higher bits are zero-padded.
 */
static inline uint32_t rvx_gpio_port_read(RvxGpio *gpio)
{
  return gpio->RVX_GPIO_READ_REG;
}

/**
 * @brief Drive GPIO output pins to the specified logic levels (low/high).
 *
 * The `level_mask` parameter specifies the desired logic levels for the GPIO output pins. Bit `n` in `level_mask`
 * corresponds to GPIO pin `n`. A value of 1 sets an output pin to high logic level (boolean true), and a value of 0
 * sets it to low logic level (boolean false).
 *
 * If pin `n` is configured as an input, setting its corresponding bit in `level_mask` updates its internal output
 * latch, but does not affect the actual pin logic level.
 *
 * Bits beyond the number of available GPIO pins are ignored. For example, if the GPIO module is configured to have 4
 * pins, only the lower 4 bits of `level_mask` are considered; higher bits are ignored.
 *
 * Example usage:
 * ```c
 * // Set GPIO pins 0 and 2 to high, and pins 1 and 3 to low.
 * // Macro RVX_GPIO0 expands to the base address of GPIO0: ((RvxGpio *)0x40002000U).
 * rvx_gpio_port_direction(RVX_GPIO0, 0b1111);
 * rvx_gpio_port_write(RVX_GPIO0, 0b0101);
 * ```
 *
 * @param gpio Pointer to the base address of the GPIO registers.
 * @param level_mask Bitmask specifying the logic level for each pin: 1 = high logic level (boolean true), 0 = low
 *                   logic level (boolean false).
 */
static inline void rvx_gpio_port_write(RvxGpio *gpio, const uint32_t level_mask)
{
  gpio->RVX_GPIO_OUTPUT_REG = level_mask;
}

#endif // __RVX_GPIO_H