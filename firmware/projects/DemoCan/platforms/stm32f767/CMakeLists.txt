add_subdirectory(cubemx)

target_sources(bindings PUBLIC bindings.cc)
target_link_libraries(bindings PUBLIC driver mcal-stm32f767)

# include_directories(${CMAKE_SOURCE_DIR}/mcal/stm32f767/os)
# add_library(os)
# target_link_libraries(os PUBLIC driver mcal-stm32f767)
