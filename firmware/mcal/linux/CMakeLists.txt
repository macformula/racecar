add_library(mcal-linux INTERFACE)

target_include_directories(mcal-linux INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_link_libraries(mcal-linux INTERFACE shared)

target_sources(mcal-linux
    INTERFACE
    analog_input.cc
    can.cc
    digital_input.cc
    digital_output.cc
    pwm_output.cc
)

add_subdirectory(vcan)
target_link_libraries(mcal-linux INTERFACE vcan pthread)
