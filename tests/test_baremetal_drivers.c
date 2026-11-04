/**
 * @file    test_baremetal_drivers.c
 * @author  Yigit Cakirdogan
 * @brief   Unit tests verifying register-level bitmasks for GPIO (MODER/AFR/BSRR),
 *          SPI Master CR1 configuration, I2C 9-clock bus recovery, and IWDG sequence.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f407_regs.h"
#include "hal_gpio.h"
#include "hal_spi.h"

#define TEST_ASSERT(cond)                                                          \
    do {                                                                           \
        if (!(cond)) {                                                             \
            fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            exit(EXIT_FAILURE);                                                    \
        }                                                                          \
    } while (0)

static void test_gpio_register_bitmasks(void)
{
    memset(&sim_gpioa, 0, sizeof(sim_gpioa));

    gpio_pin_cfg_t cfg = {
        .pin       = 5U,
        .mode      = GPIO_MODE_AF,
        .otype     = GPIO_OTYPE_PUSHPULL,
        .speed     = GPIO_SPEED_HIGH,
        .pupd      = GPIO_PUPD_NONE,
        .af_select = 5U
    };
    TEST_ASSERT(hal_gpio_init(GPIOA, &cfg));
    TEST_ASSERT(((GPIOA->MODER >> 10U) & 0x03UL) == 0x02UL);
    TEST_ASSERT(((GPIOA->OSPEEDR >> 10U) & 0x03UL) == 0x03UL);
    TEST_ASSERT(((GPIOA->AFR[0] >> 20U) & 0x0FUL) == 5UL);

    hal_gpio_write_pin(GPIOA, 5U, true);
    TEST_ASSERT(GPIOA->BSRR == (1UL << 5U));
    TEST_ASSERT((GPIOA->ODR & (1UL << 5U)) != 0U);

    hal_gpio_write_pin(GPIOA, 5U, false);
    TEST_ASSERT(GPIOA->BSRR == (1UL << (5U + 16U)));
    TEST_ASSERT((GPIOA->ODR & (1UL << 5U)) == 0U);
    printf("[PASS] test_gpio_register_bitmasks\n");
}

static void test_spi_master_and_transfer(void)
{
    memset(&sim_spi1, 0, sizeof(sim_spi1));
    sim_spi1.SR = SPI_SR_TXE;

    spi_master_cfg_t cfg = {
        .baud_div         = SPI_BAUD_DIV16,
        .cpol_high        = true,
        .cpha_second_edge = true
    };
    TEST_ASSERT(hal_spi_master_init(SPI1, &cfg));
    TEST_ASSERT((SPI1->CR1 & SPI_CR1_SPE) != 0U);
    TEST_ASSERT((SPI1->CR1 & SPI_CR1_MSTR) != 0U);
    TEST_ASSERT((SPI1->CR1 & SPI_CR1_CPOL) != 0U);
    TEST_ASSERT((SPI1->CR1 & SPI_CR1_CPHA) != 0U);
    TEST_ASSERT(((SPI1->CR1 >> 3U) & 0x07UL) == SPI_BAUD_DIV16);

    uint8_t rx = 0U;
    TEST_ASSERT(hal_spi_transfer_byte(SPI1, 0xA5U, &rx, 1000U));
    TEST_ASSERT(rx == 0xA5U);
    printf("[PASS] test_spi_master_and_transfer\n");
}

static void test_i2c_bus_unstick_and_iwdg(void)
{
    memset(&sim_gpiob, 0, sizeof(sim_gpiob));
    memset(&sim_i2c1, 0, sizeof(sim_i2c1));
    memset(&sim_iwdg, 0, sizeof(sim_iwdg));

    uint8_t pulses = hal_i2c_bus_unstick_recovery(I2C1, GPIOB, 6U, 7U);
    TEST_ASSERT(pulses == 9U);
    TEST_ASSERT((I2C1->CR1 & I2C_CR1_PE) != 0U);
    TEST_ASSERT(((GPIOB->AFR[0] >> 24U) & 0x0FUL) == 4UL);
    TEST_ASSERT(((GPIOB->AFR[0] >> 28U) & 0x0FUL) == 4UL);

    hal_iwdg_start(0x04U, 0x0FFFU);
    TEST_ASSERT(IWDG->PR == 0x04UL);
    TEST_ASSERT(IWDG->RLR == 0x0FFFUL);
    TEST_ASSERT(IWDG->KR == IWDG_KEY_ENABLE);

    hal_iwdg_refresh();
    TEST_ASSERT(IWDG->KR == IWDG_KEY_RELOAD);
    printf("[PASS] test_i2c_bus_unstick_and_iwdg\n");
}

int main(void)
{
    printf("=== Running STM32F4 Bare-Metal Register Driver Test Suite ===\n");
    test_gpio_register_bitmasks();
    test_spi_master_and_transfer();
    test_i2c_bus_unstick_and_iwdg();
    printf("=== All bare-metal driver tests passed! ===\n");
    return 0;
}
