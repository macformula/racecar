add_library(control-system INTERFACE)
add_subdirectory(src)
target_include_directories(control-system INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(control-system INTERFACE ${DIR_PROJECT})