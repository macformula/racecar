/// @author Blake Freer
/// @date 2024-02-27

#pragma once

#include <stddef.h>
#include <sys/_stdint.h>

#include <cstdint>
#include <memory>
#include <utility>

#include "app.h"
#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/linear_map.h"
#include "shared/util/mappers/mapper.h"
#include "shared/util/moving_average.h"
#include "simulink.h"

class AnalogInput {
    static constexpr size_t kMovingAverageLength = 20;

public:
    AnalogInput(shared::periph::ADCInput& adc,
                shared::util::Mapper<double, uint16_t>* adc_to_position)
        : adc_(adc), adc_to_position_(adc_to_position) {}

    double Update() {
        uint32_t position = adc_.Read();
        moving_average_.LoadValue(uint16_t(position));
        return GetPosition();
    }

    /**
     * @brief Get the position from the last `Update()` call.
     */
    inline double GetPosition() {
        return adc_to_position_->Evaluate(moving_average_.GetValue());
    }

private:
    shared::periph::ADCInput& adc_;
    shared::util::MovingAverage<uint16_t, kMovingAverageLength> moving_average_;
    const shared::util::Mapper<double, uint16_t>* adc_to_position_;
};

class Speaker {
public:
    Speaker(shared::periph::DigitalOutput& digital_output)
        : digital_output_(digital_output) {}

    void Update(bool state) const {
        digital_output_.Set(state);
    }

private:
    shared::periph::DigitalOutput& digital_output_;
};

class BrakeLight {
public:
    BrakeLight(shared::periph::DigitalOutput& digital_output)
        : digital_output_(digital_output) {}

    void Update(bool state) const {
        digital_output_.Set(state);
    }

private:
    shared::periph::DigitalOutput& digital_output_;
};

class Button {
public:
    Button(shared::periph::DigitalInput& digital_input)
        : digital_input_(digital_input) {}

    bool Read() const {
        return digital_input_.Read();
    }

private:
    shared::periph::DigitalInput& digital_input_;
};

/**
 * @brief Struct containing all data required for each simulink AMK input
 * @note Sam: Do not edit this struct, even though it will be very similar to
 * the raw CAN message. Just copy over the CAN message fields to this struct in
 * AMKMotor::UpdateInputs, then these values will be passed to the simulink
 * input in main.
 */
struct AMKMotorData {
    bool bReserve;
    bool bSystemReady;
    bool bError;
    bool bWarn;
    bool bQuitDcOn;
    bool bDcOn;
    bool bQuitInverterOn;
    bool bInverterOn;
    bool bDerating;
    int16_t ActualVelocity;
    int16_t TorqueCurrent;
    int16_t MagnetizingCurrent;
    int16_t TempMotor;
    int16_t TempInverter;
    uint16_t ErrorInfo;
    int16_t TempIGBT;
};

class AMKMotor {
public:
    AMKMotor() {}

    /**
     * @brief NOT IMPLEMENTED
     * @todo sam - change the argument to take some information from the
     * simulink output and construct and send a CAN message.
     */
    void Transmit(void* placeholder) {}

    /**
     * @brief Read the motor
     *
     * @param input
     */
    AMKMotorData UpdateInputs() {
        /* SAM Fill in this section
        // You will need to add an instance variable specifying which motor
        // this is, so that you can properly
        auto amk_can_input = CAN.GetAMKData(this_amk_number_);

        */
        return AMKMotorData{
            /* populate from the CAN message
            .bReserve = amk_can_input.reserve,
            .bSystemReady = amk_can_input.system_ready,
            .bError = 0,
            .bWarn = 0,
            .bQuitDcOn = 0,
            .bDcOn = 0,
            .bQuitInverterOn = 0,
            .bInverterOn = 0,
            .bDerating = 0,
            .ActualVelocity = 0,
            .TorqueCurrent = 0,
            .MagnetizingCurrent = 0,
            .TempMotor = 0,
            .TempInverter = 0,
            .ErrorInfo = 0,
            .TempIGBT = 0,
            */
        };
    }
};