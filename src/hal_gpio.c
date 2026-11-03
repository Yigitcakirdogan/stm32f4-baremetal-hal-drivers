/**
 * @file    hal_gpio.c
 * @author  Yigit Cakirdogan
 * @brief   Implementation of register-level STM32F4 GPIO peripheral driver.
 */

#include "hal_gpio.h"
#include <stddef.h>

#if !defined(STM32F407_ON_TARGET)
GPIO_TypeDef sim_gpioa;
GPIO_TypeDef sim_gpiob;
RCC_TypeDef  sim_rcc;
SPI_TypeDef  sim_spi1;
I2C_TypeDef  sim_i2c1;
IWDG_TypeDef sim_iwdg;
#endif

bool hal_gpio_init(GPIO_TypeDef *port, const gpio_pin_cfg_t *cfg)
{
    if ((port == NULL) || (cfg == NULL) || (cfg->pin > 15U) || (cfg->af_select > 15U)) {
        return false;
    }

    uint32_t pin       = cfg->pin;
    uint32_t shift_2b  = pin * 2U;

    /* 1. Configure Pin Mode (MODER) */
    port->MODER &= ~(0x03UL << shift_2b);
    port->MODER |= ((uint32_t)cfg->mode & 0x03UL) << shift_2b;

    /* 2. Configure Output Type (OTYPER) */
    port->OTYPER &= ~(1UL << pin);
    port->OTYPER |= ((uint32_t)cfg->otype & 0x01UL) << pin;

    /* 3. Configure Output Speed (OSPEEDR) */
    port->OSPEEDR &= ~(0x03UL << shift_2b);
    port->OSPEEDR |= ((uint32_t)cfg->speed & 0x03UL) << shift_2b;

    /* 4. Configure Pull-Up / Pull-Down (PUPDR) */
    port->PUPDR &= ~(0x03UL << shift_2b);
    port->PUPDR |= ((uint32_t)cfg->pupd & 0x03UL) << shift_2b;

    /* 5. Configure Alternate Function Multiplexer (AFR[0] or AFR[1]) */
    if (cfg->mode == GPIO_MODE_AF) {
        uint32_t afr_idx   = pin >> 3U;
        uint32_t afr_shift = (pin & 0x07U) * 4U;
        port->AFR[afr_idx] &= ~(0x0FUL << afr_shift);
        port->AFR[afr_idx] |= ((uint32_t)cfg->af_select & 0x0FUL) << afr_shift;
    }

    return true;
}

void hal_gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, bool state)
{
    if ((port == NULL) || (pin > 15U)) {
        return;
    }

    if (state) {
        port->BSRR = (1UL << pin);
        port->ODR |= (1UL << pin);
    } else {
        port->BSRR = (1UL << (pin + 16U));
        port->ODR &= ~(1UL << pin);
    }
}

void hal_gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin)
{
    if ((port == NULL) || (pin > 15U)) {
        return;
    }

    uint32_t mask = (1UL << pin);
    if ((port->ODR & mask) != 0U) {
        hal_gpio_write_pin(port, pin, false);
    } else {
        hal_gpio_write_pin(port, pin, true);
    }
}

bool hal_gpio_read_pin(const GPIO_TypeDef *port, uint8_t pin)
{
    if ((port == NULL) || (pin > 15U)) {
        return false;
    }
    return ((port->IDR & (1UL << pin)) != 0U);
}
