/// @author Blake Freer
/// @date 2023-10-17

#ifndef INFRA_INPUT_ADC_INPUT_H_
#define INFRA_INPUT_ADC_INPUT_H_

#include "input/input_base.h"

namespace infra {
namespace input {

/// @brief Analog-Digital Conversion Input. Capture started by a pin
/// @tparam ReturnType
template <typename ReturnType>
class ADCInput : public InputBase<ReturnType> {
private:
    port::PortPin pin_;

    bool waiting_;
    bool newValueReady_;
    ReturnType value_;

public:
    ADCInput(port::PortPin pin)
        : pin_(pin), value_(0), waiting_(false), newValueReady_(false) {}

    /// @brief Get the most recent ADC value
    /// @note 0 is returned if no conversions have been performed
    ReturnType Read() noexcept {
        newValueReady_ = false;
        return value_;
    }

    /// @brief Start an ADC conversion by sending pin high
    void start() {
        pin_.SetDirectionOutput();
        pin_.SetHigh();
        waiting_ = true;
    }

    /// @brief Call in the ADC complete ISR to update the object's value and
    /// flags.
    friend void isr_value_callback(ADCInput& obj, ReturnType value);
};

template <typename ReturnType>
void isr_value_callback(ADCInput<ReturnType>& obj, ReturnType value) {
    obj->value_ = value;
    obj->waiting_ = false;
    obj->newValueReady_ = true;
}

}  // namespace input
}  // namespace mcal

#endif