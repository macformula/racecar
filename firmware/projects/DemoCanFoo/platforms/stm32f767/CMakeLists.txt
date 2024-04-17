add_subdirectory(cubemx)

target_include_directories(bindings PUBLIC ${CMAKE_SOURCE_DIR}/mcal/os)
target_sources(bindings PUBLIC bindings.cc)
target_link_libraries(bindings PUBLIC driver mcal-stm32f767)

add_library(os)
target_sources(os PUBLIC ${CMAKE_SOURCE_DIR}/mcal/stm32f767/os/tick_blocking.cc)
target_link_libraries(os PUBLIC driver mcal-stm32f767)

