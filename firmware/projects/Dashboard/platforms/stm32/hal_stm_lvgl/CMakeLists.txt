set(SRC screen_driver.c)
set_source_files_properties(${SRC} PROPERTIES LANGUAGE C)
add_library(hal_lvgl INTERFACE)
target_sources(hal_lvgl INTERFACE ${SRC})

target_include_directories(hal_lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})