target_link_libraries(main PRIVATE mcal-stm32f4)
target_sources(main PRIVATE bindings.cc)

add_subdirectory(cubemx/cmake/stm32cubemx)
target_link_libraries(main PRIVATE stm32cubemx)

add_subdirectory(hal_stm_lvgl)
add_subdirectory(BSP)

target_link_libraries(main PRIVATE
    hal_lvgl
    bsp
)

target_link_libraries(lvgl PUBLIC stm32cubemx)

target_compile_definitions(lvgl PUBLIC LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h)
