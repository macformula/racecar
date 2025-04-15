message(FATAL_ERROR "STM32F4 platform does not have an mcal yet.")

target_sources(main PRIVATE bindings.cc)
target_link_libraries(main PRIVATE mcal-stm32f)

add_subdirectory(cubemx/cmake/stm32cubemx)
target_link_libraries(main PRIVATE stm32cubemx)
