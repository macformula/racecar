# This file determines which mcal folder is included so that the mcal library
# can be linked to `bindings`.
set(MCAL stm32f767)
include(${CMAKE_SOURCE_DIR}/cmake/build_cubemx.cmake)