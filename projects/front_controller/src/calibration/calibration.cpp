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

// ---------- Helper Functions ----------

// Map internal FSM state to CAN message State field
static TxCalibrationStatus::State_t GetCANState(State current_state) {
    using enum State;

    switch (current_state) {
        case IDLE:
            return TxCalibrationStatus::State_t::INIT;
        case CALIBRATE_APPS_MIN:
        case CALIBRATE_APPS_MAX:
            return TxCalibrationStatus::State_t::APPS;
        case CALIBRATE_BPPS_MIN:
        case CALIBRATE_BPPS_MAX:
            return TxCalibrationStatus::State_t::BPPS;
        case CALIBRATE_STEERING_CENTER:
        case CALIBRATE_STEERING_RANGE:
        case CALIBRATE_IMU:
            return TxCalibrationStatus::State_t::STEERING;
        case COMPLETE:
            return TxCalibrationStatus::State_t::COMPLETE;
        case ERROR:
            return TxCalibrationStatus::State_t::ERROR;
        default:
            return TxCalibrationStatus::State_t::INIT;
    }
}

// Get instruction code for current state
static TxCalibrationStatus::InstructionCode_t GetInstructionCode(
    State current_state) {
    using enum State;

    switch (current_state) {
        case IDLE:
            return TxCalibrationStatus::InstructionCode_t::IDLE;
        case CALIBRATE_APPS_MIN:
            return TxCalibrationStatus::InstructionCode_t::RELEASE_ACCEL;
        case CALIBRATE_APPS_MAX:
            return TxCalibrationStatus::InstructionCode_t::PRESS_ACCEL;
        case CALIBRATE_BPPS_MIN:
            return TxCalibrationStatus::InstructionCode_t::RELEASE_BRAKE;
        case CALIBRATE_BPPS_MAX:
            return TxCalibrationStatus::InstructionCode_t::PRESS_BRAKE;
        case CALIBRATE_STEERING_CENTER:
            return TxCalibrationStatus::InstructionCode_t::CENTER_STEERING;
        case CALIBRATE_STEERING_RANGE:
            return TxCalibrationStatus::InstructionCode_t::TURN_STEERING;
        case CALIBRATE_IMU:
            return TxCalibrationStatus::InstructionCode_t::KEEP_STATIONARY;
        case COMPLETE:
            return TxCalibrationStatus::InstructionCode_t::COMPLETE;
        case ERROR:
            return TxCalibrationStatus::InstructionCode_t::ERROR;
        default:
            return TxCalibrationStatus::InstructionCode_t::IDLE;
    }
}

// Calculate progress percentage
static uint8_t GetProgressPercent(State current_state) {
    using enum State;

    switch (current_state) {
        case IDLE:
            return 0;
        case CALIBRATE_APPS_MIN:
            return 5;
        case CALIBRATE_APPS_MAX:
            return 20;
        case CALIBRATE_BPPS_MIN:
            return 35;
        case CALIBRATE_BPPS_MAX:
            return 50;
        case CALIBRATE_STEERING_CENTER:
            return 65;
        case CALIBRATE_STEERING_RANGE:
            return 80;
        case CALIBRATE_IMU:
            return 90;
        case COMPLETE:
            return 100;
        case ERROR:
            return 0;
        default:
            return 0;
    }
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
    // Send calibration status to dashboard
    static uint8_t counter = 0;
    veh_can.Send(TxCalibrationStatus{
        .counter = counter++,
        .state = GetCANState(state),
        .progress_percent = GetProgressPercent(state),
        .instruction_code = GetInstructionCode(state),
    });
}

}  // namespace calibration