// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 RVX Project Contributors

#ifndef __RVX_SPI_H
#define __RVX_SPI_H

#if __riscv_xlen != 32
#error "Unsupported XLEN"
#endif

#include "rvx_macros.h"

/// Base address of the SPI controller registers.
#define RVX_SPI_CONTROLLER_ADDRESS 0x40003000U

/// The SPI mode configuration.
typedef enum RvxSpiMode
{
  RVX_SPI_MODE_0 = 0, ///< SPI Mode 0 (CPOL = 0 / CPHA = 0).
  RVX_SPI_MODE_1 = 1, ///< SPI Mode 1 (CPOL = 0 / CPHA = 1).
  RVX_SPI_MODE_2 = 2, ///< SPI Mode 2 (CPOL = 1 / CPHA = 0).
  RVX_SPI_MODE_3 = 3  ///< SPI Mode 3 (CPOL = 1 / CPHA = 1).
} RvxSpiMode;

/// Provide access to the SPI controller registers.
typedef struct RVX_ALIGNED RvxSpiRegs
{
  volatile uint32_t RVX_SPI_MODE;        ///< RVX SPI Mode Register.
  volatile uint32_t RVX_SPI_CHIP_SELECT; ///< RVX SPI Chip Select register.
  volatile uint32_t RVX_SPI_DIVIDER;     ///< RVX SPI Clock Divider Register.
  volatile uint32_t RVX_SPI_WRITE;       ///< RVX SPI Write Register.
  volatile uint32_t RVX_SPI_READ;        ///< RVX SPI Read Register.
  volatile uint32_t RVX_SPI_STATUS;      ///< RVX SPI Status Register.
} RvxSpiRegs;

/**
 * @brief Set the mode of the SPI controller.
 *
 * The following modes are supported, defined by the `RvxSpiMode` enum:
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
 * // Pointer to the SPI controller registers.
 * RvxSpiRegs *spi_controller = (RvxSpiRegs *)RVX_SPI_CONTROLLER_ADDRESS;
 *
 * // Change the SPI controller mode to 1 (CPOL = 0, CPHA = 1)
 * rvx_spi_set_mode(spi_controller, RVX_SPI_MODE_1);
 * ```
 *
 * @note After reset, the SPI controller is in mode 0 (CPOL = 0, CPHA = 0) by default.
 *
 * @param spi_controller Pointer to the SPI controller registers.
 * @param spi_mode Desired mode as `RvxSpiMode`.
 */
static inline void rvx_spi_set_mode(RvxSpiRegs *spi_controller, RvxSpiMode spi_mode)
{
  spi_controller->RVX_SPI_MODE = spi_mode;
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
 * // Pointer to the SPI controller registers.
 * RvxSpiRegs *spi_controller = (RvxSpiRegs *)RVX_SPI_CONTROLLER_ADDRESS;
 *
 * // Divide RVX clock by 12 for SPI communication.
 * // Example: if RVX clock is 12MHz, the SPI speed will be 1MHz.
 * rvx_spi_set_divider(spi_controller, 12);
 * ```
 *
 * @note After reset, the `clock_divider` is set to 2 by default.
 *
 * @param spi_controller Pointer to the SPI controller registers.
 * @param clock_divider Even integer between 2 and 65534 (inclusive) that determines the SCLK pin frequency.
 */
static inline void rvx_spi_set_divider(RvxSpiRegs *spi_controller, uint16_t clock_divider)
{
  if (clock_divider < 2)
    clock_divider = 2;
  spi_controller->RVX_SPI_DIVIDER = (uint16_t)((clock_divider) >> 1) - 1;
}

/**
 * @brief Assert the chip select (CS) pin controlled by the SPI controller.
 *
 * @note To communicate with multiple SPI peripherals on the same bus, use additional GPIO pins as software-controlled
 * CS lines.
 *
 * @param spi_controller Pointer to the SPI controller registers.
 */
static inline void rvx_spi_assert_cs(RvxSpiRegs *spi_controller)
{
  spi_controller->RVX_SPI_CHIP_SELECT = 0;
}

/**
 * @brief Deassert the chip select (CS) pin controlled by the SPI controller.
 *
 * @note To communicate with multiple SPI peripherals on the same bus, use additional GPIO pins as software-controlled
 * CS lines.
 *
 * @param spi_controller Pointer to the SPI controller registers.
 */
static inline void rvx_spi_deassert_cs(RvxSpiRegs *spi_controller)
{
  spi_controller->RVX_SPI_CHIP_SELECT = 1;
}

/**
 * @brief Perform a full-duplex SPI transfer.
 *
 * This function transmits `tx_data` to a SPI peripheral while simultaneously receiving a byte from
 * the peripheral. The received byte is returned.
 *
 * The peripheral device must be selected prior to calling this function by asserting its chip select
 * (CS) line. The return value is undefined if no SPI peripheral is selected.
 *
 * Example usage:
 * ```c
 * // Pointer to the SPI controller registers.
 * RvxSpiRegs *spi_controller = (RvxSpiRegs *)RVX_SPI_CONTROLLER_ADDRESS;
 *
 * // Initialize SPI controller in mode 0 and set speed to 1/12 of the RVX clock frequency.
 * rvx_spi_set_mode(spi_controller, RVX_SPI_MODE_0);
 * rvx_spi_set_divider(spi_controller, 12);
 *
 * // Transmit 0xAB to a subordinate device connected to the CS line controlled
 * // by the SPI controller and receive a byte simultaneously.
 * rvx_spi_assert_cs(spi_controller);
 * uint8_t received_1 = rvx_spi_transfer(spi_controller, 0xAB);
 * rvx_spi_deassert_cs(spi_controller);
 *
 * // Transmit 0xCD to another subordinate device using a GPIO-controlled CS line.
 * RvxGpioRegs *gpio_controller = (RvxGpioRegs *)RVX_GPIO_CONTROLLER_ADDRESS;
 * rvx_gpio_pin_write(gpio_controller, 0, RVX_GPIO_LOW); // Assert GPIO-controlled CS line for the second device
 * uint8_t received_2 = rvx_spi_transfer(spi_controller, 0xCD);
 * rvx_gpio_pin_write(gpio_controller, 0, RVX_GPIO_HIGH); // Deassert GPIO-controlled CS line for the second device
 * ```
 *
 * @param spi_controller Pointer to the SPI controller registers.
 * @param tx_data Byte to be transmitted.
 * @return The byte received from the SPI peripheral during the transfer.
 */
static inline uint8_t rvx_spi_transfer(RvxSpiRegs *spi_controller, const uint8_t tx_data)
{
  spi_controller->RVX_SPI_WRITE = tx_data;
  while (spi_controller->RVX_SPI_STATUS & 1)
    ;
  return spi_controller->RVX_SPI_READ;
}

#endif // __RVX_SPI_H