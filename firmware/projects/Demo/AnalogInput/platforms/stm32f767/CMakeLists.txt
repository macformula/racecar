target_sources(bindings PRIVATE bindings.cc)

target_link_libraries(bindings PUBLIC mcal-stm32f767)