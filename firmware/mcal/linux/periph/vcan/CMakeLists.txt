add_library(vcan STATIC vcan.cc)
target_include_directories(vcan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})