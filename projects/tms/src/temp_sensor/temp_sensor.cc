#include "temp_sensor.hpp"

#include "lookup_table.hpp"
#include "moving_average.hpp"
#include "periph/analog_input.hpp"

TempSensor::TempSensor(macfe::periph::AnalogInput& analog_input)
    : analog_input_(analog_input), rolling_temperature_() {}

float TempSensor::Update() {
    float new_temperature = Read();
    rolling_temperature_.LoadValue(new_temperature);
    return rolling_temperature_.GetValue();
}

float TempSensor::Read() {
    float volt_at_stm = analog_input_.ReadVoltage();

    /// Calculate the voltage at the temperature sensor from the voltage
    /// at the STM. They are not equal because there is a non-unity gain
    /// buffer between them. V_STM = 1.44 + 0.836 * V_TS / 2 So the
    /// inverse is V_TS = 2 * (V_STM - 1.44) / 0.83
    float volt_at_tempsensor = 2 * (volt_at_stm - 1.44) / 0.836;
    float temperature =
        macfe::LookupTable::Evaluate(volt_ts_to_degC, volt_at_tempsensor);

    return temperature;
}
