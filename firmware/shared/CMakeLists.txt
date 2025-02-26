add_library(shared INTERFACE)

target_include_directories(shared INTERFACE ${CMAKE_SOURCE_DIR})

add_subdirectory(comms)
add_subdirectory(periph)

add_subdirectory(util)