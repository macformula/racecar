add_library(mcal-stm32f767 INTERFACE)

add_subdirectory(periph)
add_subdirectory(os)

target_include_directories(mcal-stm32f767 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(mcal-stm32f767 INTERFACE shared)