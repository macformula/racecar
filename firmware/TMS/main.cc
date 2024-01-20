/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"
#include "cmsis_os.h"

#include "TMS/mcal/stm32f767/bindings.h"

extern PushButton<mcal::periph::DigitalInput> button;
extern Indicator<mcal::periph::DigitalOutput> light;
extern FanContoller<mcal::periph::PWMOutput> fanController;
extern TempSensor<mcal::periph::ADCInput> tempSensor;

float tempToPWM(uint32_t adc_value) {
    // remap 12 bit adc to 0-100%
    return float(adc_value) / 4095.0f * 100.0f;
}

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};

void StartDefaultTask(void *argument);

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle2;
uint32_t defaultTaskBuffer2[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock2;
const osThreadAttr_t defaultTask_attributes2 = {
  .name = "defaultTask2",
  .cb_mem = &defaultTaskControlBlock2,
  .cb_size = sizeof(defaultTaskControlBlock2),
  .stack_mem = &defaultTaskBuffer2[0],
  .stack_size = sizeof(defaultTaskBuffer2),
  .priority = (osPriority_t) osPriorityNormal,
};

void StartDefaultTask2(void *argument);


void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}
// end remove region

using led_pin = mcal::gpio::pin<mcal::gpio::port::b,
							  mcal::gpio::pin_num::p7,
							  mcal::gpio::mode::output_pp,
							  mcal::gpio::pull::nopull>;
using led2_pin = mcal::gpio::pin<mcal::gpio::port::b,
							  mcal::gpio::pin_num::p14,
							  mcal::gpio::mode::output_pp,
							  mcal::gpio::pull::nopull>;

using btn_pin = mcal::gpio::pin<mcal::gpio::port::c,
								mcal::gpio::pin_num::p13,
								mcal::gpio::mode::input,
								mcal::gpio::pull::down>;

infra::output::DigitalOutputPin<led_pin> led;
infra::output::DigitalOutputPin<led2_pin> led2;
infra::input::DigitalInputPin<btn_pin> btn;

int main(void) {


    led.Init();
    led2.Init();
    btn.Init();
      /* Init scheduler */
    osKernelInitialize();
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
    defaultTaskHandle2 = osThreadNew(StartDefaultTask2, NULL, &defaultTask_attributes2);
    /* Start scheduler */
    osKernelStart();

	while (true) {
		

	}
	
	return 0;
}

void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */

int status = 1;
  for(;;)
  {
    led.Set(status);
    status = !status;
    osDelay(500);
  }
  /* USER CODE END 5 */
}

void StartDefaultTask2(void *argument)
{
  /* USER CODE BEGIN 5 */
  uint32_t tempValue;

  /* Infinite loop */
int status = 0;
  for(;;)
  {
    led2.Set(status);
    status = !status;
    osDelay(500);
  }
  /* USER CODE END 5 */
}
