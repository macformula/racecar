#include "governor.hpp"

#include "bindings.hpp"
#include "driver_interface/driver_interface.hpp"
#include "thresholds.hpp"

namespace governor {

static accumulator::Command accumulator_cmd;
static motors::Command motor_cmd;
static State state;

static uint32_t elapsed;
static bool on_enter;

accumulator::Command GetAccumulatorCmd(void) {
    return accumulator_cmd;
}

motors::Command GetMotorCmd(void) {
    return motor_cmd;
}

uint32_t GetElapsed(void) {
    return elapsed;
}

State GetState(void) {
    return state;
}

void Init(void) {
    accumulator_cmd = accumulator::Command::OFF;
    motor_cmd = motors::Command::OFF;
    state = State::INIT;
    elapsed = 0;
}

void Update_100Hz(accumulator::State acc, motors::State mi, DashState dash) {
    using enum State;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    State new_state = state;

    switch (state) {
        case INIT:
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;

            if ((elapsed >= 2000) && (dash == DashState::STARTING_HV)) {
                new_state = STARTUP_HV;
            }
            break;

        case STARTUP_HV:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;

            if (acc == accumulator::State::RUNNING) {
                new_state = STARTUP_READY_TO_DRIVE;
            }
            break;

        case STARTUP_READY_TO_DRIVE:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;

            if (dash == DashState::STARTING_MOTORS) {
                new_state = STARTUP_MOTOR;
            }
            break;

        case STARTUP_MOTOR:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;

            if (mi == motors::State::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (mi == motors::State::ERROR_STARTUP) {
                new_state = ERROR_UNRECOVERABLE;  // don't keep trying
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;

            if (driver_interface::IsBrakePressed()) {
                new_state = RUNNING;
            }
            break;

        case RUNNING:
            bindings::ready_to_drive_sig_en.Set(elapsed <
                                                timeout::SPEAKER_DURATION);

            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;

            if (acc == accumulator::State::LOW_SOC) {
                new_state = SHUTDOWN;
            }

            if (acc == accumulator::State::ERR_RUNNING) {
                new_state = ERROR_UNRECOVERABLE;
            }

            if (mi == motors::State::ERROR_RUNNING) {
                new_state = ERROR_UNRECOVERABLE;
            }

            break;

        case SHUTDOWN:
            // can this be combined with ERR_STARTUP_HV?
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;

            if (acc == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = INIT;
            }
            break;

        case ERR_STARTUP_HV:
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;

            if (acc == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = INIT;
            }
            break;

        // The vehicle cannot escape these error states without rebooting.
        case ERROR_UNRECOVERABLE:
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            break;
    }

    if (hvil_interrupt) {
        new_state = SHUTDOWN;
    }

    on_enter = new_state != state;
    if (on_enter) {
        elapsed = 0;
        state = new_state;
    }
    elapsed += 10;
}

}  // namespace governor
