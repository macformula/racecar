/// @author Blake Freer
/// @date 2024-02-27

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

#include "app.h"
#include "shared/periph/adc.h"
#include "shared/periph/can.h"
#include "shared/periph/gpio.h"
#include "shared/util/mappers/linear_map.h"
#include "shared/util/mappers/mapper.h"
#include "shared/util/moving_average.h"

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
 * @note (SAM): Do not edit this struct, even though it will be very similar to
 * the raw CAN message. Just copy over the CAN message fields to this struct in
 * AMKMotor::UpdateInputs, then these values will be passed to the simulink
 * input in main.
 */
struct AMKInput {
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

struct AMKOutput {
    uint8_t bInverterOn_tx;
    uint8_t bDcOn_tx;
    uint8_t bEnable;
    uint8_t bErrorReset;
    float TargetVelocity;
    float TorqueLimitPositiv;
    float TorqueLimitNegativ;
};

class AMKMotor {
public:
    AMKMotor(shared::periph::CanBase& can_base) : can_base_(can_base) {}

    /**
     * @brief NOT IMPLEMENTED
     * @todo (SAM) construct and send a CAN message from the simulink output
     * values in `output`
     */
    void Transmit(AMKOutput output) {
        
    }

    /**
     * @brief Read the motor
     *
     * @param input
     */
    AMKInput UpdateInputs() {
        /* (SAM) Fill in this section
        // You will need to add an instance variable specifying which motor
        // this is, so that you can properly
        auto amk_can_input = CAN.GetAMKData(this_amk_number_);

        */
        return AMKInput{
            /* (SAM) populate from the CAN message
            .bReserve = amk_can_input.reserve, // sample
            .bSystemReady = amk_can_input.system_ready, // sample
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

private:
    shared::periph::CanBase& can_base_;
};

struct ContactorInput {
    bool Pack_Precharge_Feedback;
    bool Pack_Negative_Feedback;
    bool Pack_Positive_Feedback;
};

struct ContactorOutput {
    double prechargeContactorCMD;
    double HVposContactorCMD;
    double HVnegContactorCMD;
};

class Contactors {
public:
    Contactors(shared::periph::CanBase& can_base) : can_base_(can_base) {}

    ContactorInput ReadInput() {
        /* (SAM) read from CAN and fill this struct */

        // auto msg = can_base_.Read(...);

        return ContactorInput{
            // (SAM) populate from `msg`
            .Pack_Precharge_Feedback = 0,
            .Pack_Negative_Feedback = 0,
            .Pack_Positive_Feedback = 0,
        };
    }

    void Transmit(ContactorOutput output) {
        /* (SAM) Send a CAN message using these simulink outputs */
        // can_base.Send(...);
    }

private:
    shared::periph::CanBase& can_base_;
};

class StatusLight {
public:
    StatusLight(shared::periph::DigitalOutput& digital_output)
        : digital_output_(digital_output) {}

    void Toggle() {
        digital_output_.Set(next_value_);
        next_value_ = !next_value_;
    }

private:
    bool next_value_ = true;
    shared::periph::DigitalOutput& digital_output_;
};