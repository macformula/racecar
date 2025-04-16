include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

add_subdirectory(lvgl)
target_link_libraries(main PRIVATE lvgl)

target_sources(main PRIVATE main.cc)
add_subdirectory(src)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
