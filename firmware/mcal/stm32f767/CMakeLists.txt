add_library(mcal-stm32f767)

add_subdirectory(periph)
add_subdirectory(os)

target_include_directories(mcal-stm32f767 PUBLIC ${SMAKE_CURRENT_SOURCE_DIR}/..)
target_sources(mcal-stm32f767 PUBLIC retarget.cc)

target_link_libraries(mcal-stm32f767 PUBLIC shared)