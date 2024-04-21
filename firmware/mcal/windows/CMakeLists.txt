add_library(mcal-windows INTERFACE)

target_include_directories(mcal-windows INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_subdirectory(periph)
target_link_libraries(mcal-windows INTERFACE shared)