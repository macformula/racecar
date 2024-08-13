add_library(mcal-arduino INTERFACE)

add_subdirectory(periph)

target_include_directories(mcal-arduino INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_link_libraries(mcal-arduino INTERFACE shared)