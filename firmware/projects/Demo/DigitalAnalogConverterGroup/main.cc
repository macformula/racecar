#include "bindings.hpp"

int main() {
    bindings::Init();

    // Initialize variables
    constexpr float kMinVoltage = 0.0F;
    constexpr float kMaxVoltage = bindings::kVoltageReference;
    constexpr float kVoltageStep = 0.1F;

    // Track which analog output we're adjusting
    uint8_t current_output_index = 0;

    while (true) {
        /* Update LED position indicators to show current output index
         *
         * LED Pattern |  Output Index
         * ------------+----------------
         *     0b000    | analog_output0
         *     0b001    | analog_output1
         *     0b010    | analog_output2
         *     0b011    | analog_output3
         *     0b100    | analog_output4
         *     0b101    | analog_output5
         *     0b110    | analog_output6
         *     0b111    | analog_output7
         */
        bindings::led_position0.Set(((current_output_index + 1) & 0b001) != 0);
        bindings::led_position1.Set(((current_output_index + 1) & 0b010) != 0);
        bindings::led_position2.Set(((current_output_index + 1) & 0b100) != 0);

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

        bindings::DelayMs(10);  
    }
}