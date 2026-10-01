// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_SPI_H
#define __RVX_SPI_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

#define RVX_SPI_STATUS_BUSY_MASK 0x1U ///< SPI transfer-in-progress status bit.

/// The SPI clock polarity and phase configuration.
typedef enum RvxSpiMode
{
  RVX_SPI_MODE_0 = 0, ///< SPI Mode 0 (CPOL = 0 / CPHA = 0).
  RVX_SPI_MODE_1 = 1, ///< SPI Mode 1 (CPOL = 0 / CPHA = 1).
  RVX_SPI_MODE_2 = 2, ///< SPI Mode 2 (CPOL = 1 / CPHA = 0).
  RVX_SPI_MODE_3 = 3  ///< SPI Mode 3 (CPOL = 1 / CPHA = 1).
} RvxSpiMode;

/**
 * @brief Structure representing the SPI controller registers.
 *
 * The fields of this structure are laid out in the same order as the hardware registers,
 * allowing direct access to them through a pointer.
 *
 * For example:
 *
 * ```c
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * uint32_t spi_status = RVX_SPI0->RVX_SPI_STATUS_REG; // Read the status register of SPI0.
 * ```
 */
typedef struct RVX_ALIGNED RvxSpi
{
  volatile uint32_t RVX_SPI_MODE_REG;        ///< RVX SPI Mode Register.
  volatile uint32_t RVX_SPI_CHIP_SELECT_REG; ///< RVX SPI Chip Select Register.
  volatile uint32_t RVX_SPI_DIVIDER_REG;     ///< RVX SPI Clock Divider Register.
  volatile uint32_t RVX_SPI_WRITE_REG;       ///< RVX SPI Write Register.
  volatile uint32_t RVX_SPI_READ_REG;        ///< RVX SPI Read Register.
  volatile uint32_t RVX_SPI_STATUS_REG;      ///< RVX SPI Status Register.
} RvxSpi;

/**
 * @brief Set the mode for the SPI controller.
 *
 * The following modes are supported, defined by the `RvxSpiMode` enum:
 * - `RVX_SPI_MODE_0` (CPOL = 0, CPHA = 0)
 * - `RVX_SPI_MODE_1` (CPOL = 0, CPHA = 1)
 * - `RVX_SPI_MODE_2` (CPOL = 1, CPHA = 0)
 * - `RVX_SPI_MODE_3` (CPOL = 1, CPHA = 1)
 *
 * Example usage:
 * ```c
 * // Set SPI0 to mode 1 (CPOL = 0, CPHA = 1).
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * rvx_spi_set_mode(RVX_SPI0, RVX_SPI_MODE_1);
 * ```
 *
 * @note After reset, the SPI controller is in mode 0 (CPOL = 0, CPHA = 0) by default.
 *
 * @param spi Pointer to the base address of the SPI registers.
 * @param spi_mode Desired mode as `RvxSpiMode`.
 */
static inline void rvx_spi_set_mode(RvxSpi *spi, RvxSpiMode spi_mode)
{
  spi->RVX_SPI_MODE_REG = spi_mode;
}

/**
 * @brief Set the clock divider for the SPI controller.
 *
 * The clock divider determines the frequency of SPI communication according to the formula `sclk_freq =
 * rvx_clock_freq / clock_divider`, where `sclk_freq` is the resulting SPI clock frequency, `rvx_clock_freq` is the
 * frequency of the clock driving RVX, and `clock_divider` is the even integer value passed to this function.
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
 * // Divide RVX clock by 12 for SPI communication.
 * // Example: if RVX clock is 12MHz, the SPI speed will be 1MHz.
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * rvx_spi_set_divider(RVX_SPI0, 12);
 * ```
 *
 * @note After reset, the `clock_divider` is set to 2 by default.
 *
 * @param spi Pointer to the base address of the SPI registers.
 * @param clock_divider Even integer between 2 and 65534 (inclusive) that determines the SCLK pin frequency.
 */
static inline void rvx_spi_set_divider(RvxSpi *spi, uint16_t clock_divider)
{
  if (clock_divider < 2)
    clock_divider = 2;
  spi->RVX_SPI_DIVIDER_REG = (uint16_t)((clock_divider) >> 1) - 1;
}

/**
 * @brief Return `true` if the SPI controller is ready for a new transfer, or `false` otherwise.
 *
 * Example usage:
 *
 * ```c
 * // Wait until SPI0 is ready for a new transfer, then send a byte.
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * while (!rvx_spi_ready(RVX_SPI0));
 * rvx_spi_write(RVX_SPI0, 0x55);
 * ```
 *
 * @param spi Pointer to the base address of the SPI registers.
 * @return `true` if the SPI controller is ready for a new transfer, `false` otherwise.
 */
static inline bool rvx_spi_ready(RvxSpi *spi)
{
  return (spi->RVX_SPI_STATUS_REG & RVX_SPI_STATUS_BUSY_MASK) == 0U;
}

/**
 * @brief Assert the chip select (CS) pin controlled by the SPI controller.
 *
 * @note To communicate with multiple SPI peripherals on the same bus, use additional GPIO pins as software-controlled
 * CS lines.
 *
 * @param spi Pointer to the base address of the SPI registers.
 */
static inline void rvx_spi_assert_cs(RvxSpi *spi)
{
  spi->RVX_SPI_CHIP_SELECT_REG = 0;
}

/**
 * @brief Deassert the chip select (CS) pin controlled by the SPI controller.
 *
 * @note To communicate with multiple SPI peripherals on the same bus, use additional GPIO pins as software-controlled
 * CS lines.
 *
 * @param spi Pointer to the base address of the SPI registers.
 */
static inline void rvx_spi_deassert_cs(RvxSpi *spi)
{
  spi->RVX_SPI_CHIP_SELECT_REG = 1;
}

/**
 * @brief Read the byte received during the most recently completed SPI transfer.
 *
 * To check that a transfer has completed before calling this function, use `rvx_spi_ready()`.
 *
 * If this function is called before any data has been received, `0x00` is returned.
 *
 * This function is non-blocking and the read is non-destructive (does not remove the byte from the buffer).
 *
 * Example usage:
 *
 * ```c
 * // Wait for SPI0 to finish a transfer, then read the received byte.
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * while (!rvx_spi_ready(RVX_SPI0));
 * uint8_t rx_data = rvx_spi_read(RVX_SPI0);
 * ```
 *
 * @param spi Pointer to the base address of the SPI registers.
 * @return The byte received during the most recently completed SPI transfer.
 */
static inline uint8_t rvx_spi_read(RvxSpi *spi)
{
  return spi->RVX_SPI_READ_REG;
}

/**
 * @brief Transmit a byte over SPI.
 *
 * To check if the SPI controller is ready before calling this function, use `rvx_spi_ready()`.
 *
 * This function is non-blocking.
 *
 * If this function is called before the SPI controller is ready to transmit, the byte is discarded.
 *
 * An SPI peripheral device must be selected before calling this function by asserting its chip select (CS) line.
 *
 * Example usage:
 *
 * ```c
 * // Wait for SPI0 to be ready before transmitting, then send the byte.
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * while (!rvx_spi_ready(RVX_SPI0));
 * rvx_spi_write(RVX_SPI0, 0x9F);
 * ```
 *
 * @param spi Pointer to the base address of the SPI registers.
 * @param tx_data Byte to transmit.
 */
static inline void rvx_spi_write(RvxSpi *spi, const uint8_t tx_data)
{
  spi->RVX_SPI_WRITE_REG = tx_data;
}

/**
 * @brief Perform a full-duplex SPI transfer.
 *
 * This function waits for any ongoing transfer to finish, transmits `tx_data` to an SPI peripheral while
 * simultaneously receiving a byte, and waits for the new transfer to complete before returning the received byte.
 *
 * An SPI peripheral device must be selected prior to calling this function by asserting its chip select
 * (CS) line. The return value is undefined if no SPI peripheral is selected.
 *
 * Example usage:
 * ```c
 * // Initialize SPI controller in mode 0 and set speed to 1/12 of the RVX clock frequency.
 * // Macro RVX_SPI0 expands to the base address of SPI0: ((RvxSpi *)0x40003000U).
 * rvx_spi_set_mode(RVX_SPI0, RVX_SPI_MODE_0);
 * rvx_spi_set_divider(RVX_SPI0, 12);
 *
 * // Transmit 0xAB to a subordinate device connected to the CS line controlled
 * // by the SPI controller and receive a byte simultaneously.
 * rvx_spi_assert_cs(RVX_SPI0);
 * uint8_t received_1 = rvx_spi_transfer(RVX_SPI0, 0xAB);
 * rvx_spi_deassert_cs(RVX_SPI0);
 *
 * // Transmit 0xCD to another subordinate device using a GPIO-controlled CS line.
 * rvx_gpio_pin_write(RVX_GPIO0, 0, RVX_GPIO_LOW); // Assert GPIO-controlled CS line for the second device.
 * uint8_t received_2 = rvx_spi_transfer(RVX_SPI0, 0xCD);
 * rvx_gpio_pin_write(RVX_GPIO0, 0, RVX_GPIO_HIGH); // Deassert GPIO-controlled CS line for the second device.
 * ```
 *
 * @param spi Pointer to the base address of the SPI registers.
 * @param tx_data Byte to be transmitted.
 * @return The byte received from the SPI peripheral during the transfer.
 */
static inline uint8_t rvx_spi_transfer(RvxSpi *spi, const uint8_t tx_data)
{
  while (!rvx_spi_ready(spi))
    ;
  rvx_spi_write(spi, tx_data);
  while (!rvx_spi_ready(spi))
    ;
  return rvx_spi_read(spi);
}

#endif // __RVX_SPI_H