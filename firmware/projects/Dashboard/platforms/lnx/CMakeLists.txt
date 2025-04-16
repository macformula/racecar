target_sources(main PRIVATE bindings.cc lvgl_linux.cc)
target_link_libraries(main PRIVATE mcal-linux)

add_definitions(-DSIMULATOR=1 -DLV_BUILD_TEST=0)

add_subdirectory(lv_drivers) # do we need this? seems to only be for touch screen simulators
target_link_libraries(main PRIVATE lv_drivers)
target_link_libraries(lv_drivers PRIVATE lvgl)
target_include_directories(lv_drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL2 REQUIRED)
target_link_libraries(lvgl PUBLIC SDL2::SDL2)

target_compile_definitions(lvgl PUBLIC LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})