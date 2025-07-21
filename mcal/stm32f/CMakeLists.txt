add_library(mcal-stm32f INTERFACE)
target_sources(mcal-stm32f INTERFACE can.cc)

target_include_directories(mcal-stm32f INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_link_libraries(mcal-stm32f INTERFACE shared)

