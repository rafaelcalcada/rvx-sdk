// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_SPI_H
#define __RVX_SPI_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// The SPI mode configuration.
typedef enum RvxSpiMode
{
  RVX_SPI_MODE_0 = 0, ///< SPI Mode 0 (CPOL = 0 / CPHA = 0).
  RVX_SPI_MODE_1 = 1, ///< SPI Mode 1 (CPOL = 0 / CPHA = 1).
  RVX_SPI_MODE_2 = 2, ///< SPI Mode 2 (CPOL = 1 / CPHA = 0).
  RVX_SPI_MODE_3 = 3  ///< SPI Mode 3 (CPOL = 1 / CPHA = 1).
} RvxSpiMode;

/// Provide access to the SPI controller registers.
typedef struct RVX_ALIGNED RvxSpi
{
  volatile uint32_t RVX_SPI_MODE;        ///< RVX SPI Mode Register.
  volatile uint32_t RVX_SPI_CHIP_SELECT; ///< RVX SPI Chip Select register.
  volatile uint32_t RVX_SPI_DIVIDER;     ///< RVX SPI Clock Divider Register.
  volatile uint32_t RVX_SPI_WRITE;       ///< RVX SPI Write Register.
  volatile uint32_t RVX_SPI_READ;        ///< RVX SPI Read Register.
  volatile uint32_t RVX_SPI_STATUS;      ///< RVX SPI Status Register.
} RvxSpi;

/**
 * @brief Set the mode of the SPI controller.
 *
 * This function allows changing the SPI mode to any of the four supported modes:
 *
 * - `RVX_SPI_MODE_0` (CPOL = 0, CPHA = 0)
 *
 * - `RVX_SPI_MODE_1` (CPOL = 0, CPHA = 1)
 *
 * - `RVX_SPI_MODE_2` (CPOL = 1, CPHA = 0)
 *
 * - `RVX_SPI_MODE_3` (CPOL = 1, CPHA = 1)
 *
 * Example usage:
 * ```c
 * // Change the SPI mode to 1 (CPOL = 0, CPHA = 1)
 * rvx_spi_set_mode(RVX_SPI_ADDRESS, RVX_SPI_MODE_1);
 * ```
 *
 * @note After reset, the SPI controller is in mode 0 (CPOL = 0, CPHA = 0) by default.
 *
 * @param spi_address Base address of the SPI controller.
 * @param spi_mode Desired mode as `RvxSpiMode`.
 */
static inline void rvx_spi_set_mode(RvxSpi *spi_address, RvxSpiMode spi_mode)
{
  spi_address->RVX_SPI_MODE = spi_mode;
}

/**
 * @brief Set the clock divider for the SPI controller, adjusting the frequency of the SPI clock signal and
 * consequently the data transfer rate.
 *
 * The SCLK pin frequency is determined by the `clock_divider` parameter according to the formula:
 *
 * - `sclk_freq = rvx_clock_freq / clock_divider`
 *
 * Where:
 *
 * - `sclk_freq` is the resulting SPI clock frequency (SCLK line frequency).
 *
 * - `rvx_clock_freq` is the frequency of the clock driving RVX (and thus the SPI controller).
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
 * // Divide RVX clock by 12 for SPI communication.
 * // For example, if RVX is connected to a 12MHz clock source, the resulting SPI speed will be 1MHz.
 * rvx_spi_set_divider(RVX_SPI_ADDRESS, 12);
 * ```
 *
 * @note After reset, the `clock_divider` is set to 2 by default.
 *
 * @param spi_address Base address of the SPI controller.
 * @param clock_divider Even integer between 2 and 65534 (inclusive) that determines the SCLK pin frequency.
 */
static inline void rvx_spi_set_divider(RvxSpi *spi_address, uint16_t clock_divider)
{
  spi_address->RVX_SPI_DIVIDER = (uint8_t)(clock_divider >> 1) - 1;
}

/**
 * @brief Assert the CS (Chip Select) pin controlled by the SPI controller.
 *
 * The SPI controller provides only one CS pin under its control. To communicate with multiple SPI devices on the same
 * bus, use additional GPIO pins as software-controlled CS lines.
 *
 * @param spi_address Base address of the SPI controller.
 */
static inline void rvx_spi_assert_cs(RvxSpi *spi_address)
{
  spi_address->RVX_SPI_CHIP_SELECT = 0;
}

/**
 * @brief Deassert the CS (Chip Select) pin controlled by the SPI controller.
 *
 * The SPI controller provides only one CS pin under its control. To communicate with multiple SPI devices on the same
 * bus, use additional GPIO pins as software-controlled CS lines.
 *
 * @param spi_address Base address of the SPI controller.
 */
static inline void rvx_spi_deassert_cs(RvxSpi *spi_address)
{
  spi_address->RVX_SPI_CHIP_SELECT = 1;
}

/**
 * @brief Perform a full-duplex SPI transfer.
 *
 * This function transmits `tx_data` to a SPI subordinate device while simultaneously receiving a byte from
 * the subordinate. The subordinate device must be selected prior to calling this function by asserting its CS (Chip
 * Select) line.
 *
 * The return value is undefined if no SPI subordinate is selected.
 *
 * Example usage:
 * ```c
 * // Initialize SPI controller in mode 0 and set speed to 1/12 of the RVX clock frequency.
 * rvx_spi_set_mode(RVX_SPI_ADDRESS, RVX_SPI_MODE_0);
 * rvx_spi_set_divider(RVX_SPI_ADDRESS, 12);
 *
 * // Transmit 0xAB to a subordinate device connected to the CS line controlled
 * // by the SPI controller and receive a byte simultaneously.
 * rvx_spi_assert_cs(RVX_SPI_ADDRESS);
 * uint8_t received_1 = rvx_spi_transfer(RVX_SPI_ADDRESS, 0xAB);
 * rvx_spi_deassert_cs(RVX_SPI_ADDRESS);
 *
 * // Transmit 0xCD to another subordinate device using a GPIO-controlled CS line.
 * rvx_gpio_set_low(RVX_GPIO_ADDRESS, 0); // Assert GPIO-controlled CS for the second device
 * uint8_t received_2 = rvx_spi_transfer(RVX_SPI_ADDRESS, 0xCD);
 * rvx_gpio_set_high(RVX_GPIO_ADDRESS, 0); // Deassert GPIO-controlled CS for the second device
 * ```
 *
 * @param spi_address Base address of the SPI controller.
 * @param tx_data Byte to be transmitted.
 * @return The byte received from the SPI subordinate during the transfer.
 */
static inline uint8_t rvx_spi_transfer(RvxSpi *spi_address, const uint8_t tx_data)
{
  spi_address->RVX_SPI_WRITE = tx_data;
  while (spi_address->RVX_SPI_STATUS & 1)
    ;
  return spi_address->RVX_SPI_READ;
}

#endif // __RVX_SPI_H