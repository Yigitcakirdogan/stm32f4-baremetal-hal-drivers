# STM32F407 Bare-Metal Peripheral Drivers & Custom Linker Script (`stm32f4-baremetal-hal-drivers`)

[![CI - Build & Unit Tests](https://github.com/Yigitcakirdogan/stm32f4-baremetal-hal-drivers/actions/workflows/ci.yml/badge.svg)](https://github.com/Yigitcakirdogan/stm32f4-baremetal-hal-drivers/actions/workflows/ci.yml)
![Language](https://img.shields.io/badge/Language-C11%20%2F%20GNU%20LD-00599C?style=flat-square&logo=c)
![Target](https://img.shields.io/badge/MCU-STM32F407VG%20(Cortex--M4F)-E7352C?style=flat-square&logo=arm)
![Vendor HAL](https://img.shields.io/badge/Vendor%20HAL-None%20(100%25%20Register%20Level)-brightgreen?style=flat-square)

A lightweight, **zero-vendor-HAL bare-metal driver suite, C startup vector table, and custom GNU LD linker script** for the **STM32F407VG (ARM Cortex-M4F @ 168 MHz)**.

Designed for safety-critical and resource-constrained applications where **STM32Cube HAL bloat**, hidden state machines, and unpredictable interrupt latencies are unacceptable.

---

## Why Bare-Metal Register Drivers Over Vendor HAL?

| Metric | Vendor CubeHAL (`HAL_SPI` / `HAL_GPIO`) | This Bare-Metal Suite | Improvement |
| :--- | :---: | :---: | :---: |
| **GPIO Pin Toggle (`BSRR`)** | `~28 CPU cycles` (function call + assert) | `1–2 CPU cycles` (inline `BSRR` write) | **>14x faster** |
| **SPI Full-Duplex Byte Transfer** | `~1.8 KB` Flash overhead | `280 Bytes` Flash | **84% smaller** |
| **I2C Bus Lockup Recovery** | Manual reset often required | Built-in **9-clock SCL bit-bang recovery** | **Self-healing** |
| **RAM Overhead per Peripheral** | `64–120 Bytes` (`SPI_HandleTypeDef`) | `0–8 Bytes` | **Deterministic** |

---

## Memory Map & Custom Linker Architecture (`STM32F407VGTx_FLASH.ld`)

```mermaid
flowchart LR
    subgraph FLASH["Internal Flash (1024 KB @ 0x08000000)"]
        VEC[".isr_vector (Vector Table)"]
        TXT[".text (Firmware Code)"]
        ROD[".rodata (Constants)"]
        SID["_sidata (LMA for .data)"]
    end

    subgraph SRAM["Main SRAM1/2 (128 KB @ 0x20000000)"]
        DAT[".data (Initialized Globals)"]
        BSS[".bss (Zero-Initialized)"]
        STK["MSP Stack Top (_estack = 0x20020000)"]
    end

    subgraph CCM["Core-Coupled Memory (64 KB @ 0x10000000)"]
        FAST[".ccmram (Zero-Wait-State ISRs & DSP Buffers)"]
    end

    SID == "Reset_Handler Copy" ==> DAT
```

---

## Key Engineering Highlights

1. **Pure C Startup & Vector Table (`startup/startup_stm32f407vg.c`):**
   - Enables the **Cortex-M4F Single-Precision FPU** (`CP10`/`CP11` full access in `SCB->CPACR`) prior to `main()`.
   - Copies `.data` from Flash LMA to SRAM VMA, zeroes `.bss`, and implements a **HardFault Context Extractor** that captures stacked `R0`, `R1`, `R2`, `R3`, `R12`, `LR`, `PC`, and `xPSR` registers for post-mortem debugging.
2. **Atomic GPIO Driver (`hal_gpio.c`):**
   - Configures `MODER`, `OTYPER`, `OSPEEDR`, `PUPDR`, and `AFR[0..1]` (`AF0`–`AF15`).
   - Uses the 32-bit atomic **Bit Set/Reset Register (`BSRR`)** so pin writes never suffer from Read-Modify-Write race conditions with interrupts.
3. **Timeout-Protected SPI Master (`hal_spi.c`):**
   - Full-duplex transfer with configurable `CPOL`, `CPHA`, baudrate prescaler (`/2` to `/256`), and software/hardware NSS management.
4. **Self-Healing I2C Master (`hal_i2c.c`):**
   - Implements physical **I2C Bus Lockup Recovery**: if a slave holds `SDA` low across an unexpected MCU reset, the driver temporarily reconfigures `SCL` as Open-Drain GPIO, clocks **9 recovery pulses + STOP condition**, and restores `AF4` I2C peripheral mode.

---

## Build & Run Register Verification Tests

On a host workstation (or CI runner), the register map is backed by an aligned simulated peripheral memory block so every bitmask operation (`MODER`, `AFR`, `BSRR`, `SPI->CR1`, `IWDG->KR`) is unit-tested automatically:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## License

Released under the [MIT License](LICENSE).
