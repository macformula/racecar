

#include <chrono>
#include <csignal>
#include <cstdint>
#include <thread>

#include "etl/array.h"
#include "fan_controller.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "generated/githash.hpp"
#include "sim_bridge.hpp"
#include "temp_sensor.hpp"

static const size_t STACK_SIZE_WORDS = 2048 * 16;
static const uint32_t PRIORITY_10HZ = 2;
static const uint32_t PRIORITY_1HZ = 1;

// bindings for tms-sil
tms_TmsToSil outputs = tms_TmsToSil_init_zero;
tms_SilToTms inputs = tms_SilToTms_init_zero;

std::thread wt;
std::thread rt;

namespace {
volatile std::sig_atomic_t g_running = 1;

void SignalHandler(int) {
    g_running = 0;
}
}  // namespace

using namespace generated::can;
using namespace std::chrono_literals;  // enables 100ms, 1s suffixes

constexpr int kSensorCount = temp_sensors.size();
static_assert(kSensorCount > 0);

void SoftwareReset() {}

void Task1Hz(generated::can::VehBus& veh_bus) {
    using namespace std::chrono_literals;
    auto next_wake = std::chrono::steady_clock::now();

    while (g_running) {
        next_wake += 1000ms;

        macfe::tms::Process1HzStep(veh_bus);
        std::this_thread::sleep_until(next_wake);
    }
}

void Task10Hz(macfe::tms::TmsContext& tms_ctx, TmsSilBridge& sim_bridge) {
    using namespace std::chrono_literals;
    auto next_wake = std::chrono::steady_clock::now();

    while (g_running) {
        next_wake += 100ms;

        sim_bridge.PollRx();
        macfe::tms::Process10HzStep(tms_ctx, 100.0f);
        sim_bridge.SendTx();

        std::this_thread::sleep_until(next_wake);
    }
}

int main(void) {
    std::signal(SIGINT, SignalHandler);
    TmsSilBridge sim_bridge;

    if (!sim_bridge.Start("127.0.0.1", 11002)) {
        std::cerr << "Failed to connect to sim server, running standalone.\n";
    }

    mcal::periph::AnalogInput* adc_ptrs[6] = {
        &sim_bridge.temp_adc[0], &sim_bridge.temp_adc[1],
        &sim_bridge.temp_adc[2], &sim_bridge.temp_adc[3],
        &sim_bridge.temp_adc[4], &sim_bridge.temp_adc[5]};

    etl::array temp_sensors{
        TempSensor{adc_ptrs[0]}, TempSensor{adc_ptrs[1]},
        TempSensor{adc_ptrs[2]}, TempSensor{adc_ptrs[3]},
        TempSensor{adc_ptrs[4]}, TempSensor{adc_ptrs[5]},
    };

    FanController fan_controller{sim_bridge.fan_pwm};

    mcal::sil::CanBase can_driver{"vcan0"};
    generated::can::VehBus veh_can_bus{can_driver};

    macfe::tms::TmsContext tms_ctx(adc_ptrs, temp_sensors, fan_controller,
                                   veh_can_bus);

    std::cout << "TMS-SIL - Starting 10Hz and 1Hz Threads ... \n";

    std::thread thread_10hz(Task10Hz, std::ref(tms_ctx), std::ref(sim_bridge));
    std::thread thread_1hz(Task1Hz, std::ref(veh_can_bus));

    if (thread_10hz.joinable()) thread_10hz.join();
    if (thread_1hz.joinable()) thread_1hz.join();

    sim_bridge.Stop();
    return 0;
}
