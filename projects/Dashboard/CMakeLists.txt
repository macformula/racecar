include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

add_subdirectory(lvgl)
target_compile_options(lvgl PRIVATE "-w")
target_link_libraries(main PRIVATE lvgl)
set_target_properties(lvgl_examples PROPERTIES EXCLUDE_FROM_ALL true)
set_target_properties(lvgl_demos PROPERTIES EXCLUDE_FROM_ALL true)

target_sources(main PRIVATE main.cc)
add_subdirectory(src)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
