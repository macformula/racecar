# Blake Freer
# April 13, 2023
# This file is included in the cubemx/CMakeLists.txt file of all projects'
# platforms that use the stm32f767 mcal

# Calls generate_cubemx.mk in the context of this file, which is adjacent to
# the board_config.ioc file.
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