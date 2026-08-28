#include "tms_tasks.hpp"

#include "generated/can/veh_messages.hpp"
#include "generated/githash.hpp"

namespace macfe::tms {

using namespace generated::can;

namespace {

TxTMSValues TMSBroadcast(
    std::span<macfe::periph::AnalogInput* const, kSensorCount> adcs) {
    return TxTMSValues{
        .val1 = static_cast<uint8_t>(adcs[0]->ReadVoltage() * 50.0f),
        .val2 = static_cast<uint8_t>(adcs[1]->ReadVoltage() * 50.0f),
        .val3 = static_cast<uint8_t>(adcs[2]->ReadVoltage() * 50.0f),
        .val4 = static_cast<uint8_t>(adcs[3]->ReadVoltage() * 50.0f),
        .val5 = static_cast<uint8_t>(adcs[4]->ReadVoltage() * 50.0f),
        .val6 = static_cast<uint8_t>(adcs[5]->ReadVoltage() * 50.0f),
    };
}

TxBmsBroadcast PackBmsBroadcast(
    const std::array<float, kSensorCount>& temperatures) {
    uint8_t low_index = 0;
    uint8_t high_index = 0;
    int8_t low_temp = static_cast<int8_t>(temperatures[0]);
    int8_t high_temp = static_cast<int8_t>(temperatures[0]);

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

}  // namespace

void Process10HzStep(TmsContext& ctx, float dt_ms) {
    std::array<float, kSensorCount> temperatures;
    float avg_temp = 0.0f;

    for (size_t i = 0; i < kSensorCount; i++) {
        temperatures[i] = ctx.temp_sensors[i].Update();
        avg_temp += temperatures[i];
    }
    avg_temp /= static_cast<float>(kSensorCount);

    TxBmsBroadcast bms_broadcast = PackBmsBroadcast(temperatures);
    TxTMSValues tms = TMSBroadcast(ctx.adc_channels);

    ctx.veh_can_bus.Send(bms_broadcast);
    ctx.veh_can_bus.Send(tms);

    ctx.fan_controller.Update(avg_temp, dt_ms);
}

void Process1HzStep(generated::can::VehBus& veh_can_bus) {
    veh_can_bus.Send(generated::can::TxTmsGitHash{
        .commit = macfe::generated::GIT_HASH,
        .dirty = macfe::generated::GIT_DIRTY,
    });
}

}  // namespace macfe::tms