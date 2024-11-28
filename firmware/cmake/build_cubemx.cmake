# Blake Freer
# November 2024

# Generate code from CubeMX. The Makefile modifies the toolchain to fit our
# system.
# This file is included in the mcal_conf.cmake file of all projects'
# platforms that use the stm32f767 mcal

execute_process(
    COMMAND make "--file=${CMAKE_SOURCE_DIR}/cmake/generate_cubemx.mk"
    WORKING_DIRECTORY ${DIR_PLATFORM}/cubemx
    RESULT_VARIABLE status
)

if(status AND NOT status EQUAL 0)
    message(FATAL_ERROR
        "${out} Failed to generate from CubeMX. You should manually 'Generate Code' before building."
    )
endif()

set(CMAKE_TOOLCHAIN_FILE "${DIR_PLATFORM}/cubemx/cmake/racecar-toolchain.cmake")