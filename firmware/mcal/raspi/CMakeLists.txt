add_library(mcal-raspi INTERFACE)

add_subdirectory(periph)

target_include_directories(mcal-raspi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(mcal-raspi INTERFACE shared)