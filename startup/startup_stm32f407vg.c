/**
 * @file    startup_stm32f407vg.c
 * @author  Yigit Cakirdogan
 * @brief   Cortex-M4F Vector Table, Reset_Handler (.data/.bss init + FPU enable),
 *          and HardFault diagnostic context extractor written in pure C.
 */

#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern int main(void);

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} cortex_m4_stack_frame_t;

volatile cortex_m4_stack_frame_t g_last_hardfault_frame;

void Reset_Handler(void)
{
    /* 1. Copy .data section from Flash LMA (_sidata) to SRAM VMA (_sdata.._edata) */
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    /* 2. Zero-initialize .bss section */
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0UL;
    }

    /* 3. Enable Cortex-M4F Hardware FPU (CP10 and CP11 Full Access in SCB->CPACR) */
    volatile uint32_t *scb_cpacr = (volatile uint32_t *)0xE000ED88UL;
    *scb_cpacr |= (0xFUL << 20U);

    /* 4. Branch to application entry point */
    (void)main();

    while (1) {
        /* Should never reach here */
    }
}

void HardFault_Handler_C(const cortex_m4_stack_frame_t *frame)
{
    if (frame != 0) {
        g_last_hardfault_frame = *frame;
    }
    while (1) {
        /* Halt for SWD debugger inspection */
    }
}
