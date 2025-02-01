# Add linked libraries
add_subdirectory(Drivers)

target_compile_definitions(lvgl PUBLIC LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Drivers/lv_conf.h)
target_link_libraries(lvgl PUBLIC stm32cubemx)

target_link_libraries(main
    PRIVATE
    stm32cubemx
    lvgl
    hal_lvgl
    bspstm
    bspcomp

    # Add user defined libraries
)

target_sources(main PRIVATE main.cc)

add_subdirectory(src)

target_link_directories(lvgl PRIVATE Drivers)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Drivers)
