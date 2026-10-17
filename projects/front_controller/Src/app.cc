/// @author Blake Freer
/// @date 2025-06

#include "adc.h"
#include "can.h"
#include "fc_common/accumulator/accumulator.hpp"
#include "fc_common/alerts/alerts.hpp"
#include "fc_common/dbc_hash/dbc_hash.hpp"
#include "fc_common/driver_interface/driver_interface.hpp"
#include "fc_common/hsd/hsd.hpp"
#include "fc_common/motors/motors.hpp"
#include "fc_common/sensors/driver/driver.hpp"
#include "fc_common/sensors/dynamics/dynamics.hpp"
#include "fc_common/suspension/suspension.hpp"
#include "fc_common/vehicle_dynamics/vehicle_dynamics.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "generated/githash.hpp"
#include "gpio.h"
#include "main.h"
#include "physical.hpp"
#include "stm32f/analog_input.hpp"
#include "stm32f/can.hpp"
#include "stm32f/gpio.hpp"
#include "thresholds.hpp"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

extern "C" void SystemClock_Config();

static const size_t STACK_SIZE_WORDS =
    2048 * 16;  // TODO: Does this need to be so large?

// higher number = higher priority
static const uint32_t PRIORITY_100HZ = 3;
static const uint32_t PRIORITY_10HZ = 2;
static const uint32_t PRIORITY_1HZ = 1;

StaticTask_t t100hz_control_block;
StackType_t t100hz_buffer[STACK_SIZE_WORDS];

StaticTask_t t10hz_control_block;
StackType_t t10hz_buffer[STACK_SIZE_WORDS];

StaticTask_t t1hz_control_block;
StackType_t t1hz_buffer[STACK_SIZE_WORDS];

using generated::can::kVehDbcHash;
using generated::can::VehBus;
using macfe::stm32f::AnalogInput;
using macfe::stm32f::DigitalInput;
using macfe::stm32f::DigitalOutput;

// =========== Vehicle Dynamics Sensors ====================
AnalogInput suspension_travel1{&hadc1, ADC_CHANNEL_8};
AnalogInput suspension_travel2{&hadc1, ADC_CHANNEL_9};
AnalogInput wheel_speed_front_left{&hadc1, ADC_CHANNEL_15};
AnalogInput wheel_speed_front_right{&hadc3, ADC_CHANNEL_14};
AnalogInput wheel_speed_rear_left{&hadc1, ADC_CHANNEL_14};
AnalogInput wheel_speed_rear_right{&hadc3, ADC_CHANNEL_9};

// =========== Driver Control ==============================
AnalogInput steering_angle_sensor{&hadc1, ADC_CHANNEL_2};
AnalogInput accel_pedal_sensor1{&hadc3, ADC_CHANNEL_6};
AnalogInput accel_pedal_sensor2{&hadc3, ADC_CHANNEL_7};
AnalogInput brake_pressure_sensor{&hadc1, ADC_CHANNEL_12};

// =========== Status Monitors =============================
AnalogInput precharge_monitor{&hadc1, ADC_CHANNEL_1};
DigitalInput bspd_fault{BSPD_FAULT_GPIO_Port, BSPD_FAULT_Pin};

// =========== Outputs =====================================
DigitalOutput dashboard_power_en{DASHBOARD_HSD_EN_GPIO_Port,
                                 DASHBOARD_HSD_EN_Pin};
DigitalOutput imd_fault_led_en{IMD_FAULT_LED_EN_GPIO_Port,
                               IMD_FAULT_LED_EN_Pin};
DigitalOutput bms_fault_led_en{BMS_FAULT_LED_EN_GPIO_Port,
                               BMS_FAULT_LED_EN_Pin};
DigitalOutput ready_to_drive_sig_en{RTDS_EN_GPIO_Port, RTDS_EN_Pin};
DigitalOutput debug_led{DEBUG_LED_GPIO_Port, DEBUG_LED_Pin};

// =========== HSD =========================================
AnalogInput hsd1_isense{&hadc1, ADC_CHANNEL_10};
DigitalOutput hsd1_isense_en{HSD1_I_SENSE_EN_GPIO_Port, HSD1_I_SENSE_EN_Pin};
DigitalOutput hsd1_sel0{HSD1_SEL0_GPIO_Port, HSD1_SEL0_Pin};
DigitalOutput hsd1_sel1{HSD1_SEL1_GPIO_Port, HSD1_SEL1_Pin};

AnalogInput hsd2_isense{&hadc1, ADC_CHANNEL_11};
DigitalOutput hsd2_isense_en{HSD2_I_SENSE_EN_GPIO_Port, HSD2_I_SENSE_EN_Pin};

macfe::fc::HSD4 hsd1{hsd1_isense, hsd1_isense_en, hsd1_sel0, hsd1_sel1};
macfe::fc::HSD1 hsd2{hsd2_isense, hsd2_isense_en};

macfe::stm32f::CanBase veh_can_base{&hcan2};
macfe::stm32f::CanBase pt_can_base{&hcan1};

generated::can::VehBus veh_can_bus{veh_can_base};
generated::can::PtBus pt_can_bus{pt_can_base};

// Controllers own their state; sensor dependencies are shared explicitly.
macfe::fc::sensors::DriverSensors driver_sensors =
    macfe::fc::sensors::DriverSensors(steering_angle_sensor,
                                      brake_pressure_sensor,
                                      accel_pedal_sensor1, accel_pedal_sensor2);
macfe::fc::DriverInterface driver_controls{driver_sensors};
macfe::fc::VehicleDynamics dynamics{driver_sensors};
//! move state to state
macfe::fc::Accumulator accumulator = macfe::fc::Accumulator(precharge_monitor);
macfe::fc::Motors motors = macfe::fc::Motors();
macfe::fc::HSD hsd = macfe::fc::HSD(hsd1, hsd2);
void Initialize() {
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_ADC3_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();

    veh_can_base.Setup();
    pt_can_base.Setup();
}

int GetTickMs() {
    return HAL_GetTick();
}

void DelayMs(int ms) {
    HAL_Delay(ms);
}

void SoftwareReset() {
    NVIC_SystemReset();
    Error_Handler();
}

namespace fsm {
using State = generated::can::TxFcStatus::State_t;

static State state = State::START_DASHBOARD;
static uint32_t elapsed = 0;

static void Update_100Hz(void) {
    using enum State;
    using DashState = generated::can::RxDashStatus::State_t;

    State new_state = state;

    macfe::fc::Accumulator::Command acc_cmd =
        macfe::fc::Accumulator::Command::OFF;
    macfe::fc::Motors::Command motor_cmd = macfe::fc::Motors::Command::OFF;
    bool speaker = false;
    float torque_request = 0.f;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    switch (state) {
        case START_DASHBOARD: {
            acc_cmd = macfe::fc::Accumulator::Command::OFF;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            // wait until dashboard comes online
            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value() &&
                (msg->State() == DashState::LOGO ||
                 msg->State() == DashState::SELECT_PROFILE)) {
                // notice: ideally we should only check for select profile if
                // we're going to bypass the logo screen, this will need to be
                // changed
                new_state = WAIT_DRIVER_SELECT;
            } else if (elapsed > timeout::DASHBOARD_BOOT_TIME) {
                alerts::Get().dashboard_boot_timeout = true;
            } else {
                // keep waiting
            }
        } break;

        case WAIT_DRIVER_SELECT: {
            acc_cmd = macfe::fc::Accumulator::Command::OFF;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value()) {
                if (msg->State() == DashState::WAIT_SELECTION_ACK) {
                    dynamics.SetProfile(msg->Profile());
                    new_state = WAIT_START_HV;
                }
            }
        } break;

        case WAIT_START_HV: {
            acc_cmd = macfe::fc::Accumulator::Command::OFF;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value()) {
                if (dash->State() == DashState::STARTING_HV) {
                    new_state = STARTING_HV;
                }
            }
        } break;

        case STARTING_HV:
            acc_cmd = macfe::fc::Accumulator::Command::ENABLED;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator.GetState() ==
                macfe::fc::Accumulator::State::RUNNING) {
                new_state = WAIT_START_MOTOR;
            } else if (accumulator.GetState() ==
                       macfe::fc::Accumulator::State::ERROR) {
                new_state = ERROR;
            } else {
                // waiting
            }
            break;

        case WAIT_START_MOTOR: {
            acc_cmd = macfe::fc::Accumulator::Command::ENABLED;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value()) {
                if (dash->State() == DashState::STARTING_MOTORS) {
                    new_state = STARTING_MOTORS;
                }
            }
        } break;

        case STARTING_MOTORS:
            acc_cmd = macfe::fc::Accumulator::Command::ENABLED;
            motor_cmd = macfe::fc::Motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (motors.GetState() == macfe::fc::Motors::State::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (motors.GetState() == macfe::fc::Motors::State::ERROR) {
                new_state = ERROR;
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            acc_cmd = macfe::fc::Accumulator::Command::ENABLED;
            motor_cmd = macfe::fc::Motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (driver_controls.IsBrakePressed()) {
                new_state = RUNNING;
            }
            break;

        case RUNNING: {
            acc_cmd = macfe::fc::Accumulator::Command::ENABLED;
            motor_cmd = macfe::fc::Motors::Command::ENABLED;
            speaker = elapsed < timeout::SPEAKER_DURATION;
            torque_request = driver_controls.GetTorqueRequest();

            if (accumulator.GetState() ==
                macfe::fc::Accumulator::State::ERROR) {
                new_state = ERROR;
            }

            if (motors.GetState() == macfe::fc::Motors::State::ERROR) {
                new_state = ERROR;
            }

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value() && (dash->State() == DashState::SHUTDOWN)) {
                new_state = SHUTDOWN;
            }
            auto inv1_2 = pt_can_bus.GetRxInv1_ActualValues2();
            auto inv2_2 = pt_can_bus.GetRxInv2_ActualValues2();
            veh_can_bus.Send(generated::can::TxInverterStatus{
                .temp_motor_inv1 = inv1_2->TempMotor(),
                .temp_inverter_inv1 = inv1_2->TempInverter(),
                .temp_motor_inv2 = inv2_2->TempMotor(),
                .temp_inverter_inv2 = inv2_2->TempInverter(),
            });
        } break;

        case SHUTDOWN:
            acc_cmd = macfe::fc::Accumulator::Command::OFF;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator.GetState() == macfe::fc::Accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = START_DASHBOARD;
            }
            break;

        case ERROR: {
            acc_cmd = macfe::fc::Accumulator::Command::OFF;
            motor_cmd = macfe::fc::Motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value()) {
                if (msg->State() == DashState::SHUTDOWN) {
                    new_state = SHUTDOWN;
                }
            }

        } break;
    }

    if (hvil_interrupt) {
        new_state = SHUTDOWN;
    }

    accumulator.SetCommand(acc_cmd);
    motors.SetCommand(motor_cmd);
    dynamics.SetDriverTorqueRequest(torque_request);
    ready_to_drive_sig_en.Set(speaker);

    if (new_state != state) {
        state = new_state;
        elapsed = 0;
    } else {
        elapsed += 10;
    }
}
}  // namespace fsm

// Shows that the ECU is alive
void ToggleDebugLed() {
    static bool toggle = true;
    debug_led.Set(toggle);
    toggle = !toggle;
}

// Reboot the ECU. Assumes that the Rasberry Pi has pulled the BOOT pin high
// so that the ECU enters bootloader mode.
void CheckCanFlash() {
    auto msg = veh_can_bus.GetRxInitiateCanFlash();

    if (msg.has_value() &&
        msg->ECU() ==
            generated::can::RxInitiateCanFlash::ECU_t::FrontController) {
        SoftwareReset();
    }
}

void UpdateErrorLeds() {
    auto error_led = veh_can_bus.GetRxLvAlerts();
    if (error_led.has_value()) {
        imd_fault_led_en.Set(error_led->ImdFault());
        bms_fault_led_en.Set(error_led->BmsFault());
    } else {
        // Default to lights on
        imd_fault_led_en.SetHigh();
        bms_fault_led_en.SetHigh();
    }
}

void task_1hz(void* argument) {
    (void)argument;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        veh_can_bus.Send(generated::can::TxFcGitHash{
            .commit = generated::githash::GIT_HASH,
            .dirty = generated::githash::GIT_DIRTY,
        });

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(1000));
    }
}

void task_10hz(void* argument) {
    (void)argument;
    static uint8_t tx_counter = 0;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        ToggleDebugLed();
        UpdateErrorLeds();
        dbc_hash::Update_10Hz(veh_can_bus);
        // CheckCanFlash();  // no CAN flash in 2025. pcb needs an external
        // oscillator
        suspension::ReadAndSendTravel(veh_can_bus, suspension_travel1,
                                      suspension_travel2);
        hsd.Update_10Hz(veh_can_bus);

        veh_can_bus.Send(generated::can::TxFcStatus{
            .counter = tx_counter++,
            .state = fsm::state,
            .accumulator_state = accumulator.GetState(),
            .motor_state = motors.GetState(),
            .inv1_state = motors.GetLeftState(),
            .inv2_state = static_cast<generated::can::TxFcStatus::Inv2State_t>(
                motors.GetRightState()),
            .dbc_valid = dbc_hash::IsValid(),
            .inv1_starter = static_cast<uint8_t>(motors.GetLeftStarterState()),
            .inv2_starter = static_cast<uint8_t>(motors.GetRightStarterState()),
        });
        veh_can_bus.Send(generated::can::TxDashCommand{
            .config_received = fsm::state == fsm::State::WAIT_START_HV,
            .hv_started = accumulator.GetState() ==
                          macfe::fc::Accumulator::State::RUNNING,
            .motor_started =
                motors.GetState() == macfe::fc::Motors::State::RUNNING,
            .drive_started = fsm::state == fsm::State::RUNNING,
            .reset = fsm::state == fsm::State::START_DASHBOARD,
            .errored = fsm::state == fsm::State::ERROR,
            .hv_shutdown = fsm::state == fsm::State::SHUTDOWN,
            .hv_precharge_percent =
                static_cast<uint8_t>(accumulator.GetPrechargePercent()),
            .speed = motors.GetMph(),
            .hv_soc_percent = static_cast<uint8_t>(accumulator.GetSocPercent()),
        });

        veh_can_bus.Send(accumulator.GetDebugMsg());
        veh_can_bus.Send(alerts::Get());

        veh_can_bus.Send(driver_sensors.GetAppsDebugMsg());
        veh_can_bus.Send(driver_sensors.GetBppsSteerDebugMsg());
        veh_can_bus.Send(motors.GetCounters());

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(100));
    }
}

void task_100hz(void* argument) {
    (void)argument;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        driver_sensors.Update_100Hz();
        sensors::dynamics::Update_100Hz(
            wheel_speed_front_left, wheel_speed_front_right,
            wheel_speed_rear_left, wheel_speed_rear_right);
        driver_controls.Update_100Hz();

        fsm::Update_100Hz();

        accumulator.Update_100Hz(veh_can_bus);
        dynamics.Update_100Hz();
        motors.Update_100Hz(pt_can_bus, veh_can_bus,
                            dynamics.GetLeftMotorRequest(),
                            dynamics.GetRightMotorRequest());

        veh_can_bus.Send(accumulator.GetContactorCommand());
        veh_can_bus.Send(generated::can::TxLvCommand{
            .brake_light_enable = driver_controls.IsBrakePressed(),
        });

        // Disable Inverter commands for manual debugging
        veh_can_bus.Send(generated::can::TxInverterSwitchCommand{
            .close_inverter_switch = motors.GetInverterEnable(),
        });
        pt_can_bus.Send(motors.GetLeftSetpoints());
        pt_can_bus.Send(motors.GetRightSetpoints());

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(10));
    }
}

int main(void) {
    Initialize();

    alerts::Reset();
    dynamics.Init(GetTickMs());

    // todo (2026): this is always on. just tie high on PCB
    dashboard_power_en.SetHigh();

    xTaskCreateStatic(task_100hz, "100HZ", STACK_SIZE_WORDS, NULL,
                      PRIORITY_100HZ, t100hz_buffer, &t100hz_control_block);

    xTaskCreateStatic(task_10hz, "10HZ", STACK_SIZE_WORDS, NULL, PRIORITY_10HZ,
                      t10hz_buffer, &t10hz_control_block);

    xTaskCreateStatic(task_1hz, "1HZ", STACK_SIZE_WORDS, NULL, PRIORITY_1HZ,
                      t1hz_buffer, &t1hz_control_block);

    vTaskStartScheduler();

    while (true) continue;

    return 0;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              char* pcTaskName);

// Optional: call this periodically, e.g., from task_10hz or task_1hz

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              char* pcTaskName) {
    // Handle stack overflow (e.g., light an LED, halt, etc.)
    debug_led.SetHigh();
    macfe::periph::CanErrorHandler(&veh_can_base);
    while (1) {
        // Trap CPU here
    }
}
