# Blake Freer
# October 27, 2023

cmake_minimum_required(VERSION 3.12)

# gcc-arm
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_FLAGS_INIT
    "-fdata-sections -ffunction-sections --specs=nano.specs -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS_INIT
    "${CMAKE_C_FLAGS_INIT} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}objdump)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set project name
if ("${PROJECT_NAME}" STREQUAL "")
	set(PROJECT_NAME TMS)
endif()
project(${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use Debug mode by default
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Set Microcontroller information
set(MCAL_BOARDNAME stm32f767)
set(MCU_FAMILY STM32F7xx)
set(MCU_MODEL STM32F767xx)

set(MCAL_CORE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/mcal/${MCAL_BOARDNAME}/Core)

set(CPU_PARAMETERS
	-mcpu=cortex-m7
	-mthumb
	-mfpu=fpv5-d16
	-mfloat-abi=hard)

set(STARTUP_SCRIPT ${MCAL_CORE}/startup_stm32f767xx.s)
set(MCU_LINKER_SCRIPT ${MCAL_CORE}/STM32F767ZITx_FLASH.ld)

set(EXECUTABLE ${CMAKE_PROJECT_NAME})

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_EXTENSIONS ON)

# Core MCAL Source Code
set(MCAL_CORE_INCLUDE_DIRECTORIES ${MCAL_CORE}/Inc)
set(INFRA_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/infra)

# Driver source code
set(DRIVER_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Inc
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Inc/Legacy
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/CMSIS/Device/ST/STM32F7xx/Include
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/CMSIS/Include
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/mcal
)

set(DRIVER_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
)

# file(GLOB_RECURSE DRIVER_SOURCES CONFIGURE_DEPENDS
# 	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/*.c
# 	${CMAKE_CURRENT_SOURCE_DIR}/drivers/${MCAL_BOARDNAME}/STM32F7xx_HAL_Driver/Src/*.cc
# )

# Core source code
set(CORE_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/TMS/mcl/stm32f767/Core/Inc
)
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/TMS/mcal/stm32f767/Core/Src/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/TMS/mcal/stm32f767/Core/Src/*.cc
)

set(MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/TMS/main.cc)

add_executable(
	${EXECUTABLE}
	${DRIVER_SOURCES}
	${CORE_SOURCES}
	${STARTUP_SCRIPT}
	${MAIN_SOURCE}
)

target_compile_definitions(${EXECUTABLE} PRIVATE
	${MCU_MODEL}
	USE_HAL_DRIVER
	STM32F767xx
)

# Include directories to executable
target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
	${DRIVER_INCLUDE_DIRECTORIES}
)
target_include_directories(${EXECUTABLE} PRIVATE
	${CORE_INCLUDE_DIRECTORIES}
)

target_include_directories(${EXECUTABLE} PRIVATE
	${MCAL_CORE_INCLUDE_DIRECTORIES}
)

target_include_directories(${EXECUTABLE} PRIVATE
	${INFRA_INCLUDE_DIRECTORIES}
)

target_compile_options(${EXECUTABLE} PRIVATE
	${CPU_PARAMETERS}
	-Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wdouble-promotion
    -Wformat=2 -Wformat-truncation
    -Wundef
    -fno-common
    -Wno-unused-parameter
	-Wno-missing-field-initializers
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wconversion
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>
)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    $<$<VERSION_GREATER:$<C_COMPILER_VERSION>,10.3.1>:-Wl,--no-warn-rwx-segments>
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -Wl,--end-group
    -Wl,--print-memory-usage
)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin)

option(DUMP_ASM "Create full assembly of final executable" OFF)
if (${DUMP_ASM})
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -D -C $<TARGET_FILE:${EXECUTABLE}> > ${EXECUTABLE}.s)
endif()

