include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main PRIVATE main.cc)

add_subdirectory(control-system)
target_link_libraries(main PRIVATE control-system)