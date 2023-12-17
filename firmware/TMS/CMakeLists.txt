# Blake Freer
# December 11, 2023

cmake_minimum_required(VERSION 3.12)
set(CMAKE_SYSTEM_NAME Generic)

set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_FLAGS_INIT "-fdata-sections -ffunction-sections --specs=nano.specs -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -fno-rtti -fno-exceptions -fno-threadsafe-statics --specs=nosys.specs")

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

project("TMS")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Use Debug mode by default
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

set(EXECUTABLE ${CMAKE_PROJECT_NAME})

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH DIR_FIRMWARE)
set(DIR_PROJECT_MCAL ${CMAKE_CURRENT_SOURCE_DIR}/mcal/${PLATFORM})
set(DIR_DRIVERS ${DIR_FIRMWARE}/drivers/${PLATFORM})

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_EXTENSIONS ON)


add_executable(${EXECUTABLE}
	${CMAKE_CURRENT_SOURCE_DIR}/main.cc
	${DIR_PROJECT_MCAL}/bindings.cc
)


option(PLATFORM "Platform to build for")
if(${PLATFORM} STREQUAL "windows")
	message("Building for windows")

elseif(${PLATFORM} STREQUAL "stm32f767")
	message("Building for stm32f767")

	set(CMAKE_SYSTEM_PROCESSOR arm)
	


	set(CPU_PARAMETERS
		-mcpu=cortex-m7
		-mthumb
		-mfpu=fpv5-d16
		-mfloat-abi=hard)

	# startup/linker scripts
	set(MCU_LINKER_SCRIPT ${DIR_PROJECT_MCAL}/STM32F767ZITx_FLASH.ld)

	target_sources(${EXECUTABLE} PRIVATE
		${DIR_PROJECT_MCAL}/startup_stm32f767xx.s
		${MCU_LINKER_SCRIPT}
	)
	
	# stm driver include directories
	target_include_directories(${EXECUTABLE} PRIVATE
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Inc
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Inc/Legacy
		${DIR_DRIVERS}/CMSIS/Device/ST/STM32F7xx/Include
		${DIR_DRIVERS}/CMSIS/Include
	)

	# stm mcal includes
	target_include_directories(${EXECUTABLE} PRIVATE
		${DIR_PROJECT_MCAL}/Inc
	)

	# stm driver source code
	target_sources(${EXECUTABLE} PRIVATE
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
		${DIR_DRIVERS}/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c
	)

	file(GLOB_RECURSE SRC_PROJECT_MCAL CMAKE_CONFIGURE_DEPENDS
		${DIR_PROJECT_MCAL}/Src/*.c
	)
	target_sources(${EXECUTABLE} PRIVATE ${SRC_PROJECT_MCAL})

	target_compile_definitions(${EXECUTABLE} PRIVATE
		USE_HAL_DRIVER
		STM32F767xx
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

else()
	message(FATAL_ERROR "Invalid Platform")
endif()


target_include_directories(${EXECUTABLE} PRIVATE
	${DIR_FIRMWARE}
)

# get correct bindings.h
target_include_directories(${EXECUTABLE} PRIVATE
	${DIR_PROJECT_MCAL}
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