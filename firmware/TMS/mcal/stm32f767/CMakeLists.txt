# Blake Freer
# December 19, 2023

set(CPU_PARAMETERS
	${CPU_PARAMETERS}
	-mcpu=cortex-m7
	-mthumb
	-mfpu=fpv5-d16
	-mfloat-abi=hard
)

set(COMPILE_DEFINITIONS
	${COMPILE_DEFINITIONS}
	USE_HAL_DRIVER
	STM32F767xx
)

add_library(bindings)

target_sources(bindings
PRIVATE
	bindings.cc
)

target_sources(bindings
PUBLIC
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c
)

set(DRIVER_INC
	cubemx/Drivers/CMSIS/Device/ST/STM32F7xx/Include
	cubemx/Drivers/CMSIS/Include
	cubemx/Drivers/STM32F7xx_HAL_Driver/Inc
	cubemx/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
)

target_include_directories(bindings
PRIVATE
	${DRIVER_INC}
)

target_sources(bindings
PUBLIC
	cubemx/Src/adc.c
	cubemx/Src/gpio.c
	cubemx/Src/main.c
	cubemx/Src/stm32f7xx_hal_msp.c
	cubemx/Src/stm32f7xx_it.c
	cubemx/Src/system_stm32f7xx.c
	cubemx/Src/tim.c
)

target_include_directories(bindings
PRIVATE
	cubemx/Inc
)

target_compile_definitions(bindings
PUBLIC
	USE_HAL_DRIVER
	STM32F767xx
)

target_compile_options(bindings
PUBLIC
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
		-Wno-useless-cast
		-Wconversion
		-Wno-volatile
		-Wold-style-cast
		-Wsuggest-override>
	$<$<CONFIG:Debug>:-Og -g3 -ggdb>
	$<$<CONFIG:Release>:-Og -g0>
)

target_link_options(main PRIVATE
    -T cubemx/STM32F767ZITx_FLASH.ld
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

target_include_directories(main
PUBLIC
	${DRIVER_INC}
	cubemx/Inc
)