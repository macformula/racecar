target_sources(bindings PRIVATE bindings.cc)
target_include_directories(bindings PRIVATE ${DIR_PROJECT})

target_link_libraries(bindings PUBLIC mcal-stm32f767)

add_subdirectory(cubemx)  # provides "driver"
target_link_libraries(bindings PUBLIC driver)