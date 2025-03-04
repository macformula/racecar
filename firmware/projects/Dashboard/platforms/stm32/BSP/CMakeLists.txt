set(SRC
    STM32469I-Discovery/stm32469i_discovery_lcd.c
    STM32469I-Discovery/stm32469i_discovery_sdram.c
)
set_source_files_properties(${SRC} PROPERTIES LANGUAGE C)
add_library(bspstm INTERFACE)
target_sources(bspstm INTERFACE ${SRC})
target_include_directories(bspstm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/STM32469I-Discovery)

add_subdirectory(Components)