target_sources(bindings PRIVATE bindings.cc)
target_include_directories(bindings PRIVATE ${PROJECT_DIR}/inc)

target_link_libraries(bindings PUBLIC mcal-cli)