// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_UART_H
#define __RVX_UART_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Provide access to the UART controller registers.
typedef struct RVX_ALIGNED RvxUart
{
  volatile uint32_t RVX_UART_WRITE_REG;  ///< RVX UART Write Register.
  volatile uint32_t RVX_UART_READ_REG;   ///< RVX UART Read Register.
  volatile uint32_t RVX_UART_STATUS_REG; ///< RVX UART Status Register.
  volatile uint32_t RVX_UART_BAUD_REG;   ///< RVX UART Baud Rate Configuration Register.
} RvxUart;

/**
 * @brief Initialize the UART with a specified baud rate.
 *
 * This function configures the UART's baud rate by writing the number of clock cycles
 * per baud into the UART baud configuration register.
 *
 * After initialization, the UART will transmit and receive data at the configured
 * baud rate. This function does not modify any other UART settings.
 *
 * Example usage:
 *
 * ```c
 * // Initialize UART at 115200 bauds per second (RVX connected to a 50 MHz clock source)
 * rvx_uart_init(RVX_UART_ADDRESS, 115200, 50000000);
 * ```
 *
 * @param uart_address Base address of the UART controller.
 * @param baud_rate The desired baud rate
 * @param clock_frequency_in_hz The frequency of RVX top module clock pin, in Hz
 */
static inline void rvx_uart_init(RvxUart *uart_address, uint32_t baud_rate, uint32_t clock_frequency_in_hz)
{
  uart_address->RVX_UART_BAUD_REG = clock_frequency_in_hz / baud_rate;
}

/**
 * @brief Return `true` if the UART is ready to transmit a new byte, or `false` otherwise.
 *
 * @note In most cases, you do not need to call this function directly, as UART transmitting
 * functions (e.g., `rvx_uart_send`, `rvx_uart_send_string`) will wait until the UART is
 * ready to transmit.
 *
 * @param uart_address Base address of the UART controller.
 * @return `true` if the UART is ready to send data, `false` otherwise.
 */
static inline bool rvx_uart_ready_to_send(RvxUart *uart_address)
{
  return uart_address->RVX_UART_STATUS_REG & 1;
}

/**
 * @brief Wait until the UART transmission is complete.
 *
 * This function blocks until the UART is ready to send data again.
 *
 * Example usage:
 *
 * ```c *
 * // Send data
 * rvx_uart_send(RVX_UART_ADDRESS, 'A');
 *
 * // Wait until transmission above is complete
 * rvx_uart_wait_tx_done(RVX_UART_ADDRESS);
 * ```
 *
 * @param uart_address Base address of the UART controller.
 */
static inline void rvx_uart_wait_tx_done(RvxUart *uart_address)
{
  while (!rvx_uart_ready_to_send(uart_address))
    ;
}

/**
 * @brief Return `true` if the UART has received data that has not been read yet, or `false` otherwise.
 *
 * Example usage:
 *
 * ```c
 * // Wait for data to be received
 * while (!rvx_uart_data_available(RVX_UART_ADDRESS));
 *
 * // Read the received byte
 * uint8_t rx_data = rvx_uart_read(RVX_UART_ADDRESS);
 * ```
 *
 * @param uart_address Base address of the UART controller.
 * @return `true` if new data is available, `false` if no new data has been received
 */
static inline bool rvx_uart_data_available(RvxUart *uart_address)
{
  return (uart_address->RVX_UART_STATUS_REG >> 1) & 1;
}

/**
 * @brief Read the most recent byte received by the UART controller and clears the UART interrupt.
 *
 * This function is non-blocking and is meant to be called from an interrupt handler for the UART interrupt, which is
 * triggered by the UART controller whenever a new byte is received.
 *
 * If called before any data has been received since power-on or reset, returns `0x00`.
 *
 * If called before the completion of an on-going reception, the last fully received byte is returned.
 *
 * The read is non-destructive. If this function is called multiple times without new data being received,
 * it will return the same byte until a new byte is received.
 *
 * Example usage:
 * ```c
 * void main(void) {
 *   // Enable Fast Interrupt 0 in M-mode
 *   // The UART interrupt is connected to Fast Interrupt 0 by default
 *   rvx_irq_enable(RVX_PRIVILEGE_LEVEL_M, RVX_IRQ_FAST_BITMASK(0));
 *   // Enable interrupts globally at M-mode
 *   rvx_irq_enable_global(RVX_PRIVILEGE_LEVEL_M);
 *   // Wait for an UART interrupt to be received
 *   rvx_wait_for_interrupt();
 * }
 *
 * // Implements the interrupt handler for Fast Interrupt 0
 * RVX_TRAP_HANDLER_M(rvx_trap_handler_fast_irq_0) {
 *   // Read the received byte and clear the interrupt
 *   uint8_t rx_data = rvx_uart_read(RVX_UART_ADDRESS);
 *   // Do something with rx_data ...
 * }
 * ```
 *
 * @param uart_address Base address of the UART controller.
 * @return The most recent byte received by the UART controller, or `0x00` if no data has been received since
 * power-on or reset.
 */
static inline uint8_t rvx_uart_read(RvxUart *uart_address)
{
  return uart_address->RVX_UART_READ_REG;
}

/**
 * @brief Block until a new byte is received by the UART, then read and return that byte.
 *
 * This function is meant to be used in contexts where blocking behavior is acceptable.
 *
 * @note For non-blocking behavior, use `rvx_uart_read()` instead.
 *
 * Example usage:
 * ```c
 * // Blocks until a new byte is received, then reads it
 * uint8_t rx_data = rvx_uart_receive(RVX_UART_ADDRESS);
 * ```
 *
 * @param uart_address Base address of the UART controller.
 * @return The byte received by the UART controller.
 */
static inline uint8_t rvx_uart_receive(RvxUart *uart_address)
{
  while (!rvx_uart_data_available(uart_address))
    ;
  return uart_address->RVX_UART_READ_REG;
}

/**
 * @brief Send a byte over the UART.
 *
 * Blocks until the UART is idle, then sends a byte.
 *
 * Example usage:
 *
 * ```c
 * // Send 'A' over UART
 * rvx_uart_send(RVX_UART_ADDRESS, 'A');
 *
 * // Send 'B', busy-waits until 'A' has been fully transmitted
 * rvx_uart_send(RVX_UART_ADDRESS, 'B');
 * ```
 *
 * @param uart_address Base address of the UART controller.
 * @param data The byte to write (`uint8_t`).
 */
static inline void rvx_uart_send(RvxUart *uart_address, uint8_t data)
{
  rvx_uart_wait_tx_done(uart_address);
  uart_address->RVX_UART_WRITE_REG = data;
}

/**
 * @brief Send a null-terminated C string over the UART.
 *
 * Transmits each character in `c_str` sequentially until the null terminator (`'\0'`)
 * is reached. This function busy-waits for the UART to be ready before sending each byte.
 *
 * Example usage:
 * ```c
 * // Send the string "Hello, World!" over UART, waiting if necessary
 * rvx_uart_send_string(RVX_UART_ADDRESS, "Hello, World!");
 * ```
 *
 * @param uart_address Base address of the UART controller.
 * @param c_str Pointer to the null-terminated C string to transmit.
 */
static inline void rvx_uart_send_string(RvxUart *uart_address, const char *c_str)
{
  while (*c_str)
  {
    rvx_uart_send(uart_address, *c_str++);
  }
}

#endif // __RVX_UART_H