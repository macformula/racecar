add_library(mcal-cli INTERFACE)

target_include_directories(mcal-cli INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_subdirectory(periph)
target_link_libraries(mcal-cli INTERFACE shared)