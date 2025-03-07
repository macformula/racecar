target_sources(main PRIVATE bindings.cc)
target_link_libraries(main PUBLIC mcal-stm32f767)

add_subdirectory(cubemx/cmake/stm32cubemx)
target_link_libraries(main PRIVATE stm32cubemx)
