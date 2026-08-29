/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#include "fan_controller.hpp"

#include <array>

#include "etl/algorithm.h"
#include "lookup_table.hpp"
#include "periph/pwm.hpp"

FanController::FanController(macfe::periph::PWMOutput& pwm) : pwm_(pwm) {}

void FanController::Start(float initial_power) {
    pwm_.Start();
    pwm_.SetDutyCycle(100.f - initial_power);
}

void FanController::Update(float temperature, float update_period_ms) {
    float desired_power =
        macfe::LookupTable::Evaluate(temp_to_power_, temperature);

    // convert pwm = 100 - power since the fan runs on inverse logic
    // ex. pwm=20% => fan is running at 80%
    float desired_pwm = 100.f - desired_power;

    float current_pwm = pwm_.GetDutyCycle();
    float delta_pwm = desired_pwm - current_pwm;

    float elapsed_sec = update_period_ms / 1000.0f;
    float pwm_step = etl::clamp<float>(delta_pwm, -pwm_roc_ * elapsed_sec,
                                       pwm_roc_ * elapsed_sec);

    pwm_.SetDutyCycle(current_pwm + pwm_step);
}
