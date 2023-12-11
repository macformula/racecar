/// @author Blake Freer
/// @date 2023-12-11

#include <string>

#include "bindings.h"

// CubeMX imports
#include "TMS/app.h"
#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// Used in main.cc via extern ...
namespace bindings {
    mcal::periph::ADCInput temp_sensor_adc{&hadc1};
    mcal::periph::PWMOutput fan_controller_pwm{&htim1, TIM_CHANNEL_1};
} // namespace bindings

extern "C" {
void SystemClock_Config();
}

void Initialize() {
    SystemClock_Config();
    HAL_Init();
    MX_ADC1_Init();
    MX_TIM1_Init();
}

void Log(std::string message) {
    return;
}