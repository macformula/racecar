add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/etl ${CMAKE_BINARY_DIR}/third-party)
target_link_libraries(main PRIVATE etl)

target_sources(main PUBLIC main.cc)
