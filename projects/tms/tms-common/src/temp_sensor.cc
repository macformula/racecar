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

    float temperature =
        macfe::LookupTable::Evaluate(volt_ts_to_degC, volt_at_stm);

    return temperature;
}
