include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main PRIVATE main.cc)
add_subdirectory(app)