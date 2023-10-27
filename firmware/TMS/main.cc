#include "mcal_input/digital_input_pin.h"
#include "gpio.h"

using mypin = mcal::gpio::pin<mcal::gpio::port::a,
							  mcal::gpio::pins<10>,
							  mcal::gpio::mode::af_od,
							  mcal::gpio::pull::up>;

mcal::input::DigitalInputPin<mypin> pin;

int main(void) {

	pin.init();

	pin.read();
	// mypin::set();
	
	return 0;
}