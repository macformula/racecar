include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main
    PRIVATE
    main.cc
    alerts/alerts.cc
    accumulator/accumulator.cc
    dbc_hash/dbc_hash.cc
    driver_interface/driver_interface.cc
    motors/amk/starter.cc
    motors/motors.cc
    sensors/driver/driver.cc
    sensors/dynamics/dynamics.cc
    vehicle_dynamics/vehicle_dynamics_calc.cc
    vehicle_dynamics/vehicle_dynamics.cc
)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(main PUBLIC USE_FREERTOS)