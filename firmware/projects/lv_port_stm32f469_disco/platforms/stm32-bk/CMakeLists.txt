target_link_libraries(main PRIVATE mcal-stm32f4)
target_sources(main PRIVATE bindings.cc)

add_subdirectory(cubemx/cmake/stm32cubemx)
target_link_libraries(main PRIVATE stm32cubemx)