/// @author Samuel Parent
/// @date 2024-05-01

#include <chrono>
#include <thread>

#include "mcal/raspi/periph/adc.hpp"
#include "mcal/raspi/periph/can.hpp"
#include "mcal/raspi/periph/gpio.hpp"
#include "shared/periph/adc.hpp"
#include "shared/periph/can.hpp"
#include "shared/periph/gpio.hpp"
#include "validation/sil/sil_client.h"

const std::string ecu_name = "FrontController";
const std::string server_addr = "localhost:31522";
const std::string io_can_iface = "vcan0";

namespace val {
using namespace sil;
SilClient sil_client(server_addr);

}  // namespace val

namespace mcal {
using namespace raspi::periph;

CanBase io_can_base{io_can_iface};

DigitalOutput debug_led_en{ecu_name, "DebugLedEn", val::sil_client};
DigitalOutput dashboard_en{ecu_name, "DashboardEn", val::sil_client};
DigitalOutput hvil_led_en{ecu_name, "HvilLedEn", val::sil_client};
DigitalOutput brake_light_en{ecu_name, "BrakeLightEn", val::sil_client};
DigitalOutput status_led_en{ecu_name, "StatusLedEn", val::sil_client};
DigitalOutput rtds_en{ecu_name, "RtdsEn", val::sil_client};
ADCInput accel_pedal_pos_1{ecu_name, "AccelPedalPosition1", val::sil_client};
ADCInput accel_pedal_pos_2{ecu_name, "AccelPedalPosition2", val::sil_client};
ADCInput brake_pedal_pos_1{ecu_name, "BrakePedalPosition1", val::sil_client};
ADCInput brake_pedal_pos_2{ecu_name, "BrakePedalPosition2", val::sil_client};
ADCInput suspension_travel_1{ecu_name, "SuspensionTravel1", val::sil_client};
ADCInput suspension_travel_2{ecu_name, "SuspensionTravel2", val::sil_client};
ADCInput hvil_feedback{ecu_name, "HvilFeedback", val::sil_client};
ADCInput steering_angle{ecu_name, "SteeringAngle", val::sil_client};
DigitalInput start_button_n{ecu_name, "StartButtonN", val::sil_client};
DigitalInput wheel_speed_left_a{ecu_name, "WheelSpeedLeftA", val::sil_client};
DigitalInput wheel_speed_left_b{ecu_name, "WheelSpeedLeftB", val::sil_client};
DigitalInput wheel_speed_right_a{ecu_name, "WheelSpeedRightA", val::sil_client};
DigitalInput wheel_speed_right_b{ecu_name, "WheelSpeedRightB", val::sil_client};
DigitalInput wait_for_start{ecu_name, "WaitForStart", val::sil_client};

}  // namespace mcal

namespace bindings {

shared::periph::CanBase& io_can_base = mcal::io_can_base;

shared::periph::ADCInput& accel_pedal_pos_1 = mcal::accel_pedal_pos_1;
shared::periph::ADCInput& accel_pedal_pos_2 = mcal::accel_pedal_pos_2;
shared::periph::ADCInput& brake_pedal_pos_1 = mcal::brake_pedal_pos_1;
shared::periph::ADCInput& brake_pedal_pos_2 = mcal::brake_pedal_pos_2;
shared::periph::ADCInput& suspension_travel_1 = mcal::suspension_travel_1;
shared::periph::ADCInput& suspension_travel_2 = mcal::suspension_travel_2;
shared::periph::ADCInput& hvil_feedback = mcal::hvil_feedback;
shared::periph::ADCInput& steering_angle = mcal::steering_angle;

shared::periph::DigitalInput& start_button_n = mcal::start_button_n;
shared::periph::DigitalInput& wheel_speed_left_a = mcal::wheel_speed_left_a;
shared::periph::DigitalInput& wheel_speed_left_b = mcal::wheel_speed_left_b;
shared::periph::DigitalInput& wheel_speed_right_a = mcal::wheel_speed_right_a;
shared::periph::DigitalInput& wheel_speed_right_b = mcal::wheel_speed_right_b;

shared::periph::DigitalOutput& debug_led_en = mcal::debug_led_en;
shared::periph::DigitalOutput& dashboard_en = mcal::dashboard_en;
shared::periph::DigitalOutput& hvil_led_en = mcal::hvil_led_en;
shared::periph::DigitalOutput& brake_light_en = mcal::brake_light_en;
shared::periph::DigitalOutput& status_led_en = mcal::status_led_en;
shared::periph::DigitalOutput& rtds_en = mcal::rtds_en;

void Initialize() {
    std::cout << "Initializing SIL..." << std::endl;
    std::cout << "Using address: " << server_addr << std::endl;

    val::sil_client.Connect();

    mcal::accel_pedal_pos_1.Register();
    mcal::accel_pedal_pos_2.Register();
    mcal::brake_pedal_pos_1.Register();
    mcal::brake_pedal_pos_2.Register();
    mcal::suspension_travel_1.Register();
    mcal::suspension_travel_2.Register();
    mcal::hvil_feedback.Register();
    mcal::steering_angle.Register();
    mcal::start_button_n.Register();
    mcal::wheel_speed_left_a.Register();
    mcal::wheel_speed_left_b.Register();
    mcal::wheel_speed_right_a.Register();
    mcal::wheel_speed_right_b.Register();
    mcal::debug_led_en.Register();
    mcal::dashboard_en.Register();
    mcal::hvil_led_en.Register();
    mcal::brake_light_en.Register();
    mcal::status_led_en.Register();
    mcal::rtds_en.Register();
    mcal::wait_for_start.Register();

    std::cout << "Waiting for start signal" << std::endl;
    while (!mcal::wait_for_start.Read()) {
    }
    std::cout << "Received start signal" << std::endl;
}

void TickBlocking(uint32_t ticks) {
    std::chrono::milliseconds duration(ticks);

    std::this_thread::sleep_for(duration);
}

}  // namespace bindings
