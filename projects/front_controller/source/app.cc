/// @author Blake Freer
/// @date 2025-06

#include "bindings.hpp"
#include "fc-common/accumulator/accumulator.hpp"
#include "fc-common/alerts/alerts.hpp"
#include "fc-common/dbc_hash/dbc_hash.hpp"
#include "fc-common/driver_interface/driver_interface.hpp"
#include "fc-common/motors/motors.hpp"
#include "fc-common/sensors/driver/driver.hpp"
#include "fc-common/sensors/dynamics/dynamics.hpp"
#include "fc-common/suspension/suspension.hpp"
#include "fc-common/vehicle_dynamics/vehicle_dynamics.hpp"
#include "generated/can/pt_bus.hpp"
#include "generated/can/pt_messages.hpp"
#include "generated/can/veh_bus.hpp"
#include "generated/can/veh_messages.hpp"
#include "generated/githash.hpp"
#include "physical.hpp"
#include "thresholds.hpp"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

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

using namespace generated::can;
using mcal::stm32f;

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

hsd::HSD4Channel hsd1{hsd1_isense, hsd1_isense_en, hsd1_sel0, hsd1_sel1};
hsd::HSD1Channel hsd2{hsd2_isense, hsd2_isense_en};

CanBase veh_can_base{&hcan2};
CanBase pt_can_base{&hcan1};

VehBus veh_can_bus{veh_can_base};
PtBus pt_can_bus{pt_can_base};

namespace fsm {
using State = TxFcStatus::State_t;

static State state = State::START_DASHBOARD;
static uint32_t elapsed = 0;

static void Update_100Hz(void) {
    using enum State;
    using DashState = RxDashStatus::State_t;

    State new_state = state;

    accumulator::Command acc_cmd = accumulator::Command::OFF;
    motors::Command motor_cmd = motors::Command::OFF;
    bool speaker = false;
    float torque_request = 0.f;

    bool hvil_interrupt = false;  // not part of EV6. What should this be?

    switch (state) {
        case START_DASHBOARD: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
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
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            auto msg = veh_can_bus.GetRxDashStatus();
            if (msg.has_value()) {
                if (msg->State() == DashState::WAIT_SELECTION_ACK) {
                    vehicle_dynamics::SetProfile(msg->Profile());
                    new_state = WAIT_START_HV;
                }
            }
        } break;

        case WAIT_START_HV: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
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
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::RUNNING) {
                new_state = WAIT_START_MOTOR;
            } else if (accumulator::GetState() == accumulator::State::ERROR) {
                new_state = ERROR;
            } else {
                // waiting
            }
            break;

        case WAIT_START_MOTOR: {
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::OFF;
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
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (motors::GetState() == motors::State::RUNNING) {
                new_state = STARTUP_SEND_READY_TO_DRIVE;
            }

            if (motors::GetState() == motors::State::ERROR) {
                new_state = ERROR;
            }
            break;

        case STARTUP_SEND_READY_TO_DRIVE:
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = false;
            torque_request = 0;

            if (driver_interface::IsBrakePressed()) {
                new_state = RUNNING;
            }
            break;

        case RUNNING: {
            acc_cmd = accumulator::Command::ENABLED;
            motor_cmd = motors::Command::ENABLED;
            speaker = elapsed < timeout::SPEAKER_DURATION;
            torque_request = driver_interface::GetTorqueRequest();

            if (accumulator::GetState() == accumulator::State::ERROR) {
                new_state = ERROR;
            }

            if (motors::GetState() == motors::State::ERROR) {
                new_state = ERROR;
            }

            auto dash = veh_can_bus.GetRxDashStatus();
            if (dash.has_value() && (dash->State() == DashState::SHUTDOWN)) {
                new_state = SHUTDOWN;
            }
        } break;

        case SHUTDOWN:
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
            speaker = false;
            torque_request = 0;

            if (accumulator::GetState() == accumulator::State::IDLE) {
                // should probably check if motors and DI have shut down
                new_state = START_DASHBOARD;
            }
            break;

        case ERROR: {
            acc_cmd = accumulator::Command::OFF;
            motor_cmd = motors::Command::OFF;
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

    accumulator::SetCommand(acc_cmd);
    motors::SetCommand(motor_cmd);
    vehicle_dynamics::SetDriverTorqueRequest(torque_request);
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
        msg->ECU() == RxInitiateCanFlash::ECU_t::FrontController) {
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
        veh_can_bus.Send(TxFcGitHash{
            .commit = macfe::generated::GIT_HASH,
            .dirty = macfe::generated::GIT_DIRTY,
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
        suspension::task_10hz(veh_can_bus);
        hsd::Update_10Hz(veh_can_bus);

        veh_can_bus.Send(TxFcStatus{
            .counter = tx_counter++,
            .state = fsm::state,
            .accumulator_state = accumulator::GetState(),
            .motor_state = motors::GetState(),
            .inv1_state = motors::GetLeftState(),
            .inv2_state =
                static_cast<TxFcStatus::Inv2State_t>(motors::GetRightState()),
            .dbc_valid = dbc_hash::IsValid(),
            .inv1_starter = static_cast<uint8_t>(motors::GetLeftStarterState()),
            .inv2_starter =
                static_cast<uint8_t>(motors::GetRightStarterState()),
        });
        veh_can_bus.Send(TxDashCommand{
            .config_received = fsm::state == fsm::State::WAIT_START_HV,
            .hv_started =
                accumulator::GetState() == accumulator::State::RUNNING,
            .motor_started = motors::GetState() == motors::State::RUNNING,
            .drive_started = fsm::state == fsm::State::RUNNING,
            .reset = fsm::state == fsm::State::START_DASHBOARD,
            .errored = fsm::state == fsm::State::ERROR,
            .hv_precharge_percent =
                static_cast<uint8_t>(accumulator::GetPrechargePercent()),
            .speed = motors::GetMph(),
            .hv_soc_percent =
                static_cast<uint8_t>(accumulator::GetSocPercent()),
        });

        veh_can_bus.Send(accumulator::GetDebugMsg());
        veh_can_bus.Send(alerts::Get());

        veh_can_bus.Send(sensors::driver::GetAppsDebugMsg());
        veh_can_bus.Send(sensors::driver::GetBppsSteerDebugMsg());
        veh_can_bus.Send(motors::GetCounters());

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(100));
    }
}

void task_100hz(void* argument) {
    (void)argument;

    TickType_t wake_time = xTaskGetTickCount();

    while (true) {
        sensors::driver::Update_100Hz();
        sensors::dynamics::Update_100Hz();
        driver_interface::Update_100Hz();

        fsm::Update_100Hz();

        accumulator::Update_100Hz(veh_can_bus);
        vehicle_dynamics::Update_100Hz();
        motors::Update_100Hz(pt_can_bus, veh_can_bus,
                             vehicle_dynamics::GetLeftMotorRequest(),
                             vehicle_dynamics::GetRightMotorRequest());

        veh_can_bus.Send(accumulator::GetContactorCommand());
        veh_can_bus.Send(TxLvCommand{
            .brake_light_enable = driver_interface::IsBrakePressed(),
        });

        // Disable Inverter commands for manual debugging
        veh_can_bus.Send(TxInverterSwitchCommand{
            .close_inverter_switch = motors::GetInverterEnable(),
        });
        pt_can_bus.Send(motors::GetLeftSetpoints());
        pt_can_bus.Send(motors::GetRightSetpoints());

        vTaskDelayUntil(&wake_time, pdMS_TO_TICKS(10));
    }
}

int main(void) {
    Initialize();

    alerts::Reset();
    accumulator::Init();
    motors::Init();
    vehicle_dynamics::Init();

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
