cmake_minimum_required(VERSION 3.16)
project(stm32f4_baremetal_hal_drivers VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wshadow -ffunction-sections -fdata-sections)
endif()

add_library(stm32f4_baremetal STATIC
    src/hal_gpio.c
    src/hal_spi.c
)

target_include_directories(stm32f4_baremetal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

add_executable(test_baremetal_drivers tests/test_baremetal_drivers.c)
target_link_libraries(test_baremetal_drivers PRIVATE stm32f4_baremetal)
add_test(NAME BareMetalRegisterTestSuite COMMAND test_baremetal_drivers)
