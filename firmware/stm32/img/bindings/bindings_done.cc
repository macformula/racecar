// WARNING: The bindings.md article is heavily dependent on the exact line
// numbers. If you edit this file, you must update that document.

// ============================== CubeMX Includes ==============================
#include "adc.h"
#include "can.h"
#include "gpio.h"
#include "main.h"
#include "stm32f7xx_hal.h"

extern "C" {
// This requires extern since it is not declared in a header, only defined in
// cubemx/../main.c
void SystemClock_Config();
}

// ============================= Firmware Includes =============================
#include "../../bindings.hpp"
#include "mcal/stm32f/periph/analog_input.hpp"
#include "mcal/stm32f/periph/can.hpp"
#include "mcal/stm32f/periph/gpio.hpp"

// =============================== MCAL Namespace ==============================
namespace mcal {
using namespace mcal::periph::stm32f;
// --------- Define C++ peripheral objects from CubeMX generated code ----------

DigitalOutput tssi_red_en{TSSI_RED_SIG_GPIO_Port, TSSI_RED_SIG_Pin};
DigitalOutput tssi_green_signal{TSSI_GN_SIG_GPIO_Port, TSSI_GN_SIG_Pin};

AnalogInput dcdc_sense{&hadc1, ADC_CHANNEL_10};
AnalogInput suspension_travel3{&hadc1, ADC_CHANNEL_15};
AnalogInput suspension_travel4{&hadc1, ADC_CHANNEL_14};

CanBase veh_can_base{&hcan2};

}  // namespace mcal

// ============================ Bindings Namespace =============================
namespace bindings {
// ------------- Bind mcal peripherals to the bindings.hpp handles -------------
macfe::periph::DigitalOutput& tssi_red_en = mcal::tssi_red_en;
macfe::periph::DigitalOutput& tssi_green_en = mcal::tssi_green_en;

macfe::periph::AnalogInput& dcdc_sense = mcal::dcdc_sense;
macfe::periph::AnalogInput& suspension_travel3 = mcal::suspension_travel3;
macfe::periph::AnalogInput& suspension_travel4 = mcal::suspension_travel4;

macfe::periph::CanBase& veh_can_base = mcal::veh_can_base;

// ---------------- Implement other platform-specific functions ----------------
void Initialize() {
    HAL_Init();            // from cubemx/inc/stm32f7xx_hal.h
    SystemClock_Config();  // from cubemx/src/main.c

    // ------------------ Add additional initialization code -------------------
    MX_GPIO_Init();  // from cubemx/inc/gpio.h
    MX_ADC1_Init();  // from cubemx/inc/adc.h
    MX_CAN2_Init();  // from cubemx/inc/can.h
}

}  // namespace bindings
