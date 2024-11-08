add_library(mcal-linux STATIC)

target_include_directories(mcal-linux PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_subdirectory(periph)
target_link_libraries(mcal-linux INTERFACE shared)
