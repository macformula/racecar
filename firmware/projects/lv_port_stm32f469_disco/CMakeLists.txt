# Add linked libraries
add_subdirectory(Drivers)
target_link_libraries(main
    PRIVATE
    stm32cubemx
    lvgl
    hal_lvgl
    bspstm
    bspcomp

    # Add user defined libraries
)

target_sources(main PRIVATE main.c)