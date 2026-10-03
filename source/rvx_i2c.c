// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#include "rvx_i2c.h"

static bool rvx_i2c_start(RvxI2c *i2c)
{
  if ((i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_SDA_MASK) == 0U)
    return false;
  if ((i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_SCL_MASK) == 0U)
    return false;
  if ((i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_MASK) != 0U)
    return false;
  i2c->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_START;
  i2c->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_MASK;
  while (i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_MASK)
    ;
  return true;
}

static void rvx_i2c_repeated_start(RvxI2c *i2c)
{
  i2c->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_RESTART;
  i2c->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_MASK;
  while (i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_MASK)
    ;
}

static void rvx_i2c_stop(RvxI2c *i2c)
{
  i2c->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_STOP;
  i2c->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_RUN_MASK;
  while (i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_MASK)
    ;
}

static bool rvx_i2c_write_byte(RvxI2c *i2c, const uint8_t tx_data)
{
  i2c->RVX_I2C_DATA_REG = tx_data;
  i2c->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_DATA;
  i2c->RVX_I2C_STATUS_REG = RVX_I2C_STATUS_ACK_MASK | RVX_I2C_STATUS_RUN_MASK;
  while (i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_MASK)
    ;
  return (i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_ACK_MASK) == 0U;
}

static uint8_t rvx_i2c_read_byte(RvxI2c *i2c, bool send_ack)
{
  i2c->RVX_I2C_DATA_REG = 0xffU;
  i2c->RVX_I2C_COMMAND_REG = RVX_I2C_COMMAND_DATA;
  i2c->RVX_I2C_STATUS_REG = (send_ack ? 0U : RVX_I2C_STATUS_ACK_MASK) | RVX_I2C_STATUS_RUN_MASK;
  while (i2c->RVX_I2C_STATUS_REG & RVX_I2C_STATUS_RUN_MASK)
    ;
  return (uint8_t)i2c->RVX_I2C_DATA_REG;
}

bool rvx_i2c_write(RvxI2c *i2c, uint8_t target_address, const uint8_t *tx_buffer, size_t tx_length)
{
  if (!rvx_i2c_start(i2c))
    return false;
  if (!rvx_i2c_write_byte(i2c, (uint8_t)(target_address << 1)))
  {
    rvx_i2c_stop(i2c);
    return false;
  }
  for (size_t i = 0; i < tx_length; i++)
  {
    if (!rvx_i2c_write_byte(i2c, tx_buffer[i]))
    {
      rvx_i2c_stop(i2c);
      return false;
    }
  }
  rvx_i2c_stop(i2c);
  return true;
}

bool rvx_i2c_read(RvxI2c *i2c, uint8_t target_address, uint8_t *rx_buffer, size_t rx_length)
{
  if (!rvx_i2c_start(i2c))
    return false;
  if (!rvx_i2c_write_byte(i2c, (uint8_t)((target_address << 1) | 1U)))
  {
    rvx_i2c_stop(i2c);
    return false;
  }
  for (size_t i = 0; i < rx_length; i++)
  {
    rx_buffer[i] = rvx_i2c_read_byte(i2c, i < rx_length - 1U);
  }
  rvx_i2c_stop(i2c);
  return true;
}

bool rvx_i2c_write_read(RvxI2c *i2c, uint8_t target_address, const uint8_t *tx_buffer, size_t tx_length,
                        uint8_t *rx_buffer, size_t rx_length)
{
  if (!rvx_i2c_start(i2c))
    return false;
  if (!rvx_i2c_write_byte(i2c, (uint8_t)(target_address << 1)))
  {
    rvx_i2c_stop(i2c);
    return false;
  }
  for (size_t i = 0; i < tx_length; i++)
  {
    if (!rvx_i2c_write_byte(i2c, tx_buffer[i]))
    {
      rvx_i2c_stop(i2c);
      return false;
    }
  }
  rvx_i2c_repeated_start(i2c);
  if (!rvx_i2c_write_byte(i2c, (uint8_t)((target_address << 1) | 1U)))
  {
    rvx_i2c_stop(i2c);
    return false;
  }
  for (size_t i = 0; i < rx_length; i++)
  {
    rx_buffer[i] = rvx_i2c_read_byte(i2c, i < rx_length - 1U);
  }
  rvx_i2c_stop(i2c);
  return true;
}
