/// @author Blake Freer
/// @date 2023-11-08

#ifndef TMS_PROJECT_H_
#define TMS_PROJECT_H_

#include "infra/gpio/digital_input.h"
#include "infra/gpio/digital_output.h"

extern void Initialize();

extern infra::gpio::DigitalInput& btn;
extern infra::gpio::DigitalOutput& led;

#endif