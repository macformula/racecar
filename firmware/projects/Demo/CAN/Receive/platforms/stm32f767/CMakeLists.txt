add_subdirectory(cubemx/cmake/stm32cubemx)

target_sources(bindings PUBLIC bindings.cc)
target_link_libraries(bindings PUBLIC stm32cubemx mcal-stm32f767)
