/// @author Blake Freer
/// @date 2023-11-18
/// @brief Functions and types that will be used in TMS main

#pragma once

#include <cstdint>

#include "../generated/can/veh_bus.hpp"
#include "../generated/can/veh_messages.hpp"
#include "shared/periph/adc.hpp"
#include "shared/periph/pwm.hpp"
#include "shared/util/mappers/clamper.hpp"
#include "shared/util/mappers/linear_map.hpp"
#include "shared/util/mappers/mapper.hpp"
#include "shared/util/moving_average.hpp"

/***************************************************************
    App-level objects
***************************************************************/

class TempSensor {
public:
    TempSensor(shared::periph::ADCInput& adc,
               const shared::util::Mapper<float>& volt_to_temp)
        : adc_(adc), volt_to_temp_(volt_to_temp), rolling_temperature_() {}

    float Update() {
        float new_temperature = Read();
        rolling_temperature_.LoadValue(new_temperature);
        return rolling_temperature_.GetValue();
    }

private:
    shared::periph::ADCInput& adc_;

    /// @brief Mapping from raw ADC value to temperature [degC]
    const shared::util::Mapper<float>& volt_to_temp_;

    static constexpr int moving_average_length_ = 20;
    shared::util::MovingAverage<float, moving_average_length_>
        rolling_temperature_;

    float Read() {
        uint32_t adc_value = adc_.Read();
        float volt = static_cast<float>(adc_value) * 3.3f / 4095.0f;
        float temperature = volt_to_temp_.Evaluate(volt);
        return temperature;
    }
};

class FanContoller {
public:
    FanContoller(shared::periph::PWMOutput& pwm,
                 shared::util::Mapper<float>& temp_to_power,
                 float pwm_step_size)
        : pwm_(pwm),
          temp_to_power_(temp_to_power),
          pwm_step_size_(pwm_step_size) {}

    void Start(float initial_power) {
        pwm_.Start();
        pwm_.SetDutyCycle(power_to_pwm_.Evaluate(initial_power));
    }

    void Update(float temperature) {
        // convert pwm = 100 - power since the fan runs on inverse logic
        // ex. pwm=20% => fan is running at 80%
        float desired_pwm =
            power_to_pwm_.Evaluate(temp_to_power_.Evaluate(temperature));
        float current_pwm = pwm_.GetDutyCycle();
        float delta_pwm = desired_pwm - current_pwm;

        float pwm_step = shared::util::Clamper<float>::Evaluate(
            delta_pwm, -pwm_step_size_, pwm_step_size_);

        pwm_.SetDutyCycle(current_pwm + pwm_step);
    }

private:
    shared::periph::PWMOutput& pwm_;

    /// @brief Mapping from temperature [degC] to fan power
    shared::util::Mapper<float>& temp_to_power_;

    /// @brief Fan pwm signal is inverted (high duty = low power)
    const shared::util::LinearMap<float> power_to_pwm_{-1.0f, 100.0f};

    /// @brief Largest allowable PWM per Update() call.
    /// @todo Express pwm_step_size in pwm/second and use Update() frequency
    /// to determine it.
    float pwm_step_size_;
};

class BmsBroadcaster {
public:
    BmsBroadcaster(generated::can::VehBus& can_bus, uint8_t num_thermistors)
        : can_bus_(can_bus), num_thermistors_(num_thermistors) {}

    void SendBmsBroadcast(uint8_t high_thermistor_id,
                          int8_t high_thermistor_value,
                          uint8_t low_thermistor_id,
                          int8_t low_thermistor_value,
                          int8_t avg_thermistor_value) {
        generated::can::TxBmsBroadcast bms_broadcast_{
            .therm_module_num = kThermistorModuleNumber,
            .low_therm_value = low_thermistor_value,
            .high_therm_value = high_thermistor_value,
            .avg_therm_value = avg_thermistor_value,
            .num_therm_en = num_thermistors_,
            .high_therm_id = high_thermistor_id,
            .low_therm_id = low_thermistor_id,
        };
        bms_broadcast_.checksum = CalculateBmsBroadcastChecksum(bms_broadcast_);

        can_bus_.Send(bms_broadcast_);
    }

private:
    static constexpr uint8_t kThermistorModuleNumber = 0;

    generated::can::VehBus& can_bus_;
    uint8_t num_thermistors_;

    uint8_t CalculateBmsBroadcastChecksum(
        const generated::can::TxBmsBroadcast& bms_broadcast) {
        // This is a constant defined by Orion. It was discovered by
        // decoding the CAN traffic coming from the Orion Thermal Expansion
        // Pack.
        constexpr uint8_t kChecksumConstant = 0x41;

        return static_cast<uint8_t>(
            bms_broadcast.high_therm_id + bms_broadcast.high_therm_value +
            bms_broadcast.low_therm_id + bms_broadcast.low_therm_value +
            bms_broadcast.avg_therm_value + bms_broadcast.num_therm_en +
            bms_broadcast.therm_module_num + kChecksumConstant);
    }
};
