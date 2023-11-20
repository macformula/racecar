/// @author Blake Freer
/// @date 2023-11-18

#include <cstdint>

#include "app.h"

/** @todo APP objects are instantiated in bindings.h -> this should be improved,
ideally the objects can be defined in main and instantiated in bindings.cc
The issue rn is the App objects are templated, such we cannot "import" them
with 'extern' without providing mcal peripheral type, which goes against the
point of abstracting from the mcal

The following lines of code would allow for an extern import of PushButton

---- main.cc ----
1 |  #include "app.h"
2 |  #include "mcal/stm32f767/periph/gpio.h"
3 |  extern PushButton<mcal::periph::DigitalInput> button;
 
---- bindings.cc ----
1 |  #include "app.h"
2 |  #include "mcal/stm32f767/periph/gpio.h"
3 |  PushButton button{mcal::periph::DigitalInput(ButtonPin_GPIO_Port, ButtonPin_Pin)};

The issue is obvious in line 2 and 3 of main.cc -> we should not have to include
the mcal implementation into main, but it is required to specify the template
type in the extern PushButton declaration.

--------------------------------------------------------------------------------

The current workaround uses bindings as a header .h file instead of a source .cc
file. This means that when it is included into main.cc, the App object
definitions exists as if they were written in main, except main.cc doesn't
explicity reference anything from mcal. This is a decent solution, since header
files can easily be switched out at runtime. My issue with this is that the APP
objects are not defined in main, so there is no way to guarantee that a
bindings.h file actually instatiates the correct objects with the corrent names.
**/
#include "TMS/mcal/stm32f767/bindings.h"


float adcToPwm(std::uint32_t adc_value) {
	// remap 12 bit adc to 0-100%
	return float(adc_value) / 4095.0f * 100.0f;
}

int main(void) {
    Initialize();

    fanController.StartPWM();

    std::uint32_t tempValue;

    while (true) {
		button.Read();
        light.SetLight(button.Read());

        tempValue = tempSensor.Read();
        fanController.Set(adcToPwm(tempValue));
    }

    return 0;
}