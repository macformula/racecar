target_sources(control-system
    INTERFACE
    motor_interface.cc
    battery_monitor.cc
    driver_interface.cc
    governor.cc
    vehicle_dynamics.cc
    vehicle_dynamics_calc.cc
)