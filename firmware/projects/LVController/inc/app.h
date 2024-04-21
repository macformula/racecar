/// @author Blake Freer
/// @date 2023-12-25

#include <cmath>

#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"
#include "shared/util/mappers/mapper.h"

/**
 * @brief A Subsytem which can be enabled / disabled.
 */
class Subsystem {
public:
    /**
     * @brief Construct a new Subsystem object
     *
     * @param enable_output Digital Output which enables the subsystem.
     * @param invert_output Set to true for inverse logic (output low =
     * enabled).
     */
    Subsystem(shared::periph::DigitalOutput& enable_output, bool invert_output)
        : enable_output_(enable_output), high_is_enable_(!invert_output) {}

    inline void Enable() const {
        enable_output_.Set(high_is_enable_);
    }

    inline void Disable() const {
        enable_output_.Set(!high_is_enable_);
    }

private:
    shared::periph::DigitalOutput& enable_output_;
    bool high_is_enable_;
};

class Indicator {
public:
    Indicator(shared::periph::DigitalOutput& output, bool invert_output)
        : output_(output), high_is_on_(!invert_output) {}

    inline void TurnOn() {
        output_.Set(high_is_on_);
    }

    inline void TurnOff() {
        output_.Set(!high_is_on_);
    }

private:
    shared::periph::DigitalOutput& output_;
    bool high_is_on_;
};

class Fan : public Subsystem {
public:
    Fan(shared::periph::DigitalOutput& enable_output, bool invert_enable,
        shared::periph::PWMOutput& pwm_output,
        shared::util::Mapper<float>& power_to_duty)
        : Subsystem(enable_output, invert_enable),
          pwm_output_(pwm_output),
          power_to_duty_(power_to_duty) {}

    void Enable() {
        SetPower(0.0f);
        Subsystem::Enable();
    }

    void Disable() {
        SetPower(0.0f);
        Subsystem::Disable();
    }

    void SetTargetPower(float power) {
        target_duty_ = power_to_duty_.Evaluate(power);
    }

    void Update() const {
        float current_duty = pwm_output_.GetDutyCycle();
        float duty_step = shared::util::Clamper<float>::Evaluate(
            target_duty_ - current_duty, -duty_step_size_, +duty_step_size_);
        pwm_output_.SetDutyCycle(current_duty + duty_step);
    }

    bool IsAtTarget() {
        float error = std::abs(pwm_output_.GetDutyCycle() - target_duty_);
        return error < 0.1f;
    }

private:
    shared::periph::PWMOutput& pwm_output_;
    shared::util::Mapper<float>& power_to_duty_;
    float target_duty_ = 0.0f;
    float duty_step_size_ = 1.0f;

    void SetPower(float power) {
        pwm_output_.SetDutyCycle(power_to_duty_.Evaluate(power));
    }
};

class Status {
public:
    Status(shared::periph::DigitalInput& input, bool valid_state)
        : input_(input), valid_state_(valid_state) {}

    bool IsValid() {
        return input_.Read() == valid_state_;
    }

private:
    shared::periph::DigitalInput& input_;
    bool valid_state_;
};

class DCDC : public Subsystem {
public:
    DCDC(shared::periph::DigitalOutput& enable_output, bool invert_enable,
         shared::periph::DigitalInput& valid_input, bool invert_valid)
        : Subsystem(enable_output, invert_enable),
          valid_input_(valid_input),
          valid_state_(!invert_valid){};

    bool IsValid() const {
        return valid_input_.Read() == valid_state_;
    }

private:
    shared::periph::DigitalInput& valid_input_;
    bool valid_state_;
};