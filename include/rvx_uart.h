// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_UART_H
#define __RVX_UART_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_csr.h"
#include "rvx_macros.h"

#define RVX_UART_STATUS_TX_READY_MASK 0x1U ///< UART transmit-ready status bit.
#define RVX_UART_STATUS_RX_READY_MASK 0x2U ///< UART receive-ready status bit.

/**
 * @brief Structure representing the UART controller registers.
 *
 * The fields of this structure are laid out in the same order as the hardware registers,
 * allowing direct access to them through a pointer.
 *
 * For example:
 *
 * ```c
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * uint32_t uart_status = RVX_UART0->RVX_UART_STATUS_REG; // Read the status register of UART0.
 * ```
 */
typedef struct RVX_ALIGNED RvxUart
{
  volatile uint32_t RVX_UART_WRITE_REG;  ///< RVX UART Write Register.
  volatile uint32_t RVX_UART_READ_REG;   ///< RVX UART Read Register.
  volatile uint32_t RVX_UART_STATUS_REG; ///< RVX UART Status Register.
  volatile uint32_t RVX_UART_BAUD_REG;   ///< RVX UART Baud Rate Configuration Register.
} RvxUart;

/**
 * @brief Set the UART baud rate to the closest achievable baud rate. Return the number of clock ticks per UART bit.
 *
 * The baud rate is adjusted to the closest achievable baud rate by setting the UART baud rate register to the number of
 * clock ticks per UART bit, which is calculated as follows:
 *
 *  `clock_ticks_per_bit = clock_frequency / baud_rate`
 *
 * The calculated `clock_ticks_per_bit` value is returned. A return value of `0` indicates an error, which can occur in
 * the following cases:
 *
 * - The `baud_rate` parameter is `0`.
 *
 * - The `clock_frequency` parameter is `0`.
 *
 * - The requested baud rate is higher than the `clock_frequency`.
 *
 * In case of an error, the UART baud rate is not changed.
 *
 * The actual baud rate of the UART may differ from the requested baud rate due to the integer division of the clock
 * frequency by the baud rate. The actual baud rate can be calculated as follows:
 *
 *  `actual_baud_rate = clock_frequency / clock_ticks_per_bit`
 *
 * @param uart Pointer to the base address of the UART registers.
 * @param baud_rate The desired baud rate (must be non-zero).
 * @param clock_frequency The RVX clock frequency (must be non-zero).
 * @return The number of clock ticks per UART bit, or `0` in case of an error.
 */
static inline uint32_t rvx_uart_set_baud_rate(RvxUart *uart, uint32_t baud_rate, uint32_t clock_frequency)
{
  if (baud_rate == 0U)
    return 0U;

  if (clock_frequency == 0U)
    return 0U;

  if (baud_rate > clock_frequency)
    return 0U;

  uint32_t clock_ticks_per_bit = clock_frequency / baud_rate;
  uint32_t remainder = clock_frequency % baud_rate;
  if (remainder != 0U && remainder >= (baud_rate / 2U))
  {
    clock_ticks_per_bit += 1U; // Round up if remainder is at least half of the baud rate
  }

  uart->RVX_UART_BAUD_REG = clock_ticks_per_bit;
  return clock_ticks_per_bit;
}

/**
 * @brief Return `true` if the UART is ready to transmit a new byte, or `false` otherwise.
 *
 * Example usage:
 *
 * ```c
 * // Wait until the UART is ready to transmit a new byte, then send it.
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * while (!rvx_uart_tx_ready(RVX_UART0));
 * rvx_uart_write(RVX_UART0, 0x55);
 * ```
 *
 * @param uart Pointer to the base address of the UART registers.
 * @return `true` if the UART is ready to send data, `false` otherwise.
 */
static inline bool rvx_uart_tx_ready(RvxUart *uart)
{
  return (uart->RVX_UART_STATUS_REG & RVX_UART_STATUS_TX_READY_MASK) != 0U;
}

/**
 * @brief Return `true` if the UART has received a new byte, or `false` otherwise.
 *
 * Example usage:
 *
 * ```c
 * // Wait until a new byte is received, then read it.
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * while (!rvx_uart_rx_ready(RVX_UART0));
 * uint8_t rx_data = rvx_uart_read(RVX_UART0);
 * ```
 *
 * @param uart Pointer to the base address of the UART registers.
 * @return `true` if the UART has received a new byte, `false` otherwise.
 */
static inline bool rvx_uart_rx_ready(RvxUart *uart)
{
  return (uart->RVX_UART_STATUS_REG & RVX_UART_STATUS_RX_READY_MASK) != 0U;
}

/**
 * @brief Read the last byte received by the UART and clear the UART interrupt.
 *
 * To check if a new byte has been received before calling this function, use `rvx_uart_rx_ready()`.
 *
 * If this function is called before any data has been received, `0x00` is returned.
 *
 * This function is non-blocking and the read is non-destructive (does not remove the byte from the buffer).
 *
 * Example usage:
 * ```c
 * // Wait until a new byte is received, then read it.
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * while (!rvx_uart_rx_ready(RVX_UART0));
 * uint8_t rx_data = rvx_uart_read(RVX_UART0);
 * ```
 *
 * @param uart Pointer to the base address of the UART registers.
 * @return The last byte received by the UART controller, or `0x00` if called before any data has been received.
 */
static inline uint8_t rvx_uart_read(RvxUart *uart)
{
  return uart->RVX_UART_READ_REG;
}

/**
 * @brief Write a byte to the UART for transmission.
 *
 * To check if the UART is ready to transmit before calling this function, use `rvx_uart_tx_ready()`.
 *
 * This function is non-blocking.
 *
 * If this function is called before the UART is ready to transmit, the byte may be
 * lost.
 *
 * Example usage:
 *
 * ```c
 * // Wait until the UART is ready to transmit, then send a byte.
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * while (!rvx_uart_tx_ready(RVX_UART0));
 * rvx_uart_write(RVX_UART0, 0x55);
 * ```
 *
 * @param uart Pointer to the base address of the UART registers.
 * @param tx_data The byte to write (`uint8_t`).
 */
static inline void rvx_uart_write(RvxUart *uart, uint8_t tx_data)
{
  uart->RVX_UART_WRITE_REG = tx_data;
}

/**
 * @brief Print a null-terminated string over the UART.
 *
 * The string is transmitted as-is. No newline or other characters are appended
 * to the end of the string.
 *
 * This function will block until the UART has transmitted the entire string.
 *
 * Example usage:
 *
 * ```c
 * // Send "Hello, UART!" over UART0.
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * // The call below will block until the entire string is transmitted.
 * rvx_uart_print(RVX_UART0, "Hello, UART!");
 * ```
 *
 * @param uart Pointer to the base address of the UART registers.
 * @param str Pointer to the null-terminated C string to transmit.
 */
static inline void rvx_uart_print(RvxUart *uart, const char *str)
{
  while (*str)
  {
    while (!rvx_uart_tx_ready(uart))
      ;
    rvx_uart_write(uart, *str++);
  }
  while (!rvx_uart_tx_ready(uart))
    ;
}

/**
 * @brief Print a null-terminated string over the UART, followed by a newline character.
 *
 * The string is transmitted as-is, followed by a newline character (`\n`).
 *
 * This function will block until the UART has transmitted the entire string
 * and the newline character.
 *
 * Example usage:
 *
 * ```c
 * // Send "Hello, UART!" followed by a newline over UART0.
 * // Macro RVX_UART0 expands to the base address of UART0: ((RvxUart *)0x40000000U).
 * // The call below will block until the entire string and the newline are transmitted.
 * rvx_uart_println(RVX_UART0, "Hello, UART!");
 * ```
 *
 * @param uart Pointer to the base address of the UART registers.
 * @param str Pointer to the null-terminated C string to transmit.
 */
static inline void rvx_uart_println(RvxUart *uart, const char *str)
{
  rvx_uart_print(uart, str);
  rvx_uart_write(uart, '\n');
  while (!rvx_uart_tx_ready(uart))
    ;
}

#endif // __RVX_UART_H