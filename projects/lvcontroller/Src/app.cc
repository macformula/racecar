/// @author Blake Freer
/// @date 2023-12-25

#include <cstdint>
#include <optional>

#include "adc.h"
#include "can.h"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "generated/githash.hpp"
#include "gpio.h"
#include "main.h"
#include "periph/can.hpp"
#include "periph/gpio.hpp"
#include "periph/pwm.hpp"
#include "stm32f/analog_input.hpp"
#include "stm32f/can.hpp"
#include "stm32f/gpio.hpp"
#include "stm32f/pwm.hpp"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "tim.h"

// LV Modules
#include "lv_common/accumulator/accumulator.hpp"
#include "lv_common/alerts/alerts.hpp"
#include "lv_common/brakelight/brakelight.hpp"
#include "lv_common/dcdc/dcdc.hpp"
#include "lv_common/fans/fans.hpp"
#include "lv_common/hsd/hsd.hpp"
#include "lv_common/motor_controller/motor_controller.hpp"
#include "lv_common/scheduler/scheduler.hpp"
#include "lv_common/suspension/suspension.hpp"
#include "lv_common/tssi/tssi.hpp"

using namespace generated::can;

// Tractive System Status Indicator
macfe::stm32f::DigitalOutput tssi_en{TSSI_EN_GPIO_Port, TSSI_EN_Pin};
macfe::stm32f::DigitalOutput tssi_red_signal_n{TSSI_RED_SIG_GPIO_Port,
                                               TSSI_RED_SIG_Pin};
macfe::stm32f::DigitalOutput tssi_green_signal_n{TSSI_GN_SIG_GPIO_Port,
                                                 TSSI_GN_SIG_Pin};

// Both red and green are inverted logic
macfe::periph::InvertedDigitalOutput tssi_red_signal{tssi_red_signal_n};
macfe::periph::InvertedDigitalOutput tssi_green_signal{tssi_green_signal_n};

macfe::stm32f::DigitalInput imd_fault{IMD_FAULT_STATUS_GPIO_Port,
                                      IMD_FAULT_STATUS_Pin};
macfe::stm32f::DigitalInput bms_fault{BMS_FAULT_STATUS_GPIO_Port,
                                      BMS_FAULT_STATUS_Pin};

// Powertrain Cooling
macfe::stm32f::DigitalOutput powertrain_pump1_en{POWERTRAIN_PUMP1_EN_GPIO_Port,
                                                 POWERTRAIN_PUMP1_EN_Pin};
macfe::stm32f::DigitalOutput powertrain_pump2_en{POWERTRAIN_PUMP2_EN_GPIO_Port,
                                                 POWERTRAIN_PUMP2_EN_Pin};
macfe::stm32f::DigitalOutput powertrain_fan1_en{POWERTRAIN_FAN1_EN_GPIO_Port,
                                                POWERTRAIN_FAN1_EN_Pin};
macfe::stm32f::DigitalOutput powertrain_fan2_en{POWERTRAIN_FAN2_EN_GPIO_Port,
                                                POWERTRAIN_FAN2_EN_Pin};
macfe::stm32f::PWMOutput powertrain_fan_pwm{&htim2, HAL_TIM_ACTIVE_CHANNEL_1};

// Motor Controller (i.e. Inverters)
macfe::stm32f::DigitalOutput motor_controller_en{MOTOR_CONTROLLER_EN_GPIO_Port,
                                                 MOTOR_CONTROLLER_EN_Pin};
macfe::stm32f::DigitalOutput motor_ctrl_precharge_en{
    MOTOR_CONTROLLER_PRECHARGE_EN_GPIO_Port, MOTOR_CONTROLLER_PRECHARGE_EN_Pin};
macfe::stm32f::DigitalOutput motor_ctrl_switch_en{
    MOTOR_CONTROLLER_SWITCH_EN_GPIO_Port, MOTOR_CONTROLLER_SWITCH_EN_Pin};

// Subsystems
macfe::stm32f::DigitalOutput accumulator_en{ACCUMULATOR_EN_GPIO_Port,
                                            ACCUMULATOR_EN_Pin};
macfe::stm32f::DigitalOutput front_controller_en{FRONT_CONTROLLER_EN_GPIO_Port,
                                                 FRONT_CONTROLLER_EN_Pin};
macfe::stm32f::DigitalOutput imu_gps_en{IMU_GPS_EN_GPIO_Port, IMU_GPS_EN_Pin};
macfe::stm32f::DigitalOutput shutdown_circuit_en{SHUTDOWN_CIRCUIT_EN_GPIO_Port,
                                                 SHUTDOWN_CIRCUIT_EN_Pin};

// DCDC System  Measurement
macfe::stm32f::DigitalOutput vicor_en{VICOR_EN_GPIO_Port, VICOR_EN_Pin};
// macfe::stm32f::DigitalOutput dcdc_sense_select{DCDC_SNS_SEL_GPIO_Port,
//                                                DCDC_SNS_SEL_Pin};
// // Analog Sensing (Rev 4)
macfe::stm32f::AnalogInput lv_battery{&hadc3, ADC_CHANNEL_15};
macfe::stm32f::AnalogInput bus_voltage{&hadc3, ADC_CHANNEL_14};
macfe::stm32f::AnalogInput bus_current{&hadc3, ADC_CHANNEL_9};

// HSD
macfe::stm32f::AnalogInput hsd1_isense{&hadc1, ADC_CHANNEL_8};
macfe::stm32f::DigitalOutput hsd1_isense_en{HSD1_I_SENSE_EN_GPIO_Port,
                                            HSD1_I_SENSE_EN_Pin};
macfe::stm32f::DigitalOutput hsd1_sel0{HSD1_SEL0_GPIO_Port, HSD1_SEL0_Pin};
macfe::stm32f::DigitalOutput hsd1_sel1{HSD1_SEL1_GPIO_Port, HSD1_SEL1_Pin};

macfe::stm32f::AnalogInput hsd2_isense{&hadc1, ADC_CHANNEL_9};
macfe::stm32f::DigitalOutput hsd2_isense_en{HSD2_I_SENSE_EN_GPIO_Port,
                                            HSD2_I_SENSE_EN_Pin};
macfe::stm32f::DigitalOutput hsd2_sel{HSD2_SEL_GPIO_Port, HSD2_SEL_Pin};

macfe::stm32f::AnalogInput hsd3_isense{&hadc3, ADC_CHANNEL_6};
macfe::stm32f::DigitalOutput hsd3_isense_en{HSD3_I_SENSE_EN_GPIO_Port,
                                            HSD3_I_SENSE_EN_Pin};
macfe::stm32f::DigitalOutput hsd3_sel{HSD3_SEL_GPIO_Port, HSD3_SEL_Pin};

macfe::stm32f::AnalogInput hsd4_isense{&hadc3, ADC_CHANNEL_7};
macfe::stm32f::DigitalOutput hsd4_isense_en{HSD4_I_SENSE_EN_GPIO_Port,
                                            HSD4_I_SENSE_EN_Pin};
macfe::stm32f::DigitalOutput hsd4_sel{HSD4_SEL_GPIO_Port, HSD4_SEL_Pin};

macfe::stm32f::AnalogInput hsd5_isense{&hadc3, ADC_CHANNEL_8};
macfe::stm32f::DigitalOutput hsd5_isense_en{HSD5_I_SENSE_EN_GPIO_Port,
                                            HSD5_I_SENSE_EN_Pin};
macfe::stm32f::DigitalOutput hsd5_sel{HSD5_SEL_GPIO_Port, HSD5_SEL_Pin};

macfe::stm32f::AnalogInput hsd6_isense{&hadc3, ADC_CHANNEL_5};
macfe::stm32f::DigitalOutput hsd6_isense_en{HSD6_I_SENSE_EN_GPIO_Port,
                                            HSD6_I_SENSE_EN_Pin};
macfe::stm32f::DigitalOutput hsd6_sel{HSD6_SEL_GPIO_Port, HSD6_SEL_Pin};

macfe::lv::HSD4 hsd1{hsd1_isense, hsd1_isense_en, hsd1_sel0, hsd1_sel1};
macfe::lv::HSD2 hsd2{hsd2_isense, hsd2_isense_en, hsd2_sel};
macfe::lv::HSD2 hsd3{hsd3_isense, hsd3_isense_en, hsd3_sel};
macfe::lv::HSD2 hsd4{hsd4_isense, hsd4_isense_en, hsd4_sel};
macfe::lv::HSD2 hsd5{hsd5_isense, hsd5_isense_en, hsd5_sel};
macfe::lv::HSD2 hsd6{hsd6_isense, hsd6_isense_en, hsd6_sel};

// Other IO
macfe::stm32f::DigitalOutput brake_light_en{BRAKE_LIGHT_EN_GPIO_Port,
                                            BRAKE_LIGHT_EN_Pin};
macfe::stm32f::AnalogInput suspension_travel3{&hadc1, ADC_CHANNEL_15};
macfe::stm32f::AnalogInput suspension_travel4{&hadc1, ADC_CHANNEL_14};

//! Just declare stuff here
macfe::stm32f::CanBase veh_can_base{&hcan2};
generated::can::VehBus veh_can{veh_can_base};

macfe::lv::Accumulator Accumulator = macfe::lv::Accumulator(accumulator_en);
macfe::lv::Fans Fans =
    macfe::lv::Fans(powertrain_fan_pwm, powertrain_fan1_en, powertrain_fan2_en);
macfe::lv::HSD HSD = macfe::lv::HSD(hsd1, hsd2, hsd3, hsd4, hsd5, hsd6);
macfe::lv::Motor_Controller Motor_Controller = macfe::lv::Motor_Controller(
    motor_controller_en, motor_ctrl_precharge_en, motor_ctrl_switch_en);
macfe::lv::TSSI TSSI = macfe::lv::TSSI(tssi_en, tssi_red_signal,
                                       tssi_green_signal, imd_fault, bms_fault);

extern "C" {
/**
 * This requires extern since it is not declared in a header, only defined
 * in cubemx/../main.c
 */
// void MPU_Config();
void SystemClock_Config();
}
void Initialize() {
    // MPU_Config();
    SystemClock_Config();
    HAL_Init();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_ADC3_Init();
    MX_CAN2_Init();
    MX_TIM2_Init();

    veh_can_base.Setup();
}

void DelayMS(uint32_t milliseconds) {
    HAL_Delay(milliseconds);
}

int GetTick() {
    static uint32_t first_tick = HAL_GetTick();
    return HAL_GetTick() - first_tick;
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

namespace fsm {

using LvState = TxLvStatus::LvState_t;

static LvState state = LvState::PWRUP_START;
static uint32_t elapsed = 0;
void Init(void) {
    state = LvState::PWRUP_START;
    elapsed = 0;
}

void Update_100hz(void) {
    using enum LvState;

    LvState new_state = state;

    switch (state) {
        case PWRUP_START:
            TSSI.SetEnabled(false);
            //! DCDC.Controller.SetEnabled(false);
            Fans.SetPowerSetpoint(0);
            Motor_Controller.SetEnabled(false);
            Accumulator.SetEnabled(false);

            front_controller_en.SetLow();
            imu_gps_en.SetLow();
            powertrain_pump1_en.SetLow();
            powertrain_pump2_en.SetLow();
            shutdown_circuit_en.SetLow();

            if (elapsed > 50) {
                new_state = PWRUP_TSSI_ON;
            }
            break;

        case PWRUP_TSSI_ON:
            TSSI.SetEnabled(true);

            if (elapsed > 50) {
                new_state = PWRUP_PERIPHERALS_ON;
            }
            break;

        case PWRUP_PERIPHERALS_ON:
            front_controller_en.SetHigh();
            imu_gps_en.SetHigh();

            if (elapsed > 50) {
                if (veh_can.GetRxLvCommand().has_value()) {
                    // don't actually care about the status, just that
                    // FC has come online
                    new_state = PWRUP_ACCUMULATOR_ON;
                }
            }
            break;

        case PWRUP_ACCUMULATOR_ON:
            Accumulator.SetEnabled(true);

            if (Accumulator.ConfirmContactorsOpen()) {
                new_state = PWRUP_MOTOR_CONTROLLER_PRECHARGING;
            }
            break;

        case PWRUP_MOTOR_CONTROLLER_PRECHARGING:
            Motor_Controller.SetEnabled(true);

            if (Motor_Controller.GetState() !=
                generated::can::TxLvStatus::MotorControllerState_t::ON) {
                new_state = PWRUP_SHUTDOWN_ON;
            }
            break;

        case PWRUP_SHUTDOWN_ON:
            shutdown_circuit_en.SetHigh();

            if (Accumulator.IsRunning()) {
                veh_can.Send(TxIMD_Request{
                    .index = 0x47,
                    .data0 = 0x2C,
                    .data1 = 0x01,
                });
                new_state = DCDC_ON;
            }
            break;

        case DCDC_ON:
            // DCDC.Controller.SetEnabled(true);

            if (elapsed > 100) {
                new_state = POWERTRAIN_PUMP_ON;
            }
            break;

        case POWERTRAIN_PUMP_ON:
            powertrain_pump1_en.SetHigh();
            powertrain_pump2_en.SetHigh();

            if (elapsed > 100) {
                new_state = POWERTRAIN_FAN_ON;
            }
            break;

        case POWERTRAIN_FAN_ON:
            Fans.SetPowerSetpoint(100);

            if (Fans.IsAtSetpoint() || elapsed > 10000) {
                new_state = READY_TO_DRIVE;
            }
            break;

        case READY_TO_DRIVE:
            break;

        case SHUTDOWN_DRIVER_WARNING:
            // This state's action is "sending the State over CAN" which
            // happens before the switch

            // Give the driver 30 s to stop before we make them.
            if (elapsed > 30000) {
                new_state = SHUTDOWN_PUMP_OFF;
            }
            break;

        case SHUTDOWN_PUMP_OFF:
            powertrain_pump1_en.SetLow();
            powertrain_pump2_en.SetLow();

            if (elapsed > 50) {
                new_state = SHUTDOWN_FAN_OFF;
            }
            break;

        case SHUTDOWN_FAN_OFF:
            Fans.SetPowerSetpoint(0);

            if (elapsed > 50) {
                new_state = PWRUP_ACCUMULATOR_ON;
            }

            break;
    }

    bool check_shutdown = state == DCDC_ON || state == POWERTRAIN_PUMP_ON ||
                          state == POWERTRAIN_FAN_ON || state == READY_TO_DRIVE;

    if (check_shutdown && Accumulator.ConfirmContactorsOpen()) {
        new_state = SHUTDOWN_PUMP_OFF;
    }

    // no LVBMS yet, so we can't check for shutdown
    /*
    // Shutdown checks that can occur from multiple states
    bool check_shutdown = state == DCDC_ON || state ==
    POWERTRAIN_PUMP_ON || state == POWERTRAIN_FAN_ON || state ==
    READY_TO_DRIVE;

    if (check_shutdown) {
        const float kNoCurrentThresholdAmp = 0.5;  // what should this
    be? bool lost_lv_comms = false;  // this should come from a SPI
    heartbeat

        if (dcdc::GetVoltage() < 19.2 ||
            (lost_lv_comms && dcdc::GetAmps() < kNoCurrentThresholdAmp))
    { new_state = SHUTDOWN_DRIVER_WARNING;
        }

        auto msg = veh_can.GetRxFcStatus();
        // remove static_cast once cangen properly handles enums
        if (msg.has_value())
            // -1 is NOT SENT by FC as of 2025/02/12. This should be
            // updated in the future once FC sends useful statuses
            if (msg->GovStatus() == static_cast<uint8_t>(-1)) {
                new_state = SHUTDOWN_PUMP_OFF;
            }
    }
    */

    // If a transition was indicated, handle it
    if (new_state != state) {
        elapsed = 0;
        state = new_state;
    } else {
        elapsed += 10;
    }
}

}  // namespace fsm

void check_can_flash(void) {
    auto msg = veh_can.GetRxInitiateCanFlash();

    if (msg.has_value() &&
        msg->ECU() == RxInitiateCanFlash::ECU_t::LvController) {
        SoftwareReset();
    }
}

void task_1hz(void) {
    veh_can.Send(TxLvDbcHash(generated::can::kVehDbcHash));
    veh_can.Send(TxLvGitHash{
        .commit = generated::githash::GIT_HASH,
        .dirty = generated::githash::GIT_DIRTY,
    });
    veh_can.Send(TxIMD_Request{.index = 0x4A});  // ask for warning threshold
    veh_can.GetRxIMD_Response();
}

void task_10hz(void) {
    static uint8_t tx_counter = 0;

    suspension::task_10hz(veh_can, suspension_travel3, suspension_travel4);
    TSSI.task_10hz();
    Accumulator.task_10hz(veh_can);
    // check_can_flash(); // unused in 2025
    HSD.Update_10Hz(veh_can);

    veh_can.Send(TxLvAlerts{
        .hsd_overcurrent = alerts::Get().hsd_overcurrent,
        .imd_fault = TSSI.GetImdFault(),
        .bms_fault = TSSI.GetBmsFault(),
    });

    // veh_can.Send(TxLvDcdc{
    //     .lv_battery_voltage = DCDC.Controller.GetLvBatteryVoltage(),
    //     .bus_voltage = DCDC.Controller.GetVoltage(),
    //     .bus_current = DCDC.Controller.GetAmps(),
    // });

    veh_can.Send(TxLvStatus{
        .counter = tx_counter++,
        .lv_state = fsm::state,
        .motor_controller_state = Motor_Controller.GetState(),
        .motor_controller_switch_closed = Motor_Controller.GetSwitchClosed(),
    });
}

void task_100hz(void) {
    // dcdc::task_100hz();
    brake_light::task_100hz(veh_can, brake_light_en);
    Motor_Controller.task_100hz(veh_can);

    fsm::Update_100hz();
    Fans.Update_100Hz();
}

int main(void) {
    fsm::Init();

    scheduler::register_task(task_100hz, 10);
    scheduler::register_task(task_10hz, 100);
    scheduler::register_task(task_1hz, 1000);

    scheduler::run(GetTick());

    while (true) continue;

    return 0;
}