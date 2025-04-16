set(SRC
    32f469idiscovery-bsp/stm32469i_discovery_lcd.c
    32f469idiscovery-bsp/stm32469i_discovery_sdram.c
    Components/nt35510/nt35510.c
    Components/otm8009a/otm8009a.c
    io_delay.cc
)

set_source_files_properties(${SRC} PROPERTIES LANGUAGE CXX)
add_library(bsp INTERFACE)
target_sources(bsp INTERFACE ${SRC})

target_include_directories(bsp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/32f469idiscovery-bsp)