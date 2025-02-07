#include "control-system/amk_block.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>

#include "control-system/enums.hpp"

// Define Dummy CAN messages to make the test module independent
// clang-format off
struct DummyActualValues1 {
    bool AMK_bSystemReady() const { return amk_b_system_ready; }
    bool AMK_bError() const { return amk_b_error; }
    bool AMK_bWarn() const { return amk_b_warn; }
    bool AMK_bQuitDcOn() const { return amk_b_quit_dc_on; }
    bool AMK_bDcOn() const { return amk_b_dc_on; }
    bool AMK_bQuitInverterOn() const { return amk_b_quit_inverter_on; }
    bool AMK_bInverterOn() const { return amk_b_inverter_on; }
    bool AMK_bDerating() const { return amk_b_derating; }
    int16_t AMK_ActualVelocity() const { return amk_actual_velocity; }
    int16_t AMK_TorqueCurrent() const { return amk_torque_current; }
    int16_t AMK_MagnetizingCurrent() const { return amk_magnetizing_current; }

    bool amk_b_system_ready; 
    bool amk_b_error; 
    bool amk_b_warn; 
    bool amk_b_quit_dc_on; 
    bool amk_b_dc_on; 
    bool amk_b_quit_inverter_on; 
    bool amk_b_inverter_on; 
    bool amk_b_derating; 
    int16_t amk_actual_velocity; 
    int16_t amk_torque_current; 
    int16_t amk_magnetizing_current; 
};

struct DummySetpoints1 {
    bool amk_b_inverter_on;
    bool amk_b_dc_on;
    bool amk_b_enable;
    bool amk_b_error_reset;
    int16_t amk__target_velocity;
    int16_t amk__torque_limit_positiv;
    int16_t amk__torque_limit_negativ;
};
// clang-format on

using AmkMgr = AmkManager<DummyActualValues1, DummySetpoints1>;

template <SetPoints SP>
void assert_setpoint_equal(SP actual_sp, SP expected_sp) {
    assert(actual_sp.amk_b_inverter_on == expected_sp.amk_b_inverter_on);
    assert(actual_sp.amk_b_dc_on == expected_sp.amk_b_dc_on);
    assert(actual_sp.amk_b_enable == expected_sp.amk_b_enable);
    assert(actual_sp.amk_b_error_reset == expected_sp.amk_b_error_reset);
    assert(actual_sp.amk__target_velocity == expected_sp.amk__target_velocity);
    assert(actual_sp.amk__torque_limit_positiv ==
           expected_sp.amk__torque_limit_positiv);
    assert(actual_sp.amk__torque_limit_negativ ==
           expected_sp.amk__torque_limit_negativ);
}

AmkManager<DummyActualValues1, DummySetpoints1> CycleToState(
    AmkManagerBase::FsmState desired_state) {
    using enum AmkManagerBase::FsmState;

    auto possible_states = {
        MOTOR_OFF_WAITING_FOR_GOV,
        STARTUP_SYS_READY,
        STARTUP_TOGGLE_D_CON,
        STARTUP_ENFORCE_SETPOINTS_ZERO,
        STARTUP_COMMAND_ON,
        READY,
        RUNNING,
        SHUTDOWN,
    };

    bool desired_state_is_supported = false;
    for (auto state : possible_states) {
        if (desired_state == state) {
            desired_state_is_supported = true;
            break;
        }
    }
    assert(desired_state_is_supported);

    AmkMgr mgr{};
    if (desired_state == MOTOR_OFF_WAITING_FOR_GOV) return mgr;

    DummyActualValues1 val1{};
    AmkMgr::Output out;
    AmkMgr::Request motor{};
    out = mgr.UpdateMotor(val1, motor, MiCmd::STARTUP, 0);
    assert(out.fsm_state == STARTUP_SYS_READY);
    if (desired_state == STARTUP_SYS_READY) return mgr;

    val1.amk_b_system_ready = true;
    out = mgr.UpdateMotor(val1, motor, MiCmd::STARTUP, 1);
    assert(out.fsm_state == STARTUP_TOGGLE_D_CON);
    if (desired_state == STARTUP_TOGGLE_D_CON) return mgr;

    val1.amk_b_dc_on = true;
    val1.amk_b_quit_dc_on = true;
    out = mgr.UpdateMotor(val1, motor, MiCmd::STARTUP, 2);
    assert(out.fsm_state == STARTUP_ENFORCE_SETPOINTS_ZERO);
    if (desired_state == STARTUP_ENFORCE_SETPOINTS_ZERO) return mgr;

    out = mgr.UpdateMotor(val1, motor, MiCmd::STARTUP, 150);
    assert(out.fsm_state == STARTUP_COMMAND_ON);
    if (desired_state == STARTUP_COMMAND_ON) return mgr;

    val1.amk_b_inverter_on = true;
    out = mgr.UpdateMotor(val1, motor, MiCmd::STARTUP, 151);
    assert(out.fsm_state == READY);
    if (desired_state == READY) return mgr;

    val1.amk_b_quit_inverter_on = true;
    val1.amk_b_error = false;
    out = mgr.UpdateMotor(val1, motor, MiCmd::STARTUP, 152);
    assert(out.fsm_state == RUNNING);
    if (desired_state == RUNNING) return mgr;

    out = mgr.UpdateMotor(val1, motor, MiCmd::SHUTDOWN, 152);
    assert(out.fsm_state == SHUTDOWN);
    return mgr;
}

// Tests the whole control model sequence from start to end
void test_normal_sequence() {
    std::cout << "test_normal_sequence... ";

    AmkMgr::Output output;
    AmkMgr amk_manager{};

    // Inputs to change and use in each Update call
    DummyActualValues1 actual_values{};
    AmkMgr::Request motor_input{.speed_request = 15,
                                .torque_limit_positive = 25,
                                .torque_limit_negative = 35};
    MiCmd cmd = MiCmd::STARTUP;
    int time_ms = 0;

    // Expected setpoints to change and use in assert calls
    DummySetpoints1 expected_setpoints{};

    std::cout
        << "Test transition MOTOR_OFF_WAITING_FOR_GOV to STARTUP_SYS_READY"
        << std::endl;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test transition STARTUP_SYS_READY to STARTUP_TOGGLE_D_CON"
              << std::endl;
    actual_values.amk_b_system_ready = true;
    expected_setpoints.amk_b_dc_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test transition STARTUP_TOGGLE_D_CON to "
                 "STARTUP_ENFORCE_SETPOINTS_ZERO"
              << std::endl;
    actual_values.amk_b_dc_on = true;
    actual_values.amk_b_quit_dc_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test NO transition in STARTUP_TOGGLE_D_CON with not enough "
                 "time elapsed"
              << std::endl;
    time_ms += 50;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test transition STARTUP_ENFORCE_SETPOINTS_ZERO to "
                 "STARTUP_COMMAND_ON"
              << std::endl;
    time_ms += 50;
    expected_setpoints.amk_b_enable = true;
    expected_setpoints.amk_b_inverter_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::STARTUP);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test transition STARTUP_COMMAND_ON to READY" << std::endl;
    actual_values.amk_b_inverter_on = true;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::READY);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == true);
    }

    std::cout << "Test transition READY to RUNNING" << std::endl;
    actual_values.amk_b_quit_inverter_on = true;
    expected_setpoints.amk__target_velocity = 15;
    expected_setpoints.amk__torque_limit_positiv = 25;
    expected_setpoints.amk__torque_limit_negativ = 35;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == true);
    }

    std::cout << "Test transition RUNNING to SHUTDOWN" << std::endl;
    cmd = MiCmd::SHUTDOWN;
    expected_setpoints.amk__target_velocity = 0;
    expected_setpoints.amk__torque_limit_positiv = 0;
    expected_setpoints.amk__torque_limit_negativ = 0;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test NO transition in SHUTDOWN with not enough time elapsed"
              << std::endl;
    time_ms += 250;
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::RUNNING);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }

    std::cout << "Test transition SHUTDOWN to MOTOR_OFF_WAITING_FOR_GOV"
              << std::endl;
    time_ms += 250;
    expected_setpoints = DummySetpoints1{};
    {
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::OFF);
        assert_setpoint_equal(output.setpoints, expected_setpoints);
        assert(output.inverter_enable == false);
    }
    std::cout << "passed!" << std::endl;
}

void test_error_detected_state() {
    std::cout << "test_error_detected_state... ";

    using enum AmkManagerBase::FsmState;

    // Define inputs/output
    AmkMgr::Output output;
    DummyActualValues1 actual_values;
    AmkMgr::Request motor_input{};
    MiCmd cmd = MiCmd::STARTUP;
    int time_ms = 0;

    // Set inputs to cause an error for testing
    actual_values.amk_b_error = true;

    std::cout << "Test transition STARTUP_SYS_READY to ERROR_DETECTED"
              << std::endl;
    {
        auto amk_manager = CycleToState(STARTUP_SYS_READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    std::cout << "Test transition STARTUP_TOGGLE_D_CON to ERROR_DETECTED"
              << std::endl;
    {
        auto amk_manager = CycleToState(STARTUP_TOGGLE_D_CON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    std::cout
        << "Test transition STARTUP_ENFORCE_SETPOINTS_ZERO to ERROR_DETECTED"
        << std::endl;
    {
        auto amk_manager = CycleToState(STARTUP_ENFORCE_SETPOINTS_ZERO);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    std::cout << "Test transition STARTUP_COMMAND_ON to ERROR_DETECTED"
              << std::endl;
    {
        auto amk_manager = CycleToState(STARTUP_COMMAND_ON);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    std::cout << "Test transition READY to ERROR_DETECTED" << std::endl;
    {
        auto amk_manager = CycleToState(READY);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    std::cout << "Test transition RUNNING to ERROR_DETECTED" << std::endl;
    {
        auto amk_manager = CycleToState(RUNNING);
        output =
            amk_manager.UpdateMotor(actual_values, motor_input, cmd, time_ms);
        assert(output.status == MiSts::ERR);
    }

    std::cout << "passed!" << std::endl;
}

void test_error_sequence() {
    std::cout << "test_error_sequence... ";

    using enum AmkManagerBase::FsmState;
    auto amk_manager = CycleToState(RUNNING);

    AmkMgr::Output output;

    // Inputs
    DummyActualValues1 actual_values;
    AmkMgr::Request motor_input{};
    int time_ms = 0;

    // put amk into error state
    output = amk_manager.UpdateMotor({.amk_b_error = true}, motor_input,
                                     MiCmd::ERR_RESET, time_ms);
    assert(output.fsm_state == ERROR_DETECTED);
    assert(output.status == MiSts::ERR);

    std::cout << "Test transition ERROR_DETECTED to "
                 "ERROR_RESET_ENFORCE_SETPOINTS_ZERO"
              << std::endl;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         MiCmd::ERR_RESET, time_ms);
        assert(output.setpoints.amk__target_velocity == 0);
        assert(output.setpoints.amk__torque_limit_positiv == 0);
        assert(output.setpoints.amk__torque_limit_positiv == 0);
        assert(output.setpoints.amk_b_inverter_on == false);
    }

    std::cout << "Test transition ERROR_RESET_ENFORCE_SETPOINTS_ZERO to "
                 "ERROR_RESET_TOGGLE_ENABLE"
              << std::endl;
    {
        output =
            amk_manager.UpdateMotor({.amk_b_quit_inverter_on = false},
                                    motor_input, MiCmd::ERR_RESET, time_ms);
        assert(output.fsm_state == ERROR_RESET_TOGGLE_ENABLE);
        assert(output.setpoints.amk_b_enable == false);
    }

    std::cout << "Test NO transition in ERROR_RESET_TOGGLE_ENABLE with not "
                 "enough time"
              << std::endl;
    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         MiCmd::ERR_RESET, time_ms);
        assert(output.fsm_state == ERROR_RESET_TOGGLE_ENABLE);
    }

    std::cout
        << "Test transition ERROR_RESET_TOGGLE_ENABLE to ERROR_RESET_SEND_RESET"
        << std::endl;
    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         MiCmd::ERR_RESET, time_ms);
        assert(output.fsm_state == ERROR_RESET_SEND_RESET);
        assert(output.setpoints.amk_b_error_reset == true);
    }

    std::cout << "Test NO transition in ERROR_RESET_SEND_RESET" << std::endl;
    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         MiCmd::ERR_RESET, time_ms);
        assert(output.fsm_state == ERROR_RESET_SEND_RESET);
    }

    std::cout
        << "Test transition ERROR_RESET_SEND_RESET to ERROR_RESET_TOGGLE_RESET"
        << std::endl;
    time_ms += 250;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         MiCmd::ERR_RESET, time_ms);
        assert(output.fsm_state == ERROR_RESET_TOGGLE_RESET);
        assert(output.setpoints.amk_b_error_reset == false);
    }

    std::cout << "Test transition ERROR_RESET_TOGGLE_RESET to "
                 "MOTOR_OFF_WAITING_FOR_GOV"
              << std::endl;
    actual_values.amk_b_system_ready = true;
    {
        output = amk_manager.UpdateMotor(actual_values, motor_input,
                                         MiCmd::ERR_RESET, time_ms);
        assert(output.status == MiSts::OFF);
        assert(output.setpoints.amk_b_inverter_on == false);
        assert(output.setpoints.amk_b_dc_on == false);
        assert(output.setpoints.amk_b_enable == false);
        assert(output.setpoints.amk_b_error_reset == false);
        assert(output.setpoints.amk__target_velocity == false);
        assert(output.setpoints.amk__torque_limit_positiv == false);
        assert(output.setpoints.amk__torque_limit_positiv == false);
        assert(output.inverter_enable == false);
    }

    std::cout << "passed!" << std::endl;
}

void AmkTest() {
    test_normal_sequence();
    test_error_detected_state();
    test_error_sequence();

    std::cout << "All AMK tests passed!" << std::endl;
}