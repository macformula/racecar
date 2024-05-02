add_library(mcal-raspi INTERFACE)

add_subdirectory(periph)
add_subdirectory(sil)

target_include_directories(mcal-raspi 
    INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../..

)

target_link_libraries(mcal-raspi INTERFACE shared)
target_link_libraries(mcal-raspi INTERFACE sil)