/// @author Blake Freer
/// @date 2023-10-17

#ifndef MCAL_ADC_INPUT_HPP
#define MCAL_ADC_INPUT_HPP

#include "mcal_input/input_base.hpp"
#include "mcal_port/port_pin.hpp"

namespace mcal {
namespace input {

/// @brief Analog-Digital Conversion Input. Capture started by a pin
/// @tparam return_type 
template <typename return_type>
class ADCInput : public Input_Base<return_type> {

private:
    port::port_pin pin;

    bool waiting;
    bool newValueReady;
    return_type value;

public:
    ADCInput(port::port_pin _pin) : pin(_pin),
                                    value(0),
                                    waiting(false),
                                    newValueReady(false) {}

    /// @brief Get the most recent ADC value
	/// @note 0 is returned if no conversions have been performed
    /// @return return_type
    return_type read() noexcept {
        newValueReady = false;
        return value;
    }

	/// @brief Start an ADC conversion by sending pin high
	void start() {
		pin.set_direction_output();
		pin.set_high();
		waiting = true;
	}

	/// @brief Call in the ADC complete ISR to update the object's valuea and
	/// flags.
	friend void isr_value_callback(ADCInput& obj, return_type _value);
};

template <typename return_type>
void isr_value_callback(ADCInput<return_type>& obj, return_type _value) {
	obj->value = _value;
	obj->waiting = false;
	obj->newValueReady = true;
}

} // namespace input
} // namespace mcal

#endif