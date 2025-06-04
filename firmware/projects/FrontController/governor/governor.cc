#include "governor.hpp"

#include "../enums.hpp"

namespace governor {

static AccCmd accumulator_cmd;
static MiCmd motor_cmd;
static DiCmd driver_cmd;
static GovSts state;

static uint32_t elapsed;
static bool on_enter;

static const uint32_t kMotorMaxAttempts = 5;
static uint32_t motor_start_count;  // this should be handled in motors/

AccCmd GetAccumulatorCmd(void) {
    return accumulator_cmd;
}

MiCmd GetMotorCmd(void) {
    return motor_cmd;
}

DiCmd GetDriverInterfaceCmd(void) {
    return driver_cmd;
}

GovSts GetState(void) {
    return state;
}

void Init(void) {
    accumulator_cmd = AccCmd::OFF;
    motor_cmd = MiCmd::INIT;
    driver_cmd = DiCmd::INIT;
    state = GovSts::INIT;
    elapsed = 0;
    motor_start_count = 0;
}

void Update_100Hz(AccSts acc, MiSts mi, DiSts di) {
    using enum GovSts;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    GovSts new_state = state;

    switch (state) {
        case INIT:
            accumulator_cmd = AccCmd::OFF;
            motor_cmd = MiCmd::INIT;
            driver_cmd = DiCmd::INIT;
            motor_start_count = 0;

            if ((elapsed >= 2000) && (di == DiSts::REQUESTED_HV_START)) {
                new_state = STARTUP_HV;
            }
            break;

        case STARTUP_HV:
            accumulator_cmd = AccCmd::ENABLED;
            motor_cmd = MiCmd::INIT;
            driver_cmd = DiCmd::INIT;

            if (acc == AccSts::RUNNING) {
                new_state = STARTUP_READY_TO_DRIVE;
            }
            break;

        case STARTUP_READY_TO_DRIVE:
            accumulator_cmd = AccCmd::ENABLED;
            motor_cmd = MiCmd::INIT;
            driver_cmd = DiCmd::HV_IS_ON;

            if (di == DiSts::REQUESTED_MOTOR_START) {
                new_state = STARTUP_MOTOR;
            }
            break;

        case STARTUP_MOTOR:
            accumulator_cmd = AccCmd::ENABLED;
            motor_cmd = MiCmd::STARTUP;
            driver_cmd = DiCmd::HV_IS_ON;

            if (on_enter) {
                motor_start_count++;
            }

            if (mi == MiSts::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (mi == MiSts::ERR) {
                if (motor_start_count < kMotorMaxAttempts) {
                    new_state = ERR_STARTUP_MOTOR_RESET;
                } else {
                    new_state = ERR_STARTUP_MOTOR_FAULT;  // don't keep trying
                }
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            accumulator_cmd = AccCmd::ENABLED;
            motor_cmd = MiCmd::STARTUP;
            driver_cmd = DiCmd::READY_TO_DRIVE;

            if (di == DiSts::RUNNING) {
                new_state = RUNNING;
            }
            break;

        case RUNNING:
            accumulator_cmd = AccCmd::ENABLED;
            motor_cmd = MiCmd::STARTUP;
            driver_cmd = DiCmd::READY_TO_DRIVE;

            if (acc == AccSts::LOW_SOC) {
                new_state = SHUTDOWN;
            }

            if (acc == AccSts::ERR_RUNNING) {
                new_state = ERR_RUNNING_HV;
            }

            if (mi == MiSts::ERR) {
                new_state = ERR_RUNNING_MOTOR;
            }

            break;

        case SHUTDOWN:
            // can this be combined with ERR_STARTUP_HV?
            accumulator_cmd = AccCmd::OFF;
            motor_cmd = MiCmd::SHUTDOWN;
            driver_cmd = DiCmd::SHUTDOWN;

            if (acc == AccSts::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = INIT;
            }
            break;

        case ERR_STARTUP_HV:
            accumulator_cmd = AccCmd::OFF;
            motor_cmd = MiCmd::SHUTDOWN;
            driver_cmd = DiCmd::SHUTDOWN;

            if (acc == AccSts::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = INIT;
            }
            break;

        case ERR_STARTUP_MOTOR_RESET:
            accumulator_cmd = AccCmd::ENABLED;
            motor_cmd = MiCmd::ERR_RESET;
            driver_cmd = DiCmd::HV_IS_ON;  // is this correct?

            if (mi == MiSts::OFF) {
                new_state = STARTUP_MOTOR;
            }
            break;

        // The vehicle cannot escape these error states without rebooting.
        case ERR_STARTUP_MOTOR_FAULT:
        case ERR_STARTUP_DI:
        case ERR_RUNNING_HV:
        case ERR_RUNNING_MOTOR:
            accumulator_cmd = AccCmd::OFF;
            motor_cmd = MiCmd::SHUTDOWN;
            driver_cmd = DiCmd::RUN_ERROR;
            break;
    }

    if (di == DiSts::ERR) {
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
