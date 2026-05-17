// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_I2C_H
#define __RVX_I2C_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Base address of the I2C controller registers.
#define RVX_I2C_CONTROLLER_ADDRESS 0x40004000U

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
typedef struct RVX_ALIGNED RvxI2cRegs
{
  volatile uint32_t RVX_I2C_DIVIDER_REG; ///< RVX I2C Divider Register.
  volatile uint32_t RVX_I2C_DATA_REG;    ///< RVX I2C Data Register.
  volatile uint32_t RVX_I2C_COMMAND_REG; ///< RVX I2C Command Register.
  volatile uint32_t RVX_I2C_STATUS_REG;  ///< RVX I2C Status Register.
} RvxI2cRegs;

/**
 * @brief Set the clock divider for the I2C controller.
 *
 * The clock divider determines the frequency of I2C communication according to the formula `scl_freq = rvx_clock_freq /
 * clock_divider`, where `scl_freq` is the resulting I2C clock frequency, `rvx_clock_freq` is the frequency of the
 * clock driving RVX, and `clock_divider` is the even integer value passed to this function.
 *
 * The `clock_divider` value must be between 2 and 65534 (inclusive).
 *
 * If a value outside the valid range is passed for `clock_divider`, it will be rounded to the nearest valid value (2 or
 * 65534) without error.
 *
 * If an odd value is passed for `clock_divider`, it will be rounded down to the nearest even integer without error.
 *
 * Example usage:
 * ```c
 * // Pointer to the I2C controller registers.
 * RvxI2cRegs *i2c_controller = (RvxI2cRegs *)RVX_I2C_CONTROLLER_ADDRESS;
 *
 * // Divide RVX clock by 120 for I2C communication.
 * // Example: if RVX clock is 12MHz, the I2C speed will be 100kHz.
 * rvx_i2c_set_divider(i2c_controller, 120);
 * ```
 *
 * @note After reset, the `clock_divider` is set to 2 by default.
 *
 * @param i2c_controller Pointer to the I2C controller registers.
 * @param clock_divider Even integer between 2 and 65534 (inclusive) that determines the I2C SCL pin frequency.
 *
 */
static inline void rvx_i2c_set_divider(RvxI2cRegs *i2c_controller, uint16_t clock_divider)
{
  if (clock_divider < 2)
    clock_divider = 2;
  i2c_controller->RVX_I2C_DIVIDER_REG = (clock_divider >> 1) - 1;
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
 * @param i2c_controller Pointer to the I2C controller registers.
 * @return true if the START condition was successfully initiated, false otherwise (e.g., if the bus was busy).
 */
static inline bool rvx_i2c_start(RvxI2cRegs *i2c_controller)
{
  if ((i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_SDA_BITMASK) == 0)
    return false;
  if ((i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_SCL_BITMASK) == 0)
    return false;
  if ((i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK) == 1)
    return false;
  i2c_controller->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_START;
  i2c_controller->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
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
 * @param i2c_controller Pointer to the I2C controller registers.
 */
static inline void rvx_i2c_repeated_start(RvxI2cRegs *i2c_controller)
{
  i2c_controller->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_RESTART;
  i2c_controller->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
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
 * @param i2c_controller Pointer to the I2C controller registers.
 */
static inline void rvx_i2c_stop(RvxI2cRegs *i2c_controller)
{
  i2c_controller->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_STOP;
  i2c_controller->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
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
 * @param i2c_controller Pointer to the I2C controller registers.
 * @param byte Byte to be sent over the I2C bus.
 * @return true if the byte was acknowledged by the peripheral device, false otherwise.
 */
static inline bool rvx_i2c_send_byte(RvxI2cRegs *i2c_controller, const uint8_t byte)
{
  i2c_controller->RVX_I2C_DATA_REG = byte;
  i2c_controller->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_DATA;
  i2c_controller->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_ACK_BITMASK | RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
  return (i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_ACK_BITMASK) == 0;
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
 * @param i2c_controller Pointer to the I2C controller registers.
 * @param send_ack If true, an ACK is sent after receiving the byte; if false, a NACK is sent.
 * @return The byte received from the I2C bus.
 */
static inline uint8_t rvx_i2c_receive_byte(RvxI2cRegs *i2c_controller, bool send_ack)
{
  i2c_controller->RVX_I2C_DATA_REG = 0xff;
  i2c_controller->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_DATA;
  i2c_controller->RVX_I2C_STATUS_REG = (send_ack ? 0 : RVX_I2C_STATUS_ACK_BITMASK) | RVX_I2C_STATUS_RUN_BITMASK;
  while (i2c_controller->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_BITMASK)
    ;
  return i2c_controller->RVX_I2C_DATA_REG;
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
 * // Pointer to the I2C controller registers.
 * RvxI2cRegs *i2c_controller = (RvxI2cRegs *)RVX_I2C_CONTROLLER_ADDRESS;
 *
 * // Example data buffer to send
 * uint8_t tx_buffer[] = {0x01, 0x02, 0x03};
 *
 * // 7-bit address of the target I2C peripheral device
 * uint8_t peripheral_address = 0x52;
 *
 * // Send the data buffer.
 * bool success = rvx_i2c_write(i2c_controller, peripheral_address, tx_buffer, sizeof(tx_buffer));
 * ```
 *
 * @param i2c_controller Pointer to the I2C controller registers.
 * @param i2c_peripheral_address 7-bit I2C address of the target peripheral device (without the R/W bit).
 * @param tx_buffer Pointer to the data buffer to be sent.
 * @param tx_length Number of bytes to be sent from the data buffer.
 * @return true if the write operation was successful, false otherwise.
 */
static inline bool rvx_i2c_write(RvxI2cRegs *i2c_controller, uint8_t i2c_peripheral_address, const uint8_t *tx_buffer,
                                 size_t tx_length)
{
  if (!rvx_i2c_start(i2c_controller))
    return false;
  if (!rvx_i2c_send_byte(i2c_controller, (i2c_peripheral_address << 1) | 0))
  {
    rvx_i2c_stop(i2c_controller);
    return false;
  }
  for (size_t i = 0; i < tx_length; i++)
  {
    if (!rvx_i2c_send_byte(i2c_controller, tx_buffer[i]))
    {
      rvx_i2c_stop(i2c_controller);
      return false;
    }
  }
  rvx_i2c_stop(i2c_controller);
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
 * // Pointer to the I2C controller registers.
 * RvxI2cRegs *i2c_controller = (RvxI2cRegs *)RVX_I2C_CONTROLLER_ADDRESS;
 *
 * // Buffer to store received data
 * uint8_t rx_buffer[3];
 *
 * // 7-bit address of the target I2C peripheral device
 * uint8_t peripheral_address = 0x52;
 *
 * // Read data into the buffer.
 * bool success = rvx_i2c_read(i2c_controller, peripheral_address, rx_buffer, sizeof(rx_buffer));
 * ```
 *
 * @param i2c_controller Pointer to the I2C controller registers.
 * @param i2c_peripheral_address 7-bit I2C address of the target peripheral device (without the R/W bit).
 * @param rx_buffer Pointer to the buffer where received data will be stored.
 * @param rx_length Number of bytes to be read into the buffer.
 * @return true if the read operation was successful, false otherwise.
 */
static inline bool rvx_i2c_read(RvxI2cRegs *i2c_controller, uint8_t i2c_peripheral_address, uint8_t *rx_buffer,
                                size_t rx_length)
{
  if (!rvx_i2c_start(i2c_controller))
    return false;
  if (!rvx_i2c_send_byte(i2c_controller, (i2c_peripheral_address << 1) | 1))
  {
    rvx_i2c_stop(i2c_controller);
    return false;
  }
  for (size_t i = 0; i < rx_length; i++)
  {
    rx_buffer[i] = rvx_i2c_receive_byte(i2c_controller, i < rx_length - 1);
  }
  rvx_i2c_stop(i2c_controller);
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
 * // Pointer to the I2C controller registers.
 * RvxI2cRegs *i2c_controller = (RvxI2cRegs *)RVX_I2C_CONTROLLER_ADDRESS;
 *
 * // Data buffers
 * uint8_t tx_buffer[] = {0x01}; // Data to write
 * uint8_t rx_buffer[6]; // Buffer to store received data
 *
 * // 7-bit address of the target I2C peripheral device
 * uint8_t peripheral_address = 0x52;
 *
 * // Perform a write-read transaction: write 1 byte from tx_buffer, then read 6 bytes into rx_buffer.
 * bool success = rvx_i2c_write_read(i2c_controller, peripheral_address, tx_buffer, 1, rx_buffer, 6);
 * ```
 *
 * @param i2c_controller Pointer to the I2C controller registers.
 * @param i2c_peripheral_address 7-bit I2C address of the target peripheral device (without the R/W bit).
 * @param tx_buffer Pointer to the data buffer to be sent.
 * @param tx_length Number of bytes to be sent from the `tx_buffer`.
 * @param rx_buffer Pointer to the buffer where received data will be stored.
 * @param rx_length Number of bytes to be read into the `rx_buffer`.
 * @return true if the write-read operation was successful, false otherwise.
 */
static inline bool rvx_i2c_write_read(RvxI2cRegs *i2c_controller, uint8_t i2c_peripheral_address,
                                      const uint8_t *tx_buffer, size_t tx_length, uint8_t *rx_buffer, size_t rx_length)
{
  if (!rvx_i2c_start(i2c_controller))
    return false;
  if (!rvx_i2c_send_byte(i2c_controller, (i2c_peripheral_address << 1) | 0))
  {
    rvx_i2c_stop(i2c_controller);
    return false;
  }
  for (size_t i = 0; i < tx_length; i++)
  {
    if (!rvx_i2c_send_byte(i2c_controller, tx_buffer[i]))
    {
      rvx_i2c_stop(i2c_controller);
      return false;
    }
  }
  rvx_i2c_repeated_start(i2c_controller);
  if (!rvx_i2c_send_byte(i2c_controller, (i2c_peripheral_address << 1) | 1))
  {
    rvx_i2c_stop(i2c_controller);
    return false;
  }
  for (size_t i = 0; i < rx_length; i++)
  {
    rx_buffer[i] = rvx_i2c_receive_byte(i2c_controller, i < rx_length - 1);
  }
  rvx_i2c_stop(i2c_controller);
  return true;
}

#endif // __RVX_I2C_H