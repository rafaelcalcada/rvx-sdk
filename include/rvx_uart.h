// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_UART_H
#define __RVX_UART_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Base address of the UART controller registers.
#define RVX_UART_CONTROLLER_ADDRESS 0x40000000U

/// Provide access to the UART controller registers.
typedef struct RVX_ALIGNED RvxUartRegs
{
  volatile uint32_t RVX_UART_WRITE_REG;  ///< RVX UART Write Register.
  volatile uint32_t RVX_UART_READ_REG;   ///< RVX UART Read Register.
  volatile uint32_t RVX_UART_STATUS_REG; ///< RVX UART Status Register.
  volatile uint32_t RVX_UART_BAUD_REG;   ///< RVX UART Baud Rate Configuration Register.
} RvxUartRegs;

/**
 * @brief Set the UART controller baud rate.
 *
 * This function sets the UART controller baud rate by writing the appropriate value to the baud rate configuration
 * register. The value written is calculated based on the desired baud rate and the frequency of the clock signal
 * driving RVX.
 *
 * Example usage:
 *
 * ```c
 * // Pointer to the UART controller registers.
 * RvxUartRegs *uart_controller = (RvxUartRegs *)RVX_UART_CONTROLLER_ADDRESS;
 *
 * // Set UART controller baud rate to 115200 bauds per second (RVX clock frequency is 50 MHz).
 * rvx_uart_set_baud_rate(uart_controller, 115200, 50000000);
 * ```
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @param baud_rate The desired baud rate for UART communication, in bauds per second (bps).
 * @param rvx_clock_frequency The frequency of the clock signal driving RVX, in hertz (Hz).
 */
static inline void rvx_uart_set_baud_rate(RvxUartRegs *uart_controller, uint32_t baud_rate,
                                          uint32_t rvx_clock_frequency)
{
  uart_controller->RVX_UART_BAUD_REG = rvx_clock_frequency / baud_rate;
}

/**
 * @brief Return `true` if the UART controller is ready to transmit a new byte, or `false` otherwise.
 *
 * @note This function is non-blocking and can be used to check if the UART controller is ready before attempting to
 * send data.
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @return `true` if the UART controller is ready to send data, `false` otherwise.
 */
static inline bool rvx_uart_tx_ready(RvxUartRegs *uart_controller)
{
  return uart_controller->RVX_UART_STATUS_REG & 1;
}

/**
 * @brief Return `true` if the UART controller has received a new byte, or `false` otherwise.
 *
 * @note This function is non-blocking and can be used to check if the UART controller has received data before
 * attempting to read it.
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @return `true` if the UART controller has received a new byte, `false` otherwise.
 */
static inline bool rvx_uart_rx_ready(RvxUartRegs *uart_controller)
{
  return (uart_controller->RVX_UART_STATUS_REG >> 1) & 1;
}

/**
 * @brief Read the last byte received by the UART controller and clear the UART interrupt.
 *
 * This function is non-blocking and must be called from the trap handler for the UART interrupt.
 *
 * The UART controller will trigger an interrupt every time a new byte is received. The interrupt remains active until
 * the received byte is read using this function, which also clears the interrupt.
 *
 * If called before any data has been received, `0x00` is returned.
 *
 * The read is non-destructive, i.e., if this function is called again without new data being received, the same byte is
 * returned.
 *
 * Example usage:
 * ```c
 * // Pointer to the UART controller registers.
 * RvxUartRegs *uart_controller = (RvxUartRegs *)RVX_UART_CONTROLLER_ADDRESS;
 *
 * // Provides a trap handler for the UART interrupt
 * RVX_TRAP_HANDLER_M(rvx_trap_handler_uart_m) {
 *   // Read the received byte and clear the interrupt
 *   uint8_t rx_data = rvx_uart_read(uart_controller);
 *   // Do something with rx_data ...
 * }
 * ```
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @return The last byte received by the UART controller, or `0x00` if called before any data has been received.
 */
static inline uint8_t rvx_uart_read(RvxUartRegs *uart_controller)
{
  return uart_controller->RVX_UART_READ_REG;
}

/**
 * @brief Write a byte to the UART controller for transmission. Return `true` if the byte was successfully written, or
 * `false` if the UART controller is not ready to transmit.
 *
 * This function is non-blocking and will return immediately.
 *
 * @note For blocking behavior that waits until the UART controller is ready before sending, see `rvx_uart_send()`.
 *
 * Example usage:
 * ```c
 * // Pointer to the UART controller registers.
 * RvxUartRegs *uart_controller = (RvxUartRegs *)RVX_UART_CONTROLLER_ADDRESS;
 *
 * // Attempt to send 0xAB over UART without blocking.
 * bool tx_success = rvx_uart_write(uart_controller, 0xAB);
 * if (!tx_success) {
 *   // UART not ready to send, transmission failed.
 * }
 * ```
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @param tx_data The byte to write (`uint8_t`).
 * @return `true` if the byte was successfully written to the UART controller, `false` if the UART controller is not
 * ready to transmit.
 */
static inline bool rvx_uart_write(RvxUartRegs *uart_controller, uint8_t tx_data)
{
  if (!rvx_uart_tx_ready(uart_controller))
  {
    return false;
  }
  uart_controller->RVX_UART_WRITE_REG = tx_data;
  return true;
}

/**
 * @brief Block until a new byte is received by the UART, then read and return that byte.
 *
 * @note For non-blocking behavior, use `rvx_uart_read()` instead.
 *
 * Example usage:
 * ```c
 * // Pointer to the UART controller registers.
 * RvxUartRegs *uart_controller = (RvxUartRegs *)RVX_UART_CONTROLLER_ADDRESS;
 *
 * // Blocks until a new byte is received, then reads it
 * uint8_t rx_data = rvx_uart_receive(uart_controller);
 * ```
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @return The byte received by the UART controller.
 */
static inline uint8_t rvx_uart_receive(RvxUartRegs *uart_controller)
{
  while (!rvx_uart_rx_ready(uart_controller))
    ;
  return uart_controller->RVX_UART_READ_REG;
}

/**
 * @brief Block until the UART controller is ready to transmit, then send a byte.
 *
 * For non-blocking behavior, see `rvx_uart_write()`.
 *
 * Example usage:
 *
 * ```c
 * // Pointer to the UART controller registers.
 * RvxUartRegs *uart_controller = (RvxUartRegs *)RVX_UART_CONTROLLER_ADDRESS;
 *
 * // Send 'A' over UART
 * rvx_uart_send(uart_controller, 'A');
 *
 * // Wait until the transmission of 'A' is complete, then send 'B'
 * rvx_uart_send(uart_controller, 'B');
 * ```
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @param tx_data The byte to write (`uint8_t`).
 */
static inline void rvx_uart_send(RvxUartRegs *uart_controller, uint8_t tx_data)
{
  while (!rvx_uart_tx_ready(uart_controller))
    ;
  uart_controller->RVX_UART_WRITE_REG = tx_data;
}

/**
 * @brief Send a null-terminated C string over the UART.
 *
 * Transmits each character in `c_str` sequentially. This function is blocking and
 * will wait for the UART controller to be ready before sending each character.
 *
 * Example usage:
 * ```c
 * // Pointer to the UART controller registers.
 * RvxUartRegs *uart_controller = (RvxUartRegs *)RVX_UART_CONTROLLER_ADDRESS;
 *
 * // Send the string "Hello, World!" over the UART
 * rvx_uart_send_string(uart_controller, "Hello, World!");
 * ```
 *
 * @param uart_controller Pointer to the UART controller registers.
 * @param c_str Pointer to the null-terminated C string to transmit.
 */
static inline void rvx_uart_send_string(RvxUartRegs *uart_controller, const char *c_str)
{
  while (*c_str)
  {
    rvx_uart_send(uart_controller, *c_str++);
  }
}

#endif // __RVX_UART_H