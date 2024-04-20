# Blake Freer
# April 13, 2023
# This file is included in the cubemx/CMakeLists.txt file of all projects'
# platforms that use the stm32f767 mcal


if (AUTOGEN_CUBEMX)
    message(STATUS "Building CubeMX objects with the auto-generated Makefile")
    execute_process(
        # Calls generate_cubemx.mk in the context of this file, which is adjacent to
        # the board_config.ioc file.
        COMMAND ${CMAKE_MAKE_PROGRAM} "--file" "${CMAKE_SOURCE_DIR}/cmake/generate_cubemx.mk"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    string(CONCAT msg
    "Not autogenerating from CubeMX. You must manually call 'Generate "
    "Code' and append the custom_cubemx_targets to the resulting Makefile."
    )
    message(WARNING ${msg})
endif()

execute_process(
    # Build all cubemx sources to their objects. This target is added to the
    # cubemx Makefile by generate_cubemx.mk.
	COMMAND ${CMAKE_MAKE_PROGRAM} "objects"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

function(extract_make_variable output makefile var_name)
    # Get a variable from a Makefile
    # This requires the Makefile to have the following target & recipe
    # 
    # %.value:
    #     @echo $($*)
    # 
    # generate_cubemx.mk appends this to the cubemx Makefile
    # See custom_cubemx_targets.mk
    execute_process(
        COMMAND ${CMAKE_MAKE_PROGRAM} "--file=${makefile}" "${var_name}.value" "--no-print-directory"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND_ECHO STDOUT
        OUTPUT_VARIABLE value
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(VERBOSE "The make variable ${var_name} has the value \"${value}\"")
    set(${output} ${value} PARENT_SCOPE)
endfunction()

# Get C_INCLUDES, split string to a list, and remove the -I prefix
extract_make_variable(include_str "${CMAKE_CURRENT_SOURCE_DIR}/Makefile" "C_INCLUDES")
separate_arguments(C_INCLUDES NATIVE_COMMAND ${include_str})
list(TRANSFORM C_INCLUDES REPLACE "^-I" "")

# Get Global #defines
extract_make_variable(c_def_str "${CMAKE_CURRENT_SOURCE_DIR}/Makefile" "C_DEFS")
separate_arguments(C_DEFS NATIVE_COMMAND ${c_def_str})
list(TRANSFORM C_DEFS REPLACE "^-D" "")

# Get MCU which includes the CPU name (cortex-m7), floating point unit, etc
extract_make_variable(MCU "${CMAKE_CURRENT_SOURCE_DIR}/Makefile" "MCU")
separate_arguments(MCU NATIVE_COMMAND ${MCU})

# Get the linker flags. These are needed since we manually link the objects
# instead of using the cubemx Makefile.
extract_make_variable(ldflags_str "${CMAKE_CURRENT_SOURCE_DIR}/Makefile" "LDFLAGS")
separate_arguments(LDFLAGS NATIVE_COMMAND ${ldflags_str})
# Need to fully specify the link script path
list(TRANSFORM LDFLAGS REPLACE "^-T" "-T${CMAKE_CURRENT_SOURCE_DIR}/")
# Cannot have "build/" in front of the map output.
list(TRANSFORM LDFLAGS REPLACE "Map=.*\\.map" "Map=${CMAKE_PROJECT_NAME}.map")

# Prepare the driver target
add_library(driver)
set_target_properties(driver PROPERTIES LINKER_LANGUAGE C) # required since the source files are object files

# The sources are all the object files compiled with the CubeMX Makefile
FILE(GLOB OBJ_DRIVER CONFIGURE_DEPENDS "build/*.o")
target_sources(driver PUBLIC ${OBJ_DRIVER})

# Apply the Makefile settings to the targets
target_include_directories(driver PUBLIC ${C_INCLUDES})
target_compile_definitions(driver PUBLIC ${C_DEFS})

target_compile_options(driver
PUBLIC
	${MCU}
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

# Notice that the linker settings are set for the "main" target.
target_link_options(main
PRIVATE
    ${LDFLAGS}
    $<$<VERSION_GREATER:$<C_COMPILER_VERSION>,10.3.1>:-Wl,--no-warn-rwx-segments>
    -Wl,--start-group
    -Wl,--end-group
    -Wl,--print-memory-usage
)
