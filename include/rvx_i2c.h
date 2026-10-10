// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_I2C_H
#define __RVX_I2C_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// @name Bit masks for I2C Status register.
/// @{
#define RVX_I2C_STATUS_RUN_MASK (1U << 0U) ///< I2C transaction-in-progress status bit.
#define RVX_I2C_STATUS_ACK_MASK (1U << 1U) ///< I2C acknowledge status bit.
#define RVX_I2C_STATUS_IRQ_MASK (1U << 2U) ///< I2C interrupt status bit.
#define RVX_I2C_STATUS_SDA_MASK (1U << 3U) ///< I2C SDA line status bit.
#define RVX_I2C_STATUS_SCL_MASK (1U << 4U) ///< I2C SCL line status bit.
/// @}

/// The I2C command.
typedef enum RvxI2cCommand
{
  RVX_I2C_COMMAND_NOP = 0,     ///< I2C No operation (NOP) command.
  RVX_I2C_COMMAND_START = 1,   ///< I2C Command start.
  RVX_I2C_COMMAND_RESTART = 2, ///< I2C Command restart.
  RVX_I2C_COMMAND_STOP = 3,    ///< I2C Command stop.
  RVX_I2C_COMMAND_DATA = 4     ///< I2C Command data.
} RvxI2cCommand;

/**
 * @brief Structure representing the I2C controller registers.
 *
 * The fields are laid out in the same order as the hardware registers, allowing direct access through a pointer.
 *
 * For example:
 *
 * ```c
 * // Macro RVX_I2C0 expands to the base address of I2C0: ((RvxI2c *)0x40004000U).
 * uint32_t status = RVX_I2C0->RVX_I2C_STATUS_REG; // Read the status register of I2C0.
 * ```
 */
typedef struct RVX_ALIGNED RvxI2c
{
  volatile uint32_t RVX_I2C_DIVIDER_REG; ///< RVX I2C Divider Register.
  volatile uint32_t RVX_I2C_DATA_REG;    ///< RVX I2C Data Register.
  volatile uint32_t RVX_I2C_COMMAND_REG; ///< RVX I2C Command Register.
  volatile uint32_t RVX_I2C_STATUS_REG;  ///< RVX I2C Status Register.
} RvxI2c;

/**
 * @brief Set the clock divider for the I2C controller, which determines the I2C clock pin frequency.
 *
 * The `clock_divider` value must be an even integer between 2 and 65534 (inclusive). Values outside this range or odd
 * values will be ignored without error, leaving the I2C clock unchanged.
 *
 * The I2C clock frequency will be equal to the RVX clock frequency divided by the clock divider.
 *
 * `i2c_clock_frequency = rvx_clock_frequency / clock_divider`
 *
 * Example usage:
 * ```c
 * // RVX initialization (adjust the clock frequency as needed)
 * const RvxSetup rvx_setup = {.clock_frequency = 12000000U, .trap_handler = RVX_DEFAULT_TRAP_HANDLER};
 * rvx_init(&rvx_setup);
 *
 * // Set the I2C clock divider to configure the I2C clock frequency to 100 kHz (12000000 / 120 = 100000).
 * // Macro RVX_I2C0 expands to the base address of I2C0: ((RvxI2c *)0x40004000U).
 * rvx_i2c_set_clock_divider(RVX_I2C0, 120);
 * ```
 *
 */
static inline void rvx_i2c_set_clock_divider(RvxI2c *i2c, uint16_t clock_divider)
{
  if (clock_divider < 2U || clock_divider > 65534U || clock_divider % 2U != 0U)
    return;
  i2c->RVX_I2C_DIVIDER_REG = (clock_divider / 2U) - 1U;
}

/**
 * @brief Write data to an I2C peripheral device.
 *
 * This function initiates an I2C write transaction to the specified peripheral device, sending the provided data
 * buffer over the I2C bus.
 *
 * This function is blocking: it waits for the I2C controller to complete each operation and the full transaction before
 * returning. The controller is polled without a timeout.
 *
 * If the bus is busy, the function will return `false` without attempting the transaction.
 *
 * If the peripheral address is not acknowledged by the target device, or if any byte in the data buffer is not
 * acknowledged during transmission, the function will return `false` and will ensure that a STOP condition is sent to
 * release the bus.
 *
 * Example usage:
 * ```c
 * // Example data buffer to send
 * uint8_t tx_buffer[] = {0x01, 0x02, 0x03};
 *
 * // 7-bit address of the target I2C peripheral device
 * uint8_t target_address = 0x52;
 *
 * // Send the data buffer.
 * bool success = rvx_i2c_write(RVX_I2C0, target_address, tx_buffer, sizeof(tx_buffer));
 * ```
 *
 * @param i2c Pointer to the base address of the I2C registers.
 * @param target_address 7-bit I2C address of the target device (without the R/W bit).
 * @param tx_buffer Pointer to the data buffer to be sent.
 * @param tx_length Number of bytes to be sent from the data buffer.
 * @return true if the write operation was successful, false otherwise.
 */
bool rvx_i2c_write(RvxI2c *i2c, uint8_t target_address, const uint8_t *tx_buffer, size_t tx_length);

/**
 * @brief Read data from an I2C peripheral device.
 *
 * This function initiates an I2C read transaction from the specified peripheral device, receiving data into the
 * provided buffer over the I2C bus.
 *
 * This function is blocking: it waits for the I2C controller to complete each operation and the full transaction before
 * returning. The controller is polled without a timeout.
 *
 * If the bus is busy, the function will return `false` without attempting the transaction.
 *
 * If the peripheral address is not acknowledged by the target device, the function will return `false` and will ensure
 * that a STOP condition is sent to release the bus.
 *
 * Example usage:
 * ```c
 * // Buffer to store received data
 * uint8_t rx_buffer[3];
 *
 * // 7-bit address of the target I2C peripheral device
 * uint8_t target_address = 0x52;
 *
 * // Read data into the buffer.
 * bool success = rvx_i2c_read(RVX_I2C0, target_address, rx_buffer, sizeof(rx_buffer));
 * ```
 *
 * @param i2c Pointer to the base address of the I2C registers.
 * @param target_address 7-bit I2C address of the target device (without the R/W bit).
 * @param rx_buffer Pointer to the buffer where received data will be stored.
 * @param rx_length Number of bytes to be read into the buffer.
 * @return true if the read operation was successful, false otherwise.
 */
bool rvx_i2c_read(RvxI2c *i2c, uint8_t target_address, uint8_t *rx_buffer, size_t rx_length);

/**
 * @brief Perform a combined I2C write followed by a read operation.
 *
 * This function initiates an I2C write transaction to the specified peripheral device, followed by a repeated start
 * condition and an I2C read transaction. The write data is sent first, and then the read data is received into the
 * provided buffer.
 *
 * This function is blocking: it waits for the I2C controller to complete each operation and the full transaction before
 * returning. The controller is polled without a timeout.
 *
 * If the bus is busy, the function will return `false` without attempting the transaction.
 *
 * If the peripheral address is not acknowledged by the target device during either the write or read phase, the
 * function will return `false` and will ensure that a STOP condition is sent to release the bus.
 *
 * Example usage:
 * ```c
 * // Data buffers
 * uint8_t tx_buffer[] = {0x01}; // Data to write
 * uint8_t rx_buffer[6]; // Buffer to store received data
 *
 * // 7-bit address of the target I2C peripheral device
 * uint8_t target_address = 0x52;
 *
 * // Perform a write-read transaction: write 1 byte from tx_buffer, then read 6 bytes into rx_buffer.
 * bool success = rvx_i2c_write_read(RVX_I2C0, target_address, tx_buffer, 1, rx_buffer, 6);
 * ```
 *
 * @param i2c Pointer to the base address of the I2C registers.
 * @param target_address 7-bit I2C address of the target device (without the R/W bit).
 * @param tx_buffer Pointer to the data buffer to be sent.
 * @param tx_length Number of bytes to be sent from the `tx_buffer`.
 * @param rx_buffer Pointer to the buffer where received data will be stored.
 * @param rx_length Number of bytes to be read into the `rx_buffer`.
 * @return true if the write-read operation was successful, false otherwise.
 */
bool rvx_i2c_write_read(RvxI2c *i2c, uint8_t target_address, const uint8_t *tx_buffer, size_t tx_length,
                        uint8_t *rx_buffer, size_t rx_length);

#endif // __RVX_I2C_H