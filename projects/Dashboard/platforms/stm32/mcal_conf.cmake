set(MCAL stm32f)
include(${CMAKE_SOURCE_DIR}/cmake/build_cubemx.cmake)
add_compile_definitions(STM32F4)

# replace the generated linker file with our own
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/STM32F469NIHx_FLASH.ld
    ${CMAKE_CURRENT_LIST_DIR}/cubemx/STM32F469NIHx_FLASH.ld
    COPYONLY
)