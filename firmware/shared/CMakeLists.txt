add_library(shared STATIC)

target_include_directories(shared PUBLIC ${CMAKE_SOURCE_DIR})

add_subdirectory(comms)
add_subdirectory(periph)