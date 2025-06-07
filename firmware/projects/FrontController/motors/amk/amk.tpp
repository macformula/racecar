#pragma once

#include "amk.hpp"

namespace amk {

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
Amk<AV1, AV2, SP>::Amk(void) {
    Init();
}

// ---------- Accessors ----------

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
SP Amk<AV1, AV2, SP>::GetSetpoints(void) const {
    return setpoints;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
uint16_t Amk<AV1, AV2, SP>::GetErrorCode(void) const {
    return error_code;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
bool Amk<AV1, AV2, SP>::HasError(void) const {
    return error_code != 0;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
bool Amk<AV1, AV2, SP>::IsReadyForInverter(void) const {
    return ready_for_inverter;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
bool Amk<AV1, AV2, SP>::IsRunning(void) const {
    return state == State::RUNNING;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
State Amk<AV1, AV2, SP>::GetState(void) const {
    return state;
}

// ---------- Modifiers ----------

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
void Amk<AV1, AV2, SP>::SetCommand(Command _command) {
    command = _command;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
void Amk<AV1, AV2, SP>::SetRequest(Request _request) {
    request = _request;
}

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
void Amk<AV1, AV2, SP>::ProceedFromInverter(void) {
    inverter_is_set_flag = true;
}

// ---------- Behaviour ----------

template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
void Amk<AV1, AV2, SP>::Init(void) {
    state = State::OFF;
    setpoints = SP{
        .b_inverter_on = false,
        .b_dc_on = false,
        .b_enable = false,
        .b_error_reset = false,
        .target_velocity = 0,
        .torque_limit_positiv = 0,
        .torque_limit_negativ = 0,
    };

    elapsed = 0;
    ready_for_inverter = false;
    inverter_is_set_flag = false;
}

// Written according to "AMK Motors 2015" document
template <ActualValues1 AV1, ActualValues2 AV2, Setpoints1 SP>
void Amk<AV1, AV2, SP>::Update_100Hz(AV1 av1, AV2 av2) {
    using enum State;
    State new_state = state;

    SP new_sp{};
    bool new_ready_for_inverter = false;

    switch (state) {
        case State::OFF:
            new_sp = SP{
                .b_inverter_on = false,
                .b_dc_on = false,
                .b_enable = false,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if (av1.bSystemReady() && (elapsed > 100)) {
                new_state = SYSTEM_READY;
            }
            break;

        case State::SYSTEM_READY:
            new_sp = SP{
                .b_inverter_on = false,
                .b_dc_on = false,
                .b_enable = false,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if (command == Command::ENABLED && (elapsed > 100)) {
                new_state = STARTUP_bDCON;
            }
            break;

        case State::STARTUP_bDCON:
            new_sp = SP{
                .b_inverter_on = false,
                .b_dc_on = true,
                .b_enable = false,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if (av1.bQuitDcOn() && (elapsed > 100)) {
                new_state = STARTUP_bENABLE;
            }

            break;

        case State::STARTUP_bENABLE:
            new_sp = SP{
                .b_inverter_on = false,
                .b_dc_on = true,
                .b_enable = true,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if (elapsed >= 100) {
                new_state = STARTUP_bINVERTER;
            }

            break;

        case State::STARTUP_bINVERTER:
            new_sp = SP{
                .b_inverter_on = true,
                .b_dc_on = true,
                .b_enable = true,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if (av1.bQuitInverterOn() && (elapsed > 100)) {
                new_state = STARTUP_X140;
                // ensure we wait on an external signal
                inverter_is_set_flag = false;
            }

            break;

        case State::STARTUP_X140:
            new_sp = SP{
                .b_inverter_on = true,
                .b_dc_on = true,
                .b_enable = true,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = true;

            if (inverter_is_set_flag) {
                inverter_is_set_flag = false;
                new_state = RUNNING;
            }
            break;

        case State::RUNNING:
            new_sp = SP{
                .b_inverter_on = true,
                .b_dc_on = true,
                .b_enable = true,
                .b_error_reset = false,
                .target_velocity = static_cast<int16_t>(request.speed_request),
                .torque_limit_positiv =
                    static_cast<int16_t>(request.torque_limit_positive),
                .torque_limit_negativ =
                    static_cast<int16_t>(request.torque_limit_negative),
            };
            new_ready_for_inverter = true;

            break;

        case State::ERROR:
            new_sp = SP{
                .b_inverter_on = false,
                .b_dc_on = true,
                .b_enable = false,
                .b_error_reset = false,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if (command == Command::ERROR_RESET) {
                new_state = ERROR_RESET;
            }
            break;

        case State::ERROR_RESET:
            new_sp = SP{
                .b_inverter_on = false,
                .b_dc_on = true,
                .b_enable = false,
                .b_error_reset = true,
                .target_velocity = 0,
                .torque_limit_positiv = 0,
                .torque_limit_negativ = 0,
            };
            new_ready_for_inverter = false;

            if ((command == Command::OFF) && (av1.bError() == false)) {
                new_state = OFF;
            }
    }

    if (command == Command::OFF) {
        // Shutdown sequence is the same as going straight to IDLE
        new_state = OFF;
    }

    error_code = av2.ErrorInfo();
    if (av1.bError()) {
        new_state = ERROR;
    }

    setpoints = new_sp;
    ready_for_inverter = new_ready_for_inverter;

    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

}  // namespace amk