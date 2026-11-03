/**
 * @file    hal_gpio.h
 * @author  Yigit Cakirdogan
 * @brief   Register-level STM32F4 GPIO driver using atomic BSRR operations.
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "stm32f407_regs.h"

typedef enum {
    GPIO_MODE_INPUT     = 0x00U,
    GPIO_MODE_OUTPUT    = 0x01U,
    GPIO_MODE_AF        = 0x02U,
    GPIO_MODE_ANALOG    = 0x03U
} gpio_mode_t;

typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x00U,
    GPIO_OTYPE_OPENDRAIN = 0x01U
} gpio_otype_t;

typedef enum {
    GPIO_SPEED_LOW       = 0x00U,
    GPIO_SPEED_MEDIUM    = 0x01U,
    GPIO_SPEED_FAST      = 0x02U,
    GPIO_SPEED_HIGH      = 0x03U
} gpio_speed_t;

typedef enum {
    GPIO_PUPD_NONE       = 0x00U,
    GPIO_PUPD_PULLUP     = 0x01U,
    GPIO_PUPD_PULLDOWN   = 0x02U
} gpio_pupd_t;

typedef struct {
    uint8_t      pin;        /**< Pin number 0..15               */
    gpio_mode_t  mode;       /**< Input, Output, AF, or Analog   */
    gpio_otype_t otype;      /**< Push-Pull or Open-Drain        */
    gpio_speed_t speed;      /**< Slew rate speed                */
    gpio_pupd_t  pupd;       /**< Pull-up / Pull-down            */
    uint8_t      af_select;  /**< Alternate function 0..15       */
} gpio_pin_cfg_t;

bool hal_gpio_init(GPIO_TypeDef *port, const gpio_pin_cfg_t *cfg);
void hal_gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, bool state);
void hal_gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin);
bool hal_gpio_read_pin(const GPIO_TypeDef *port, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H */
