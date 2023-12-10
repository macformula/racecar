#include "bindings.h"

// CubeMX imports
#include "TMS/app.h"
#include "adc.h"
#include "gpio.h"
#include "main.h"
#include "tim.h"

// Used in main.cc via extern ...
namespace bindings {
    mcal::periph::DigitalInput button_di{ButtonPin_GPIO_Port, ButtonPin_Pin};
    mcal::periph::DigitalOutput light_do{LedPin_GPIO_Port, LedPin_Pin};
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
    MX_GPIO_Init();
    MX_TIM1_Init();
}