# Blake Freer
# January 8, 2023

message(STATUS "Building CubeMX objects with the auto-generated Makefile")
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Makefile")
	message(FATAL_ERROR "No auto-generated Makefile was found. Open the .ioc file in STM32CubeMX and click [Generate Code]")
endif()
execute_process(
	COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(driver)
set_target_properties(driver PROPERTIES LINKER_LANGUAGE C) # required since the source files are object files

# The sources are all the object files compiled with the CubeMX Makefile
FILE(GLOB OBJ_DRIVER CONFIGURE_DEPENDS "build/*.o")
target_sources(driver
PUBLIC
	${OBJ_DRIVER}
)

target_include_directories(driver
PUBLIC
	Drivers/CMSIS/Device/ST/STM32F7xx/Include
	Drivers/CMSIS/Include
	Drivers/STM32F7xx_HAL_Driver/Inc
	Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
	Inc
)

# The rest of the file specifies parameters required to link the object files.
# This is highly duplicated from the CubeMX generated Makefile, which is not ideal as
# it requires manual adjustment when the Makefile changes.

set(CPU_PARAMETERS
	-mcpu=cortex-m7
	-mthumb
	-mfpu=fpv5-d16
	-mfloat-abi=hard
)

target_compile_definitions(driver
PUBLIC
	USE_HAL_DRIVER
	STM32F767xx
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

# Notice that the linker settings are set for the "main" target
target_link_options(main PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F767ZITx_FLASH.ld
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