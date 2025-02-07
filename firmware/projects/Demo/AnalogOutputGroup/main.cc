#include "bindings.hpp"

int main() {
    bindings::Init();

    // Initialize variables
    constexpr float kMinVoltage = 0.0F;
    constexpr float kMaxVoltage = 3.3F;
    constexpr float kVoltageStep = 0.1F;

    // Track which analog output we're adjusting
    uint8_t current_output_index = 0;

    while (true) {
        /* Update LED position indicators to show current output index
         *
         * LED Pattern |  Output Index
         * ------------+----------------
         *     0b01    | analog_output0
         *     0b10    | analog_output1
         *     0b11    | analog_output2
         */
        bindings::led_position0.Set(((current_output_index + 1) & 0b01) != 0);
        bindings::led_position1.Set(((current_output_index + 1) & 0b10) != 0);

        // Check increment button
        if (bindings::button_increment.Read()) {
            float current_voltage =
                bindings::analog_output_group.GetVoltageSetpoint(
                    current_output_index);

            float new_voltage = current_voltage;

            if ((current_voltage + kVoltageStep) < kMaxVoltage) {
                new_voltage += kVoltageStep;
            }

            bindings::analog_output_group.SetVoltage(current_output_index,
                                                     new_voltage);
        }

        // Check decrement button
        if (bindings::button_decrement.Read()) {
            float current_voltage =
                bindings::analog_output_group.GetVoltageSetpoint(
                    current_output_index);

            float new_voltage = current_voltage;

            if ((current_voltage - kVoltageStep) > kMinVoltage) {
                new_voltage -= kVoltageStep;
            }

            bindings::analog_output_group.SetVoltage(current_output_index,
                                                     new_voltage);
        }

        // Check confirm button
        if (bindings::button_confirm.Read()) {
            // Move to next output
            current_output_index =
                (current_output_index + 1) % bindings::kNumAnalogOutputs;

            // If we've wrapped around, update all outputs simultaneously
            if (current_output_index == 0) {
                bindings::analog_output_group.LoadVoltages();
            }
        }

        // Update PWM to show new current output's value
        float current_voltage =
            bindings::analog_output_group.GetVoltageSetpoint(
                current_output_index);

        float duty_cycle = current_voltage / kMaxVoltage;
        float current_duty_cycle = bindings::pwm_output0.GetDutyCycle();

        if (current_duty_cycle != duty_cycle) {
            bindings::pwm_output0.SetDutyCycle(duty_cycle);
        }

        bindings::DelayMs(10);  // Longer debounce for confirm
    }
}