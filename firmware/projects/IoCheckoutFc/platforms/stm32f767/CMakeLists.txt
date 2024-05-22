# Blake Freer
# January 8, 2024

add_subdirectory(cubemx)

target_sources(bindings PUBLIC bindings.cc)
target_link_libraries(bindings PUBLIC driver mcal-stm32f767)
