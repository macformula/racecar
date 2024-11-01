target_sources(bindings PRIVATE bindings.cc)

target_link_libraries(bindings PUBLIC mcal-cli)

add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/etl ${CMAKE_BINARY_DIR}/third-party)
target_link_libraries(main PRIVATE etl)