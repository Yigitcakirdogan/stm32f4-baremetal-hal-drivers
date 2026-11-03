/**
 * @file    stm32f407_regs.h
 * @author  Yigit Cakirdogan
 * @brief   CMSIS-style memory-mapped register structures and bit definitions
 *          for STM32F407VG (supports both target silicon and host unit testing).
 */

#ifndef STM32F407_REGS_H
#define STM32F407_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define __IO volatile

/**
 * @brief General Purpose I/O (GPIO) Register Map
 */
typedef struct {
    __IO uint32_t MODER;    /**< 0x00: Mode register                    */
    __IO uint32_t OTYPER;   /**< 0x04: Output type register             */
    __IO uint32_t OSPEEDR;  /**< 0x08: Output speed register            */
    __IO uint32_t PUPDR;    /**< 0x0C: Pull-up/pull-down register       */
    __IO uint32_t IDR;      /**< 0x10: Input data register              */
    __IO uint32_t ODR;      /**< 0x14: Output data register             */
    __IO uint32_t BSRR;     /**< 0x18: Atomic bit set/reset register    */
    __IO uint32_t LCKR;     /**< 0x1C: Configuration lock register      */
    __IO uint32_t AFR[2];   /**< 0x20-0x24: Alternate function low/high */
} GPIO_TypeDef;

/**
 * @brief Reset and Clock Control (RCC) Register Map
 */
typedef struct {
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    __IO uint32_t AHB3RSTR;
    uint32_t      RESERVED0;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t      RESERVED1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    __IO uint32_t AHB3ENR;
    uint32_t      RESERVED2;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
} RCC_TypeDef;

/**
 * @brief Serial Peripheral Interface (SPI) Register Map
 */
typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t CRCPR;
    __IO uint32_t RXCRCR;
    __IO uint32_t TXCRCR;
} SPI_TypeDef;

/**
 * @brief Inter-Integrated Circuit (I2C) Register Map
 */
typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t OAR1;
    __IO uint32_t OAR2;
    __IO uint32_t DR;
    __IO uint32_t SR1;
    __IO uint32_t SR2;
    __IO uint32_t CCR;
    __IO uint32_t TRISE;
} I2C_TypeDef;

/**
 * @brief Independent Watchdog (IWDG) Register Map
 */
typedef struct {
    __IO uint32_t KR;
    __IO uint32_t PR;
    __IO uint32_t RLR;
    __IO uint32_t SR;
} IWDG_TypeDef;

/* SPI Bit Definitions */
#define SPI_CR1_CPHA        (1UL << 0)
#define SPI_CR1_CPOL        (1UL << 1)
#define SPI_CR1_MSTR        (1UL << 2)
#define SPI_CR1_SPE         (1UL << 6)
#define SPI_CR1_SSI         (1UL << 8)
#define SPI_CR1_SSM         (1UL << 9)
#define SPI_SR_RXNE         (1UL << 0)
#define SPI_SR_TXE          (1UL << 1)
#define SPI_SR_BSY          (1UL << 7)

/* I2C Bit Definitions */
#define I2C_CR1_PE          (1UL << 0)
#define I2C_CR1_START       (1UL << 8)
#define I2C_CR1_STOP        (1UL << 9)
#define I2C_CR1_ACK         (1UL << 10)
#define I2C_CR1_SWRST       (1UL << 15)
#define I2C_SR2_BUSY        (1UL << 1)

/* IWDG Key Values */
#define IWDG_KEY_RELOAD     0xAAAAUL
#define IWDG_KEY_ENABLE     0xCCCCUL
#define IWDG_KEY_WRITE_ACCESS 0x5555UL

#if defined(STM32F407_ON_TARGET)
    #define GPIOA   ((GPIO_TypeDef *)0x40020000UL)
    #define GPIOB   ((GPIO_TypeDef *)0x40020400UL)
    #define GPIOC   ((GPIO_TypeDef *)0x40020800UL)
    #define RCC     ((RCC_TypeDef  *)0x40023800UL)
    #define SPI1    ((SPI_TypeDef  *)0x40013000UL)
    #define I2C1    ((I2C_TypeDef  *)0x40005400UL)
    #define IWDG    ((IWDG_TypeDef *)0x40003000UL)
#else
    extern GPIO_TypeDef sim_gpioa;
    extern GPIO_TypeDef sim_gpiob;
    extern RCC_TypeDef  sim_rcc;
    extern SPI_TypeDef  sim_spi1;
    extern I2C_TypeDef  sim_i2c1;
    extern IWDG_TypeDef sim_iwdg;

    #define GPIOA   (&sim_gpioa)
    #define GPIOB   (&sim_gpiob)
    #define RCC     (&sim_rcc)
    #define SPI1    (&sim_spi1)
    #define I2C1    (&sim_i2c1)
    #define IWDG    (&sim_iwdg)
#endif

#ifdef __cplusplus
}
#endif

#endif /* STM32F407_REGS_H */
