target_sources(mcal-linux
    INTERFACE
    analog_input.cc
    can.cc
    digital_input.cc
    digital_output.cc
)

add_subdirectory(vcan)
target_link_libraries(mcal-linux INTERFACE vcan pthread)