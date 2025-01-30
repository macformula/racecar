target_sources(mcal-linux
    PRIVATE
    analog_input.cc
    can.cc
    digital_input.cc
    digital_output.cc
)

add_subdirectory(vcan)
target_link_libraries(mcal-linux PUBLIC vcan pthread)