include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main
    PRIVATE
    main.cc
    scheduler/scheduler.cc
    sensors/dynamics/dynamics.cc
    sensors/driver/driver.cc
    motors/motors.cc
    motors/amk/starter.cc
    accumulator/accumulator.cc
    driver_interface/driver_interface.cc
    governor/governor.cc
    vehicle_dynamics/vehicle_dynamics.cc
    vehicle_dynamics/vehicle_dynamics_calc.cc
)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# if(TESTING_ENABLED)
# target_link_libraries(tests PRIVATE control-system)

# target_sources(tests
# PRIVATE

# # accumulator/test_accumulator.cc
# # governor/test_governor.cc
# # vehicle_dynamics/test_vehicle_dynamics_calc.cc
# # motors/test_motor_interface.cc
# )
# endif()
