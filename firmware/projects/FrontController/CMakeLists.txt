include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main
    PRIVATE
    main.cc
    scheduler/scheduler.cc
)

add_library(control-system INTERFACE)
add_dependencies(control-system generated_can)
target_link_libraries(control-system INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(control-system
    INTERFACE
    motors/motor_interface.cc
    accumulator/accumulator.cc
    driver_interface/driver_interface.cc
    governor/governor.cc
    vehicle_dynamics/vehicle_dynamics.cc
    vehicle_dynamics/vehicle_dynamics_calc.cc
)

target_link_libraries(main PUBLIC control-system)

if(TESTING_ENABLED)
    target_link_libraries(tests PRIVATE control-system)

    target_sources(tests
        PRIVATE

        # accumulator/test_accumulator.cc
        governor/test_governor.cc
        vehicle_dynamics/test_vehicle_dynamics_calc.cc
        driver_interface/test_driver_interface.cc
        motors/test_motor_interface.cc
    )
endif()