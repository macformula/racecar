add_library(mcal-linux INTERFACE)

target_include_directories(mcal-linux INTERFCAE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_subdirectory(periph)
target_link_libraries(mcal-linux INTERFACE shared)