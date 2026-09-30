#include "tms-common.hpp"

#include "generated/can/veh_messages.hpp"

namespace macfe::tms {

using namespace ::generated::can;

::generated::can::TxTMSValues TMSBroadcast(
    const std::array<float, kSensorCount>& temperatures) {
    return TxTMSValues{
        .val1 = static_cast<uint8_t>(temperatures[0] * 50.0f),
        .val2 = static_cast<uint8_t>(temperatures[1] * 50.0f),
        .val3 = static_cast<uint8_t>(temperatures[2] * 50.0f),
        .val4 = static_cast<uint8_t>(temperatures[3] * 50.0f),
        .val5 = static_cast<uint8_t>(temperatures[4] * 50.0f),
        .val6 = static_cast<uint8_t>(temperatures[5] * 50.0f),
    };
}

::generated::can::TxBmsBroadcast PackBmsBroadcast(
    const std::array<float, kSensorCount>& temperatures) {
    // Compute the min, max, and avg temperatures
    uint8_t low_index = 0;
    uint8_t high_index = 0;
    int8_t low_temp = temperatures[0];
    int8_t high_temp = temperatures[0];

    float temperature_sum = temperatures[0];
    for (uint8_t i = 1; i < temperatures.size(); i++) {
        temperature_sum += temperatures[i];

        auto t = static_cast<int8_t>(temperatures[i]);
        if (t < low_temp) {
            low_temp = t;
            low_index = i;
        }
        if (t > high_temp) {
            high_temp = t;
            high_index = i;
        }
    }
    int8_t avg_temp = static_cast<int8_t>(temperature_sum / kSensorCount);

    // This is a constant defined by Orion. It was discovered by
    // decoding the CAN traffic coming from the Orion Thermal Expansion Pack.
    const uint8_t kBmsChecksumConstant = 0x41;
    const uint8_t kThermistorModuleNumber = 0;
    uint8_t checksum = kThermistorModuleNumber + low_temp + high_temp +
                       avg_temp + kSensorCount + high_index + low_index +
                       kBmsChecksumConstant;

    return TxBmsBroadcast{
        .therm_module_num = kThermistorModuleNumber,
        .low_therm_value = low_temp,
        .high_therm_value = high_temp,
        .avg_therm_value = avg_temp,
        .num_therm_en = kSensorCount,
        .high_therm_id = high_index,
        .low_therm_id = low_index,
        .checksum = checksum,
    };
}

/***************************************************************
    Program Logic
***************************************************************/
void ReadAndSendTemperatures(
    float update_period_ms, VehBus& veh_can_bus, FanController& fan_controller,
    etl::array<TempSensor, kSensorCount>& temp_sensors) {
    // Read the temperature sensors
    std::array<float, kSensorCount> temperatures;
    float avg_temp = 0;
    for (size_t i = 0; i < kSensorCount; i++) {
        temperatures[i] = temp_sensors[i].Update();
        avg_temp += temperatures[i] / kSensorCount;
    }

    // Send the temperatures to the BMS
    TxBmsBroadcast bms_broadcast = PackBmsBroadcast(temperatures);
    TxTMSValues tms = TMSBroadcast(temperatures);

    veh_can_bus.Send(bms_broadcast);
    veh_can_bus.Send(tms);

    // Adjust the fan speed based on the average temperature
    fan_controller.Update(avg_temp, update_period_ms);
}
}  // namespace macfe::tms
