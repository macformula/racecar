/// @author Blake Freer
/// @date 2024-03-06

#include <cmath>

#include "shared/periph/gpio.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/mapper.hpp"

/// @brief A Subsystem which can be enabled / disabled.
class Subsystem {
public:
    // Use shared::periph::InverseDigitalOutput if the subsystem is enabled by
    // a low signal.
    Subsystem(shared::periph::DigitalOutput& enable_output)
        : enable_output_(enable_output) {}

    inline virtual void Enable() const {
        enable_output_.SetHigh();
    }

    inline virtual void Disable() const {
        enable_output_.SetLow();
    }

private:
    shared::periph::DigitalOutput& enable_output_;
};

class Fan : public Subsystem {
public:
    Fan(shared::periph::DigitalOutput& enable_output,
        shared::periph::PWMOutput& pwm_output,
        shared::util::Mapper<float>& power_to_duty)
        : Subsystem(enable_output),
          pwm_output_(pwm_output),
          power_to_duty_(power_to_duty) {}

    void Enable() const override {
        SetPower(0.0f);
        Subsystem::Enable();
    }

    void Disable() const override {
        SetPower(0.0f);
        Subsystem::Disable();
    }

    void SetTargetPower(float power, float max_duty_per_second) {
        target_duty_ = power_to_duty_.Evaluate(power);
        duty_per_second_ = max_duty_per_second;
    }

    /// @brief Set the Power Now.
    /// @warning THIS IS DANGEROUS as large jumps can caused significant current
    /// rushes. Prefer SetTargetPower() and Update()
    void Dangerous_SetPowerNow(float power) {
        SetPower(power);
    }

    /// @brief Update the duty cycle towards the target duty cycle. The argument
    /// is the elapsed time between calls which is required to adjust the duty
    /// cycle by the rate specified in SetTargetPower.
    void Update(float interval_sec) const {
        // max_duty_step must be greater than any platform's PWM duty cycle
        // resolution
        float max_duty_step = duty_per_second_ * interval_sec;

        float current_duty = pwm_output_.GetDutyCycle();
        float duty_step = shared::util::Clamper<float>::Evaluate(
            target_duty_ - current_duty, -max_duty_step, +max_duty_step);

        pwm_output_.SetDutyCycle(current_duty + duty_step);
    }

    bool IsAtTarget() {
        constexpr float kDutyEqualTolerance = 0.1f;
        float error = std::abs(pwm_output_.GetDutyCycle() - target_duty_);
        return error < kDutyEqualTolerance;
    }

private:
    shared::periph::PWMOutput& pwm_output_;
    shared::util::Mapper<float>& power_to_duty_;
    float target_duty_ = 0.0f;
    float duty_per_second_ = 0.0f;

    void SetPower(float power) const {
        pwm_output_.SetDutyCycle(power_to_duty_.Evaluate(power));
    }
};

class DCDC : public Subsystem {
public:
    DCDC(shared::periph::DigitalOutput& enable_output,
         shared::periph::DigitalInput& valid_input,
         shared::periph::DigitalOutput& led_output)
        : Subsystem(enable_output),
          valid_input_(valid_input),
          led_(led_output) {};

    bool CheckValid() {
        bool is_valid = valid_input_.Read();
        led_.Set(is_valid);
        return is_valid;
    }

private:
    shared::periph::DigitalInput& valid_input_;
    shared::periph::DigitalOutput& led_;
};