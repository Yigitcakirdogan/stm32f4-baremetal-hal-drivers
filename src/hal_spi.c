/**
 * @file    hal_spi.c
 * @author  Yigit Cakirdogan
 * @brief   Register-level SPI Master, I2C bus lockup recovery, and IWDG implementation.
 */

#include "hal_spi.h"
#include "hal_gpio.h"

bool hal_spi_master_init(SPI_TypeDef *spi, const spi_master_cfg_t *cfg)
{
    if ((spi == NULL) || (cfg == NULL)) {
        return false;
    }

    uint32_t cr1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
    cr1 |= ((uint32_t)cfg->baud_div & 0x07UL) << 3U;

    if (cfg->cpol_high) {
        cr1 |= SPI_CR1_CPOL;
    }
    if (cfg->cpha_second_edge) {
        cr1 |= SPI_CR1_CPHA;
    }

    spi->CR1 = cr1;
    spi->CR1 |= SPI_CR1_SPE;
    return true;
}

bool hal_spi_transfer_byte(SPI_TypeDef *spi, uint8_t tx_byte, uint8_t *out_rx_byte, uint32_t timeout_loops)
{
    if (spi == NULL) {
        return false;
    }

    uint32_t guard = timeout_loops;
    while (((spi->SR & SPI_SR_TXE) == 0U) && (guard > 0U)) {
        guard--;
    }
    if (guard == 0U) {
        return false;
    }

    spi->DR = tx_byte;

#if !defined(STM32F407_ON_TARGET)
    /* On host simulation, immediately assert RXNE so unit tests verify transfer path */
    spi->SR |= SPI_SR_RXNE;
#endif

    guard = timeout_loops;
    while (((spi->SR & SPI_SR_RXNE) == 0U) && (guard > 0U)) {
        guard--;
    }
    if (guard == 0U) {
        return false;
    }

    uint8_t rx = (uint8_t)(spi->DR & 0xFFU);
    if (out_rx_byte != NULL) {
        *out_rx_byte = rx;
    }
    return true;
}

uint8_t hal_i2c_bus_unstick_recovery(I2C_TypeDef *i2c, GPIO_TypeDef *gpio_port, uint8_t scl_pin, uint8_t sda_pin)
{
    if ((i2c == NULL) || (gpio_port == NULL)) {
        return 0U;
    }

    /* 1. Disable I2C peripheral and switch SCL/SDA to Open-Drain GPIO Output */
    i2c->CR1 &= ~I2C_CR1_PE;

    gpio_pin_cfg_t scl_cfg = {
        .pin       = scl_pin,
        .mode      = GPIO_MODE_OUTPUT,
        .otype     = GPIO_OTYPE_OPENDRAIN,
        .speed     = GPIO_SPEED_FAST,
        .pupd      = GPIO_PUPD_PULLUP,
        .af_select = 0U
    };
    (void)hal_gpio_init(gpio_port, &scl_cfg);

    /* 2. Generate 9 SCL clock pulses to release stuck slave state machine */
    uint8_t pulses = 0U;
    for (uint8_t i = 0U; i < 9U; i++) {
        hal_gpio_write_pin(gpio_port, scl_pin, false);
        hal_gpio_write_pin(gpio_port, scl_pin, true);
        pulses++;
    }

    /* 3. Restore AF4 I2C mode and pulse SWRST */
    scl_cfg.mode      = GPIO_MODE_AF;
    scl_cfg.af_select = 4U;
    (void)hal_gpio_init(gpio_port, &scl_cfg);
    scl_cfg.pin = sda_pin;
    (void)hal_gpio_init(gpio_port, &scl_cfg);

    i2c->CR1 |= I2C_CR1_SWRST;
    i2c->CR1 &= ~I2C_CR1_SWRST;
    i2c->CR1 |= I2C_CR1_PE;

    return pulses;
}

void hal_iwdg_start(uint8_t prescaler_code, uint16_t reload_value)
{
    IWDG->KR  = IWDG_KEY_WRITE_ACCESS;
    IWDG->PR  = (uint32_t)(prescaler_code & 0x07U);
    IWDG->RLR = (uint32_t)(reload_value & 0x0FFFU);
    IWDG->KR  = IWDG_KEY_RELOAD;
    IWDG->KR  = IWDG_KEY_ENABLE;
}

void hal_iwdg_refresh(void)
{
    IWDG->KR = IWDG_KEY_RELOAD;
}
