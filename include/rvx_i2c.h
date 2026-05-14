// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_I2C_H
#define __RVX_I2C_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// The I2C command.
typedef enum RvxI2cCommand
{
  RVX_I2C_COMMAND_NOP = 0,     ///< I2C No operation (NOP) command.
  RVX_I2C_COMMAND_START = 1,   ///< I2C Command start.
  RVX_I2C_COMMAND_RESTART = 2, ///< I2C Command restart.
  RVX_I2C_COMMAND_STOP = 3,    ///< I2C Command stop.
  RVX_I2C_COMMAND_DATA = 4     ///< I2C Command data.
} RvxI2cCommand;

/// Provide access to the I2C controller registers.
typedef struct RVX_ALIGNED RvxI2c
{
  volatile uint32_t RVX_I2C_DIVIDER_REG; ///< RVX I2C Divider Register.
  volatile uint32_t RVX_I2C_DATA_REG;    ///< RVX I2C Data Register.
  volatile uint32_t RVX_I2C_COMMAND_REG; ///< RVX I2C Command Register.
  volatile uint32_t RVX_I2C_STATUS_REG;  ///< RVX I2C Status Register.
} RvxI2c;

/**
 * @brief Set the clock divider for the I2C controller, adjusting the frequency of the I2C clock signal and consequently
 * the data transfer rate.
 *
 * The actual I2C clock frequency is determined by the formula:
 *
 * - `scl_freq = rvx_clock_freq / clock_divider`
 *
 * Where:
 *
 * - `scl_freq` is the resulting I2C clock frequency (SCL line frequency).
 *
 * - `rvx_clock_freq` is the frequency of the clock driving RVX (and thus the I2C controller).
 *
 * - `clock_divider` is the even integer value passed to this function, which must be between 2 and 65534 (inclusive).
 *
 * If an odd value is passed for `clock_divider`, it will be rounded down to the nearest even integer without error.
 *
 * If a value outside the valid range is passed for `clock_divider`, it will be rounded to the nearest valid value (2 or
 * 65534) without error.
 *
 * Example usage:
 * ```c
 * // Divide RVX clock by 120 for I2C communication.
 * // For example, if RVX is connected to a 12MHz clock source, the resulting I2C speed will be 100kHz.
 * rvx_i2c_set_divider(RVX_I2C_ADDRESS, 120);
 * ```
 *
 * @note After reset, the `clock_divider` is set to 2 by default.
 *
 * @param i2c_address Base address of the I2C controller.
 * @param clock_divider Even integer between 2 and 65534 (inclusive) that determines the I2C SCL pin frequency.
 *
 */
static inline void rvx_i2c_set_divider(RvxI2c *i2c_address, const uint16_t clock_divider)
{
  i2c_address->RVX_I2C_DIVIDER_REG = (clock_divider >> 1) - 1;
}

/**
 * @brief Start an I2C transaction.
 *
 * This function initiates an I2C transaction by setting the START condition on the I2C bus.
 *
 * If the I2C bus is currently busy (SDA or SCL lines are low, or a transaction is already in progress), this function
 * will return `false` and will not attempt to start a new transaction.
 *
 * If the bus is idle, the START condition will be initiated and the function will return `true`.
 *
 * @note This is a low-level function that only sends the START condition. For a complete I2C transaction,
 * use `rvx_i2c_write()`, `rvx_i2c_read()` or `rvx_i2c_write_read()`.
 *
 * @param i2c_address Base address of the I2C controller.
 */
static inline bool rvx_i2c_start(RvxI2c *i2c_address)
{
  if ((i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_SDA_BITMASK) == 0)
    return false;
  if ((i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_SCL_BITMASK) == 0)
    return false;
  if ((i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK) == 1)
    return false;
  i2c_address->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_START;
  i2c_address->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
  return true;
}

/**
 * @brief Send a repeated START condition on the I2C bus.
 *
 * This function is used to initiate a repeated START condition, which is typically used in multi-byte
 * I2C transactions without releasing the bus.
 *
 * @note This is a low-level function that only sends the repeated START condition. For a complete I2C transaction,
 * use `rvx_i2c_write()`, `rvx_i2c_read()` or `rvx_i2c_write_read()`.
 *
 * @param i2c_address Base address of the I2C controller.
 */
static inline void rvx_i2c_repeated_start(RvxI2c *i2c_address)
{
  i2c_address->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_RESTART;
  i2c_address->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
}

/**
 * @brief Send a STOP condition on the I2C bus, ending the current transaction and releasing the bus.
 *
 * This function should be called at the end of an I2C transaction to signal to the peripheral device(s) that the
 * transaction is complete.
 *
 * @note This is a low-level function that only sends the STOP condition. For a complete I2C transaction,
 * use `rvx_i2c_write()`, `rvx_i2c_read()` or `rvx_i2c_write_read()`.
 *
 * @param i2c_address Base address of the I2C controller.
 */
static inline void rvx_i2c_stop(RvxI2c *i2c_address)
{
  i2c_address->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_STOP;
  i2c_address->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
}

/**
 * @brief Send a byte of data over the I2C bus.
 *
 * The byte can be either an address byte (with the R/W bit) or a data byte, depending on the context of the
 * transaction.
 *
 * If the byte was acknowledged by the peripheral device (ACK received), the function returns `true`. If the byte was
 * not acknowledged (NACK received), the function returns `false`.
 *
 * @note This is a low-level function. See `rvx_i2c_write()` for a higher-level function that handles a complete I2C
 * write transaction, including sending the START condition, address byte, data bytes, and STOP condition.
 *
 * @param i2c_address Base address of the I2C controller.
 * @param byte Byte to be sent over the I2C bus.
 * @return true if the byte was acknowledged by the peripheral device, false otherwise.
 */
static inline bool rvx_i2c_send_byte(RvxI2c *i2c_address, const uint8_t byte)
{
  i2c_address->RVX_I2C_DATA_REG = byte;
  i2c_address->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_DATA;
  i2c_address->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_ACK_BITMASK | RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
  return (i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_ACK_BITMASK) == 0;
}

/**
 * @brief Receive a byte of data over the I2C bus.
 *
 * This function reads a byte from the I2C bus. The `send_ack` parameter determines whether an ACK or NACK is sent
 * after receiving the byte.
 *
 * If `send_ack` is `true`, an ACK is sent to the peripheral device, indicating that the byte was received successfully
 * and that the master wishes to continue receiving more data. If `send_ack` is `false`, a NACK is sent, indicating that
 * the master does not wish to receive more data after this byte (e.g., because it is the last byte in the transaction).
 *
 * @note This is a low-level function. See `rvx_i2c_read()` for a higher-level function that handles a complete I2C read
 * transaction, including sending the START condition, address byte, receiving data bytes, and sending the STOP
 * condition.
 *
 * @param i2c_address Base address of the I2C controller.
 * @param send_ack If true, an ACK is sent after receiving the byte; if false, a NACK is sent.
 * @return The byte received from the I2C bus.
 */
static inline uint8_t rvx_i2c_receive_byte(RvxI2c *i2c_address, bool send_ack)
{
  i2c_address->RVX_I2C_DATA_REG = 0xff;
  i2c_address->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_DATA;
  i2c_address->RVX_I2C_STATUS_REG = (send_ack ? 0 : RVX_I2C_STATUS_ACK_BITMASK) | RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_address->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
  return i2c_address->RVX_I2C_DATA_REG;
}

/**
 * @brief Write data to an I2C peripheral device.
 *
 * This function initiates an I2C write transaction to the specified peripheral device, sending the provided data
 * buffer over the I2C bus.
 *
 * If the bus is busy, the function will return `false` without attempting the transaction.
 *
 * If the peripheral address is not acknowledged by the target device, or if any byte in the data buffer is not
 * acknowledged during transmission, the function will return `false` and will ensure that a STOP condition is sent to
 * release the bus.
 *
 * Example usage:
 * ```c
 * uint8_t data_to_send[] = {0x01, 0x02, 0x03}; // Example data buffer to send
 * uint8_t peripheral_address = 0x52; // 7-bit I2C address of the target peripheral device
 * bool success = rvx_i2c_write(RVX_I2C_ADDRESS, peripheral_address, data_to_send, sizeof(data_to_send));
 * ```
 *
 * @param i2c_controller_address Base address of the I2C controller.
 * @param i2c_peripheral_address 7-bit I2C address of the target peripheral device (without the R/W bit).
 * @param data Pointer to the data buffer to be sent.
 * @param length Number of bytes to be sent from the data buffer.
 * @return true if the write operation was successful, false otherwise.
 */
static inline bool rvx_i2c_write(RvxI2c *i2c_controller_address, uint8_t i2c_peripheral_address, const uint8_t *data,
                                 size_t length)
{
  if (!rvx_i2c_start(i2c_controller_address))
    return false;
  if (!rvx_i2c_send_byte(i2c_controller_address, (i2c_peripheral_address << 1) | 0))
  {
    rvx_i2c_stop(i2c_controller_address);
    return false;
  }
  for (size_t i = 0; i < length; i++)
  {
    if (!rvx_i2c_send_byte(i2c_controller_address, data[i]))
    {
      rvx_i2c_stop(i2c_controller_address);
      return false;
    }
  }
  rvx_i2c_stop(i2c_controller_address);
  return true;
}

/**
 * @brief Read data from an I2C peripheral device.
 *
 * This function initiates an I2C read transaction from the specified peripheral device, receiving data into the
 * provided buffer over the I2C bus.
 *
 * If the bus is busy, the function will return `false` without attempting the transaction.
 *
 * If the peripheral address is not acknowledged by the target device, the function will return `false` and will ensure
 * that a STOP condition is sent to release the bus.
 *
 * Example usage:
 * ```c
 * uint8_t data_buffer[3]; // Buffer to store received data
 * uint8_t peripheral_address = 0x52; // 7-bit I2C address of the target peripheral device
 * bool success = rvx_i2c_read(RVX_I2C_ADDRESS, peripheral_address, data_buffer, sizeof(data_buffer));
 * ```
 *
 * @param i2c_controller_address Base address of the I2C controller.
 * @param i2c_peripheral_address 7-bit I2C address of the target peripheral device (without the R/W bit).
 * @param data Pointer to the buffer where received data will be stored.
 * @param length Number of bytes to be read into the buffer.
 * @return true if the read operation was successful, false otherwise.
 */
static inline bool rvx_i2c_read(RvxI2c *i2c_controller_address, uint8_t i2c_peripheral_address, uint8_t *data,
                                size_t length)
{
  if (!rvx_i2c_start(i2c_controller_address))
    return false;
  if (!rvx_i2c_send_byte(i2c_controller_address, (i2c_peripheral_address << 1) | 1))
  {
    rvx_i2c_stop(i2c_controller_address);
    return false;
  }
  for (size_t i = 0; i < length; i++)
  {
    data[i] = rvx_i2c_receive_byte(i2c_controller_address, i < length - 1);
  }
  rvx_i2c_stop(i2c_controller_address);
  return true;
}

/**
 * @brief Perform a combined I2C write followed by a read operation.
 *
 * This function initiates an I2C write transaction to the specified peripheral device, followed by a repeated start
 * condition and an I2C read transaction. The write data is sent first, and then the read data is received into the
 * provided buffer.
 *
 * If the bus is busy, the function will return `false` without attempting the transaction.
 *
 * If the peripheral address is not acknowledged by the target device during either the write or read phase, the
 * function will return `false` and will ensure that a STOP condition is sent to release the bus.
 *
 * Example usage:
 * ```c
 * uint8_t write_data[] = {0x01}; // Data to write
 * uint8_t read_data[6]; // Buffer to store received data
 * uint8_t peripheral_address = 0x52; // 7-bit I2C address of the target peripheral device
 * bool success = rvx_i2c_write_read(RVX_I2C_ADDRESS, peripheral_address, write_data, 1, read_data, 6);
 * ```
 *
 * @param i2c_controller_address Base address of the I2C controller.
 * @param i2c_peripheral_address 7-bit I2C address of the target peripheral device (without the R/W bit).
 * @param write_data Pointer to the data buffer to be sent.
 * @param write_length Number of bytes to be sent from the write data buffer.
 * @param read_data Pointer to the buffer where received data will be stored.
 * @param read_length Number of bytes to be read into the buffer.
 * @return true if the write-read operation was successful, false otherwise.
 */
static inline bool rvx_i2c_write_read(RvxI2c *i2c_controller_address, uint8_t i2c_peripheral_address,
                                      const uint8_t *write_data, size_t write_length, uint8_t *read_data,
                                      size_t read_length)
{
  if (!rvx_i2c_start(i2c_controller_address))
    return false;
  if (!rvx_i2c_send_byte(i2c_controller_address, (i2c_peripheral_address << 1) | 0))
  {
    rvx_i2c_stop(i2c_controller_address);
    return false;
  }
  for (size_t i = 0; i < write_length; i++)
  {
    if (!rvx_i2c_send_byte(i2c_controller_address, write_data[i]))
    {
      rvx_i2c_stop(i2c_controller_address);
      return false;
    }
  }
  rvx_i2c_repeated_start(i2c_controller_address);
  if (!rvx_i2c_send_byte(i2c_controller_address, (i2c_peripheral_address << 1) | 1))
  {
    rvx_i2c_stop(i2c_controller_address);
    return false;
  }
  for (size_t i = 0; i < read_length; i++)
  {
    read_data[i] = rvx_i2c_receive_byte(i2c_controller_address, i < read_length - 1);
  }
  rvx_i2c_stop(i2c_controller_address);
  return true;
}

#endif // __RVX_I2C_H