/**
 * @file    hal_spi.h
 * @author  Yigit Cakirdogan
 * @brief   Register-level STM32F4 SPI Master driver and I2C Bus Recovery routine.
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "stm32f407_regs.h"

typedef enum {
    SPI_BAUD_DIV2   = 0x00U,
    SPI_BAUD_DIV4   = 0x01U,
    SPI_BAUD_DIV8   = 0x02U,
    SPI_BAUD_DIV16  = 0x03U,
    SPI_BAUD_DIV32  = 0x04U,
    SPI_BAUD_DIV64  = 0x05U,
    SPI_BAUD_DIV128 = 0x06U,
    SPI_BAUD_DIV256 = 0x07U
} spi_baud_div_t;

typedef struct {
    spi_baud_div_t baud_div;
    bool           cpol_high;
    bool           cpha_second_edge;
} spi_master_cfg_t;

/**
 * @brief Initialize SPI peripheral in Master mode with software NSS management.
 */
bool hal_spi_master_init(SPI_TypeDef *spi, const spi_master_cfg_t *cfg);

/**
 * @brief Perform a timeout-protected full-duplex SPI byte transfer.
 */
bool hal_spi_transfer_byte(SPI_TypeDef *spi, uint8_t tx_byte, uint8_t *out_rx_byte, uint32_t timeout_loops);

/**
 * @brief Recover a hung I2C bus by toggling SCL 9 times in Open-Drain mode and issuing SWRST.
 */
uint8_t hal_i2c_bus_unstick_recovery(I2C_TypeDef *i2c, GPIO_TypeDef *gpio_port, uint8_t scl_pin, uint8_t sda_pin);

/**
 * @brief Configure and start the Independent Hardware Watchdog (IWDG).
 */
void hal_iwdg_start(uint8_t prescaler_code, uint16_t reload_value);

/**
 * @brief Reload (kick) the Independent Hardware Watchdog.
 */
void hal_iwdg_refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SPI_H */
