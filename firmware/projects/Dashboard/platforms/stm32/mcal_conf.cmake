set(MCAL stm32f4)
include(${CMAKE_SOURCE_DIR}/cmake/build_cubemx.cmake)

# replace the generated linker file with our own
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/STM32F469NIHX_FLASH.ld
    ${CMAKE_CURRENT_LIST_DIR}/cubemx/STM32F469NIHX_FLASH.ld
    COPYONLY
)