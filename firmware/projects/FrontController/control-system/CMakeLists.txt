add_library(fc-control INTERFACE)
add_subdirectory(src)
target_include_directories(fc-control INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})