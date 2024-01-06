#include "gpio.h"
#include "main.h"

extern void SystemClock_Config();

int main() {
  SystemClock_Config();
  HAL_Init();
  MX_GPIO_Init();

  while (1) {
    HAL_GPIO_WritePin(LedPin_GPIO_Port, LedPin_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(LedPin_GPIO_Port, LedPin_Pin, GPIO_PIN_RESET);
    HAL_Delay(600);

  }

  return 0;
}