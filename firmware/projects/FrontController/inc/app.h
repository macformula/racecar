/// @author Blake Freer
/// @date 2024-02-27

#pragma once

#include <cstdint>

#include "shared/periph/adc.h"
#include "shared/periph/gpio.h"

class Pedal {
public:
    Pedal(shared::periph::ADCInput& adc) : adc_(adc) {}

    uint16_t Update() {
        /// @todo Map adc value to 0-100% range for standardization.
        position_ = uint16_t(adc_.Read());
        return GetPosition();
    }

    /**
     * @brief Get the position from the last `Update()` call.
     */
    inline uint16_t GetPosition() const {
        return position_;
    }

private:
    uint16_t position_ = 0;
    shared::periph::ADCInput& adc_;
};

class SteeringWheel {
public:
    SteeringWheel(shared::periph::ADCInput& adc) : adc_(adc) {}

    uint16_t Update() {
        /// @todo Map to range [-1, 1] or [-180 deg, +180 deg] etc
        position_ = uint16_t(adc_.Read());
        return GetPosition();
    }

    /**
     * @brief Get the position from the last `Update()` call.
     */
    inline uint16_t GetPosition() const {
        return position_;
    }

private:
    uint16_t position_ = 0;
    shared::periph::ADCInput& adc_;
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

class AMKMotor {
public:
    AMKMotor() {}

    /**
     * @brief NOT IMPLEMENTED
     * @todo sam - change the argument to take some information from the
     * simulink output and construct and send a CAN message.
     */
    void Transmit(void* placeholder) {}
};