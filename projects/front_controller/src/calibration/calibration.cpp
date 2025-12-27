#include "calibration.hpp"

#include "sensors/driver/driver.hpp"

namespace calibration {

using namespace generated::can;
//---static variables---
static State state = State::IDLE;
static Command command = Command::STOP;
static uint32_t elapsed = 0;

// Calibration data storage
struct CalibrationData {
    // APPS calibration
    float apps1_min = 0.0f;
    float apps1_max = 0.0f;
    float apps2_min = 0.0f;
    float apps2_max = 0.0f;
};

//---modifiers---
void SetCommand(Command cmd) {
    command = cmd;
}

State GetState(void) {
    return state;
}

bool IsCalibrating(void) {
    return state != State::IDLE && state != State::COMPLETE &&
           state != State::ERROR;
}

static void UpdateStateMachine(VehBus& veh_can) {
    using enum State;
    State new_state = state;
    static float measurement_accumulator = 0.0f;  // running sum
    static uint32_t sample_count = 0;
    static float previous_value = 0.0f;  // last reading for stability check

    switch (state) {
        case IDLE: {
            if (command == Command::START) {
                new_state = CALIBRATE_APPS_MIN;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
        } break;

        case CALIBRATE_APPS_MIN: {
            // send message to dashboard to tell use to release pedal
            float apps1 = sensors::driver::GetAccelPercent1();
            float apps2 = sensors::driver::GetAccelPercent2();
        }
    }
}

void Update_100Hz(VehBus& veh_can) {
    UpdateStateMachine(veh_can);
}

}  // namespace calibration