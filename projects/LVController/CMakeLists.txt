include("${CMAKE_SOURCE_DIR}/cmake/cangen.cmake")

target_sources(main PRIVATE
    main.cc
    accumulator/accumulator.cc
    brakelight/brakelight.cc
    dcdc/dcdc.cc
    fans/fans.cc
    motor_controller/motor_controller.cc
    scheduler/scheduler.cc
    suspension/suspension.cc
    tssi/tssi.cc
)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
