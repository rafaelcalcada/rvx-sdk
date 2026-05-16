// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_GPIO_H
#define __RVX_GPIO_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

// Base address of the GPIO controller registers.
#define RVX_GPIO_CONTROLLER_ADDRESS 0x40002000U

/// The mode (input/output) of a GPIO pin.
typedef enum RvxGpioPinMode
{
  RVX_GPIO_INPUT = 0, ///< Input mode.
  RVX_GPIO_OUTPUT = 1 ///< Output mode.
} RvxGpioPinMode;

typedef enum RvxGpioPinState
{
  RVX_GPIO_LOW = 0, ///< Logic 0 (low) state.
  RVX_GPIO_HIGH = 1 ///< Logic 1 (high) state.
} RvxGpioPinState;

/// Provide access to the GPIO controller registers.
typedef struct RVX_ALIGNED RvxGpioRegs
{
  volatile uint32_t RVX_GPIO_READ_REG;          ///< RVX GPIO Read Register.
  volatile uint32_t RVX_GPIO_OUTPUT_ENABLE_REG; ///< RVX GPIO Output Enable Register.
  volatile uint32_t RVX_GPIO_OUTPUT_REG;        ///< RVX GPIO Output Register.
  volatile uint32_t RVX_GPIO_CLEAR_REG;         ///< RVX GPIO Clear Register.
  volatile uint32_t RVX_GPIO_SET_REG;           ///< RVX GPIO Set Register.
} RvxGpioRegs;

/**
 * @brief Set the mode (input/output) of the GPIO pin specified by `pin_index`.
 *
 * Valid values for `pin_mode` are `RVX_GPIO_INPUT` and `RVX_GPIO_OUTPUT`.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin 0 as output and pin 1 as input.
 * rvx_gpio_pin_mode(gpio_controller, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_pin_mode(gpio_controller, 1, RVX_GPIO_INPUT);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param pin_index Index of the GPIO pin to configure as input or output.
 * @param pin_mode Desired pin mode as `RvxGpioPinMode` (`RVX_GPIO_INPUT` or `RVX_GPIO_OUTPUT`).
 */
static inline void rvx_gpio_pin_mode(RvxGpioRegs *gpio_controller, const uint8_t pin_index, RvxGpioPinMode pin_mode)
{
  if (pin_mode == RVX_GPIO_OUTPUT)
  {
    RVX_SET_BIT(gpio_controller->RVX_GPIO_OUTPUT_ENABLE_REG, pin_index);
  }
  else if (pin_mode == RVX_GPIO_INPUT)
  {
    RVX_CLR_BIT(gpio_controller->RVX_GPIO_OUTPUT_ENABLE_REG, pin_index);
  }
}

/**
 * @brief Drive a GPIO output pin to logic 1 (high).
 *
 * Drives the pin specified by `pin_index` to logic 1 (high) if it is configured as an output.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin 0 as output.
 * rvx_gpio_pin_mode(gpio_controller, 0, RVX_GPIO_OUTPUT);
 *
 * // Set pin 0 to logic 1 (high).
 * rvx_gpio_pin_set(gpio_controller, 0);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param pin_index Index of the GPIO pin to set.
 */
static inline void rvx_gpio_pin_set(RvxGpioRegs *gpio_controller, const uint8_t pin_index)
{
  gpio_controller->RVX_GPIO_SET_REG = 0x1U << pin_index;
}

/**
 * @brief Drive a GPIO output pin to logic 0 (low).
 *
 * Drives the pin specified by `pin_index` to logic 0 (low) if it is configured as an output.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin 0 as output.
 * rvx_gpio_pin_mode(gpio_controller, 0, RVX_GPIO_OUTPUT);
 *
 * // Set pin 0 to logic 0 (low).
 * rvx_gpio_pin_clear(gpio_controller, 0);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param pin_index Index of the GPIO pin to clear.
 */
static inline void rvx_gpio_pin_clear(RvxGpioRegs *gpio_controller, const uint8_t pin_index)
{
  gpio_controller->RVX_GPIO_CLEAR_REG = 0x1U << pin_index;
}

/**
 * @brief Read the logic state of a GPIO pin.
 *
 * Returns the current logic state of the pin specified by `pin_index`. Both input and output pins
 * can be read.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin 0 as input.
 * rvx_gpio_pin_mode(gpio_controller, 0, RVX_GPIO_INPUT);
 *
 * // Read the state of pin 0.
 * RvxGpioPinState pin0_state = rvx_gpio_pin_read(gpio_controller, 0);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param pin_index Index of the GPIO pin to read.
 * @return The pin logic state as `RvxGpioPinState` (`RVX_GPIO_LOW` or `RVX_GPIO_HIGH`).
 */
static inline RvxGpioPinState rvx_gpio_pin_read(RvxGpioRegs *gpio_controller, const uint8_t pin_index)
{
  return RVX_READ_BIT(gpio_controller->RVX_GPIO_READ_REG, pin_index) ? RVX_GPIO_HIGH : RVX_GPIO_LOW;
}

/**
 * @brief Write a logic value to a GPIO output pin.
 *
 * If `pin_state` is `RVX_GPIO_HIGH`, the pin is set to logic 1 (high); if `pin_state` is `RVX_GPIO_LOW`, the pin is set
 * to logic 0 (low).
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pins 0 and 1 as outputs.
 * rvx_gpio_pin_mode(gpio_controller, 0, RVX_GPIO_OUTPUT);
 * rvx_gpio_pin_mode(gpio_controller, 1, RVX_GPIO_OUTPUT);
 *
 * // Write logic 0 (low) to pin 0 and 1 (high) to pin 1.
 * rvx_gpio_pin_write(gpio_controller, 0, RVX_GPIO_LOW);
 * rvx_gpio_pin_write(gpio_controller, 1, RVX_GPIO_HIGH);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param pin_index Index of the GPIO pin to write.
 * @param pin_state Desired logic value (`RVX_GPIO_HIGH` for logic 1 or `RVX_GPIO_LOW` for logic 0).
 */
static inline void rvx_gpio_pin_write(RvxGpioRegs *gpio_controller, const uint8_t pin_index, RvxGpioPinState pin_state)
{
  if (pin_state == RVX_GPIO_HIGH)
    gpio_controller->RVX_GPIO_SET_REG = 0x1U << pin_index;
  else if (pin_state == RVX_GPIO_LOW)
    gpio_controller->RVX_GPIO_CLEAR_REG = 0x1U << pin_index;
}

/**
 * @brief Set the mode (input/output) of all GPIO pins simultaneously.
 *
 * Sets the mode of all GPIO pins according to `mode_mask`. Each bit set to 1 configures
 * the corresponding pin as an output, and each bit set to 0 configures it as an input. Pins are
 * updated simultaneously in a single register write.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Set pins 0 and 1 as inputs, and pins 2 and 3 as outputs in a single operation.
 * rvx_gpio_port_mode(gpio_controller, 0b1100);
 * ```
 *
 * @note To set the mode of a single pin, use `rvx_gpio_pin_mode()` instead.
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param mode_mask 32-bit bitmask specifying the mode of each pin:
 *                  0 = input, 1 = output.
 */
static inline void rvx_gpio_port_mode(RvxGpioRegs *gpio_controller, const uint32_t mode_mask)
{
  gpio_controller->RVX_GPIO_OUTPUT_ENABLE_REG = mode_mask;
}

/**
 * @brief Drive multiple GPIO output pins to logic 1 (high) in a single operation.
 *
 * Drives all GPIO pins corresponding to bits set to 1 in `bitmask` to logic 1 (high) if they are
 * configured as outputs.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin modes in a single operation.
 * rvx_gpio_port_mode(gpio_controller, 0b1110);
 *
 * // Set pins 1 and 2 to logic 1 (high) in a single operation.
 * rvx_gpio_port_set(gpio_controller, 0b0110);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param bitmask 32-bit bitmask specifying which pins to set:
 *                1 = set to logic 1 (high), 0 = leave unchanged.
 */
static inline void rvx_gpio_port_set(RvxGpioRegs *gpio_controller, const uint32_t bitmask)
{
  gpio_controller->RVX_GPIO_SET_REG = bitmask;
}

/**
 * @brief Drive multiple GPIO output pins to logic 0 (low) in a single operation.
 *
 * Drives all GPIO pins corresponding to bits set to 1 in `bitmask` to logic 0 (low) if they are
 * configured as outputs.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin modes in a single operation.
 * rvx_gpio_port_mode(gpio_controller, 0b1110);
 *
 * // Drive pins 1 and 2 to logic 0 (low) in a single operation.
 * rvx_gpio_port_clear(gpio_controller, 0b0110);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param bitmask 32-bit bitmask specifying which pins to clear:
 *                1 = set to logic 0 (low), 0 = leave unchanged.
 */
static inline void rvx_gpio_port_clear(RvxGpioRegs *gpio_controller, const uint32_t bitmask)
{
  gpio_controller->RVX_GPIO_CLEAR_REG = bitmask;
}

/**
 * @brief Read the logic values of all GPIO pins simultaneously.
 *
 * Returns the current logic levels of all pins in a single 32-bit value. Only the lower bits
 * corresponding to implemented pins are valid; higher bits are zero-padded.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Read all pins simultaneously.
 * uint32_t pin_values = rvx_gpio_port_read(gpio_controller);
 *
 * // Extract individual pin values.
 * bool pin0_value = (pin_values >> 0) & 1;
 * bool pin1_value = (pin_values >> 1) & 1;
 * bool pin2_value = (pin_values >> 2) & 1;
 * bool pin3_value = (pin_values >> 3) & 1;
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @return 32-bit value representing the logic states of all GPIO pins.
 *         Only lower bits corresponding to implemented pins are valid; higher bits are zero-padded.
 */
static inline uint32_t rvx_gpio_port_read(RvxGpioRegs *gpio_controller)
{
  return gpio_controller->RVX_GPIO_READ_REG;
}

/**
 * @brief Write logic values to all GPIO output pins in a single operation.
 *
 * Each bit set to 1 in `value_mask` sets the corresponding GPIO output pin to logic 1 (high), and each
 * bit set to 0 sets the corresponding pin to logic 0 (low), in a single operation.
 *
 * The bits corresponding to input pins are ignored.
 *
 * Example usage:
 * ```c
 * // Pointer to the GPIO controller registers.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 *
 * // Configure pin modes in a single operation.
 * rvx_gpio_port_mode(gpio_controller, 0b1110);
 *
 * // The state of output pins (1, 2 and 3) is updated in a single operation.
 * rvx_gpio_port_write(gpio_controller, 0b0100);
 * ```
 *
 * @param gpio_controller Pointer to the GPIO controller registers.
 * @param value_mask 32-bit bitmask specifying the logic values for the pins:
 *                   1 = set to logic 1 (high), 0 = set to logic 0 (low).
 */
static inline void rvx_gpio_port_write(RvxGpioRegs *gpio_controller, const uint32_t value_mask)
{
  gpio_controller->RVX_GPIO_OUTPUT_REG = value_mask;
}

#endif // __RVX_GPIO_H