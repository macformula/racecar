#include "governor.hpp"

namespace governor {

static accumulator::Command accumulator_cmd;
static motor::Command motor_cmd;
static driver_interface::Command driver_cmd;
static State state;

static uint32_t elapsed;
static bool on_enter;

static const uint32_t kMotorMaxAttempts = 5;
static uint32_t motor_start_count;  // this should be handled in motors/

accumulator::Command GetAccumulatorCmd(void) {
    return accumulator_cmd;
}

motor::Command GetMotorCmd(void) {
    return motor_cmd;
}

driver_interface::Command GetDriverInterfaceCmd(void) {
    return driver_cmd;
}

State GetState(void) {
    return state;
}

void Init(void) {
    accumulator_cmd = accumulator::Command::OFF;
    motor_cmd = motor::Command::INIT;
    driver_cmd = driver_interface::Command::INIT;
    state = State::INIT;
    elapsed = 0;
    motor_start_count = 0;
}

void Update_100Hz(accumulator::State acc, motor::State mi,
                  driver_interface::State di) {
    using enum State;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    State new_state = state;

    switch (state) {
        case INIT:
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motor::Command::INIT;
            driver_cmd = driver_interface::Command::INIT;
            motor_start_count = 0;

            if ((elapsed >= 2000) &&
                (di == driver_interface::State::REQUESTED_HV_START)) {
                new_state = STARTUP_HV;
            }
            break;

        case STARTUP_HV:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motor::Command::INIT;
            driver_cmd = driver_interface::Command::INIT;

            if (acc == accumulator::State::RUNNING) {
                new_state = STARTUP_READY_TO_DRIVE;
            }
            break;

        case STARTUP_READY_TO_DRIVE:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motor::Command::INIT;
            driver_cmd = driver_interface::Command::HV_IS_ON;

            if (di == driver_interface::State::REQUESTED_MOTOR_START) {
                new_state = STARTUP_MOTOR;
            }
            break;

        case STARTUP_MOTOR:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motor::Command::STARTUP;
            driver_cmd = driver_interface::Command::HV_IS_ON;

            if (on_enter) {
                motor_start_count++;
            }

            if (mi == motor::State::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (mi == motor::State::ERR) {
                if (motor_start_count < kMotorMaxAttempts) {
                    new_state = ERR_STARTUP_MOTOR_RESET;
                } else {
                    new_state = ERR_STARTUP_MOTOR_FAULT;  // don't keep trying
                }
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motor::Command::STARTUP;
            driver_cmd = driver_interface::Command::READY_TO_DRIVE;

            if (di == driver_interface::State::RUNNING) {
                new_state = RUNNING;
            }
            break;

        case RUNNING:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motor::Command::STARTUP;
            driver_cmd = driver_interface::Command::READY_TO_DRIVE;

            if (acc == accumulator::State::LOW_SOC) {
                new_state = SHUTDOWN;
            }

            if (acc == accumulator::State::ERR_RUNNING) {
                new_state = ERR_RUNNING_HV;
            }

            if (mi == motor::State::ERR) {
                new_state = ERR_RUNNING_MOTOR;
            }

            break;

        case SHUTDOWN:
            // can this be combined with ERR_STARTUP_HV?
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motor::Command::SHUTDOWN;
            driver_cmd = driver_interface::Command::SHUTDOWN;

            if (acc == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = INIT;
            }
            break;

        case ERR_STARTUP_HV:
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motor::Command::SHUTDOWN;
            driver_cmd = driver_interface::Command::SHUTDOWN;

            if (acc == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = INIT;
            }
            break;

        case ERR_STARTUP_MOTOR_RESET:
            accumulator_cmd = accumulator::Command::ENABLED;
            motor_cmd = motor::Command::ERR_RESET;
            driver_cmd =
                driver_interface::Command::HV_IS_ON;  // is this correct?

            if (mi == motor::State::OFF) {
                new_state = STARTUP_MOTOR;
            }
            break;

        // The vehicle cannot escape these error states without rebooting.
        case ERR_STARTUP_MOTOR_FAULT:
        case ERR_STARTUP_DI:
        case ERR_RUNNING_HV:
        case ERR_RUNNING_MOTOR:
            accumulator_cmd = accumulator::Command::OFF;
            motor_cmd = motor::Command::SHUTDOWN;
            driver_cmd = driver_interface::Command::RUN_ERROR;
            break;
    }

    if (di == driver_interface::State::ERR) {
        new_state = ERR_STARTUP_DI;  // what is this for? can it be removed?
    }

    if (hvil_interrupt) {
        new_state = SHUTDOWN;
    }

    on_enter = new_state != state;
    if (on_enter) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

}  // namespace governor
