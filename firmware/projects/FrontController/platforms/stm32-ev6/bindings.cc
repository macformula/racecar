#include "../../bindings.hpp"
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "mcal/stm32f767/periph/analog_input.hpp"
#include "mcal/stm32f767/periph/can.hpp"
#include "mcal/stm32f767/periph/gpio.hpp"
#include "stm32f7xx_hal.h"

extern "C" {
// This requires extern since it is not declared in a header, only defined in
// cubemx/../main.c
void SystemClock_Config();
}

namespace mcal {
using namespace stm32f767::periph;

// Power, CAN, JTAG, PWM Section:
AnalogInput precharge_monitor{&hadc, ADC_CHANNEL_1};
AnalogInput steering_angle{&hadc1, ADC_CHANNEL_2};
DigitalOutput dashboard_power_en{GPIOA, GPIO_PIN_6};
CanBase pt_can_base{&hcan1};

// I/O
AnalogInput suspension_travel1{&hadc1, ADC_CHANNEL_8};
AnalogInput suspension_travel2{&hadc1, ADC_CHANNEL_9};
CanBase veh_can_base{&hcan2};

AnalogInput brake_pressure_sensor{&hadc1, ADC_CHANNEL_12};
AnalogInput wheel_speed_rear_left{&hadc1, ADC_CHANNEL_14};
AnalogInput wheel_speed_front_left{&hadc1, ADC_CHANNEL_15};

AnalogInput wheel_speed_front_right{&hadc3, ADC_CHANNEL_14};
AnalogInput wheel_speed_rear_right{&hadc3, ADC_CHANNEL_9};

AnalogInput accel_pedal_sensor1{&hadc3, ADC_CHANNEL_6};
AnalogInput accel_pedal_sensor2{&hadc3, ADC_CHANNEL_7};

DigitalInput bspd_fault{GPIOF, GPIO_PIN_10};
DigitalOutput imd_fault_led_en{GPIOF, GPIO_PIN_14};
DigitalOutput bms_fault_led_en{GPIOF, GPIO_PIN_15};

DigitalOutput ready_to_drive_sig_en{GPIOG, GPIO_PIN_0};

// Debug
DigitalOutput debug_led{GPIOA, GPIO_PIN_3};

}  // namespace mcal

namespace bindings {

shared::periph::AnalogInput& suspension_travel1 = mcal::suspension_travel1;
shared::periph::AnalogInput& suspension_travel2 = mcal::suspension_travel2;

shared::periph::AnalogInput& wheel_speed_front_left =
    mcal::wheel_speed_front_left;
shared::periph::AnalogInput& wheel_speed_front_right =
    mcal::wheel_speed_front_right;
shared::periph::AnalogInput& wheel_speed_rear_left =
    mcal::wheel_speed_rear_left;
shared::periph::AnalogInput& wheel_speed_rear_right =
    mcal::wheel_speed_rear_right;

shared::periph::AnalogInput& steering_angle = mcal::steering_angle;

shared::periph::AnalogInput& accel_pedal_sensor1 = mcal::accel_pedal_sensor1;
shared::periph::AnalogInput& accel_pedal_sensor2 = mcal::accel_pedal_sensor2;

shared::periph::AnalogInput& brake_pressure_sensor =
    mcal::brake_pressure_sensor;

shared::periph::AnalogInput& precharge_monitor = mcal::precharge_monitor;

shared::periph::DigitalOutput& dashboard_power_en = mcal::dashboard_power_en;
shared::periph::DigitalOutput& imd_fault_led_en = mcal::imd_fault_led_en;
shared::periph::DigitalOutput& bms_fault_led_en = mcal::bms_fault_led_en;
shared::periph::DigitalOutput& ready_to_drive_sig_en =
    mcal::ready_to_drive_sig_en;
shared::periph::DigitalOutput& debug_led = mcal::debug_led;

shared::periph::DigitalInput& bspd_fault = mcal::bspd_fault;

shared::periph::CanBase& veh_can_base = mcal::veh_can_base;
shared::periph::CanBase& pt_can_base = mcal::pt_can_base;

void Initialize() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_ADC3_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();

    mcal::veh_can_base.Setup();
    mcal::pt_can_base.Setup();
}

}  // namespace bindings
