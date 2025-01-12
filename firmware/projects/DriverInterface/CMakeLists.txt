target_sources(main PRIVATE
    driver_interface.cc
    driver_interface_test.cc
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")