/// @author Blake Freer
/// @date 2024-03-06

#include <cmath>

#include "can_messages.h"
#include "shared/comms/can/can_bus.h"
#include "shared/os/mutex.h"
#include "shared/periph/gpio.h"
#include "shared/periph/pwm.h"
#include "shared/util/mappers/clamper.h"
#include "shared/util/mappers/mapper.h"

enum class LvControllerState {
    Unknown = 0,
    Startup,
    TsalEnabled,
    RaspiEnabled,
    FrontControllerEnabled,
    SpeedgoatEnabled,
    AccumulatorEnabled,
    MotorControllerPrechargeEnabled,
    MotorControllerEnabled,
    MotorControllerPrechargeDisabled,
    ImuGpsEnabled,
    ShutdownCircuitEnabled,
    WaitingForClosedContactors,
    WaitingForDcdcValid,
    DcdcValid,
    DcdcLedEnabled,
    PowertrainPumpEnabled,
    PowertrainFanEnabled,
    PowertrainFanSweeping,
    SequenceComplete,
    LostDcdcValid,
    DisabledLed,
    PowertrainPumpOff,
    PowertrainFanOff,
    PowertrainFanPwmOff,
    WaitingForOpenContactors,
    DcdcEnabled,
};

class StateBroadcaster {
public:
    StateBroadcaster(shared::can::CanBus& can_bus) : can_bus_(can_bus) {}

    void UpdateState(LvControllerState state) {
        generated::can::LvControllerStatus lv_status;
        lv_status.lv_controller_state = static_cast<uint8_t>(state);

        can_bus_.Send(lv_status);
    }

private:
    shared::can::CanBus can_bus_;
};

/**
 * @brief A Subsystem which can be enabled / disabled.
 */
class Subsystem {
public:
    /**
     * @brief Construct a new Subsystem object
     *
     * @param enable_output Digital Output which enables the subsystem.
     * enabled).
     */
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

class Indicator {
public:
    Indicator(shared::periph::DigitalOutput& output) : output_(output) {}

    inline void TurnOn() {
        output_.SetHigh();
    }

    inline void TurnOff() {
        output_.SetLow();
    }

    inline void SetState(bool value) {
        output_.Set(value);
    }

private:
    shared::periph::DigitalOutput& output_;
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

    /**
     * @brief Set the Power Now.
     * @warning THIS IS DANGEROUS as large jumps can caused significant current
     * rushes. Prefer SetTargetPower() and Update()
     *
     * @param power
     */
    void Dangerous_SetPowerNow(float power) {
        SetPower(power);
    }

    /**
     * @brief Update the duty cycle towards the target duty cycle. The argument
     * is the elapsed time between calls which is required to adjust the duty
     * cycle by the rate specified in SetTargetPower.
     *
     * @param interval_sec
     */
    void Update(float interval_sec) const {
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
          led_(led_output){};

    bool CheckValid() {
        bool is_valid = valid_input_.Read();
        led_.SetState(is_valid);
        return is_valid;
    }

private:
    shared::periph::DigitalInput& valid_input_;
    Indicator led_;
};