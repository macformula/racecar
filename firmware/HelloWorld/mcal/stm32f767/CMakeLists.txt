# Blake Freer
# December 19, 2023

set(CPU_PARAMETERS
	-mcpu=cortex-m7
	-mthumb
	-mfpu=fpv5-d16
	-mfloat-abi=hard
)

add_library(driver)

target_compile_definitions(driver
PUBLIC
	USE_HAL_DRIVER
	STM32F767xx
)

target_sources(driver
PRIVATE
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
	cubemx/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
PUBLIC
	cubemx/startup_stm32f767xx.s
)

target_include_directories(driver
PUBLIC
	cubemx/Drivers/CMSIS/Device/ST/STM32F7xx/Include
	cubemx/Drivers/CMSIS/Include
	cubemx/Drivers/STM32F7xx_HAL_Driver/Inc
	cubemx/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
)

target_include_directories(driver
PUBLIC
	cubemx/Inc
)

target_compile_options(driver
PUBLIC
	${CPU_PARAMETERS}
	-fdata-sections
	-ffunction-sections
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
		-Wno-old-style-cast
		-Wno-useless-cast
		-Wconversion
		-Wno-volatile
		-Wsuggest-override>
	$<$<CONFIG:Debug>:-Og -g3 -ggdb>
	$<$<CONFIG:Release>:-Og -g0>
)

add_library(bindings)

target_sources(bindings
PRIVATE
	bindings.cc
)

target_sources(bindings
PRIVATE
	cubemx/Src/gpio.c
	cubemx/Src/main.c
	cubemx/Src/adc.c
	cubemx/Src/tim.c
	cubemx/Src/stm32f7xx_hal_msp.c
	cubemx/Src/stm32f7xx_it.c
	cubemx/Src/system_stm32f7xx.c
)

target_include_directories(bindings
PUBLIC
	cubemx/Inc
)

# must be public so that the 'main' executable can use its headers
target_link_libraries(bindings PUBLIC driver)

target_link_options(main PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/cubemx/STM32F767ZITx_FLASH.ld
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