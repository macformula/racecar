/// @author Aiden
/// @date 2025-12-20

#include "calibration.hpp"
#include "sensors/driver/driver.hpp"
#include "sensors/dynamics/dynamics.hpp"
#include "thresholds.hpp"

namespace calibration {

using namespace generated::can;

// ---------- Static Variables ----------

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

    // BPPS calibration
    float bpps_min = 0.0f;
    float bpps_max = 0.0f;

    // Steering calibration
    float steering_center = 0.0f;
    float steering_left_max = 0.0f;
    float steering_right_max = 0.0f;

    // IMU calibration offsets
    float gyro_x_offset = 0.0f;
    float gyro_y_offset = 0.0f;
    float gyro_z_offset = 0.0f;
    float accel_x_offset = 0.0f;
    float accel_y_offset = 0.0f;
    float accel_z_offset = 0.0f;
};

static CalibrationData calib_data;

// ---------- Constants ----------
namespace timeout {
constexpr uint32_t STABILIZATION_TIME = 2000;  // 2 seconds in ms
constexpr uint32_t MEASUREMENT_TIME = 1000;    // 1 second averaging period
constexpr uint32_t STEP_TIMEOUT = 30000;       // 30 second timeout per step
}  // namespace timeout

// ---------- Modifiers ----------

void SetCommand(Command cmd) {
    command = cmd;
}

// ---------- Accessors ----------

State GetState(void) {
    return state;
}

bool IsCalibrating(void) {
    return state != State::IDLE && state != State::COMPLETE &&
           state != State::ERROR;
}

// ---------- Helper Functions ----------

// Average sensor readings over a period
float AverageSensorReading(float current_avg, float new_sample,
                           uint32_t sample_count) {
    return (current_avg * sample_count + new_sample) / (sample_count + 1);
}

// Check if sensor value is stable (not changing much)
bool IsSensorStable(float current, float previous, float threshold = 0.01f) {
    return std::abs(current - previous) < threshold;
}

// ---------- State Machine ----------

static void StateMachine_100Hz(VehBus& veh_can) {
    using enum State;

    State new_state = state;
    static float measurement_accumulator = 0.0f;
    static uint32_t sample_count = 0;
    static float previous_value = 0.0f;

    // Timeout check for all calibration steps
    if (IsCalibrating() && elapsed > timeout::STEP_TIMEOUT) {
        new_state = ERROR;
    }

    switch (state) {
        case IDLE:
            if (command == Command::START) {
                new_state = CALIBRATE_APPS_MIN;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
            break;

        case CALIBRATE_APPS_MIN: {
            // Wait for driver to release pedal
            float apps1 = sensors::driver::GetAccelPercent1();
            float apps2 = sensors::driver::GetAccelPercent2();

            if (elapsed < timeout::STABILIZATION_TIME) {
                // Give user time to release pedal
                previous_value = apps1;
            } else if (elapsed < timeout::STABILIZATION_TIME +
                                     timeout::MEASUREMENT_TIME) {
                // Average readings
                measurement_accumulator = AverageSensorReading(
                    measurement_accumulator, apps1, sample_count);
                sample_count++;
            } else {
                // Store calibrated minimum
                calib_data.apps1_min = measurement_accumulator / sample_count;
                calib_data.apps2_min = apps2;  // Could also
                average this new_state = CALIBRATE_APPS_MAX;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
        } break;

        case CALIBRATE_APPS_MAX: {
            // Wait for driver to fully press pedal
            float apps1 = sensors::driver::GetAccelPercent1();
            float apps2 = sensors::driver::GetAccelPercent2();

            if (elapsed < timeout::STABILIZATION_TIME) {
                // Give user time to press pedal
            } else if (elapsed < timeout::STABILIZATION_TIME +
                                     timeout::MEASUREMENT_TIME) {
                // Average readings
                measurement_accumulator = AverageSensorReading(
                    measurement_accumulator, apps1, sample_count);
                sample_count++;
            } else {
                calib_data.apps1_max = measurement_accumulator / sample_count;
                calib_data.apps2_max = apps2;
                new_state = CALIBRATE_BPPS_MIN;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
        } break;

        case CALIBRATE_BPPS_MIN: {
            // Wait for driver to release brake
            float bpps = sensors::driver::GetBrakePercent();

            if (elapsed < timeout::STABILIZATION_TIME) {
                // Give user time to release brake
            } else if (elapsed < timeout::STABILIZATION_TIME +
                                     timeout::MEASUREMENT_TIME) {
                measurement_accumulator = AverageSensorReading(
                    measurement_accumulator, bpps, sample_count);
                sample_count++;
            } else {
                calib_data.bpps_min = measurement_accumulator / sample_count;
                new_state = CALIBRATE_BPPS_MAX;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
        } break;

        case CALIBRATE_BPPS_MAX: {
            // Wait for driver to fully press brake
            float bpps = sensors::driver::GetBrakePercent();

            if (elapsed < timeout::STABILIZATION_TIME) {
                // Give user time to press brake
            } else if (elapsed < timeout::STABILIZATION_TIME +
                                     timeout::MEASUREMENT_TIME) {
                measurement_accumulator = AverageSensorReading(
                    measurement_accumulator, bpps, sample_count);
                sample_count++;
            } else {
                calib_data.bpps_max = measurement_accumulator / sample_count;
                new_state = CALIBRATE_STEERING_CENTER;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
        } break;

        case CALIBRATE_STEERING_CENTER: {
            // Wait for driver to center steering wheel
            float steering = sensors::driver::GetSteeringAngleDeg();

            if (elapsed < timeout::STABILIZATION_TIME) {
                // Give user time to center wheel
            } else if (elapsed < timeout::STABILIZATION_TIME +
                                     timeout::MEASUREMENT_TIME) {
                measurement_accumulator = AverageSensorReading(
                    measurement_accumulator, steering, sample_count);
                sample_count++;
            } else {
                calib_data.steering_center =
                    measurement_accumulator / sample_count;
                new_state = CALIBRATE_STEERING_RANGE;
                measurement_accumulator = 0.0f;
                sample_count = 0;
            }
        } break;

        case CALIBRATE_STEERING_RANGE: {
            // Continuously track min/max while driver turns wheel
            float steering = sensors::driver::GetSteeringAngleDeg();
            float centered = steering - calib_data.steering_center;

            // Update left max (negative values)
            if (centered < calib_data.steering_left_max) {
                calib_data.steering_left_max = centered;
            }

            // Update right max (positive values)
            if (centered > calib_data.steering_right_max) {
                calib_data.steering_right_max = centered;
            }

            // Complete after timeout (user has turned wheel fully both ways)
            if (elapsed > timeout::MEASUREMENT_TIME * 3) {
                new_state = CALIBRATE_IMU;
            }
        } break;

        case CALIBRATE_IMU: {
            // Vehicle must be stationary
            // Average gyro and accelerometer readings to find zero offsets
            // (This is simplified - real IMU calibration is more complex)

            if (elapsed < timeout::STABILIZATION_TIME) {
                // Wait for vehicle to be completely still
            } else if (elapsed < timeout::STABILIZATION_TIME +
                                     timeout::MEASUREMENT_TIME) {
                // Average IMU readings (gyro should be ~0, accel_z should be
                // ~1g)
                // TODO: Add actual IMU reading functions when available
                sample_count++;
            } else {
                // Store offsets
                calib_data.gyro_x_offset = 0.0f;  // measurement_accumulator
                /
                    // sample_count;
                    // ... store other offsets
                    new_state = COMPLETE;
            }
        } break;

        case COMPLETE:
            // Calibration finished - data stored in calib_data
            // TODO: Write to non-volatile storage (EEPROM/Flash)
            if (command == Command::STOP) {
                new_state = IDLE;
            }
            break;

        case ERROR:
            // Calibration failed (timeout, sensor error, etc.)
            if (command == Command::STOP) {
                new_state = IDLE;
            }
            break;
    }

    // Handle state transitions
    if (new_state != state) {
        state = new_state;
        elapsed = 0;
        measurement_accumulator = 0.0f;
        sample_count = 0;
        previous_value = 0.0f;
    } else {
        elapsed += 10;  // 100Hz = 10ms per cycle
    }
}

// ---------- Public Interface ----------

void Init(void) {
    state = State::IDLE;
    command = Command::STOP;
    elapsed = 0;
}

void Update_100Hz(VehBus& veh_can) {
    StateMachine_100Hz(veh_can);

    // TODO: Send calibration status to dashboard
    // veh_can.Send(TxCalibrationStatus{
    //     .state = static_cast<uint8_t>(state),
    //     .progress = ...,
    // });
}

}  // namespace calibration