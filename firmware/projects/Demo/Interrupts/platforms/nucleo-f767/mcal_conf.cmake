# This file determines which mcal folder is included so that the mcal library
# can be linked to `bindings`.
set(MCAL stm32f)
include(${CMAKE_SOURCE_DIR}/cmake/build_cubemx.cmake)
add_compile_definitions(STM32F7)