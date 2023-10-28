#include "input/digital_input_pin.h"
#include "gpio.h"

using mypin = mcal::gpio::pin<mcal::gpio::port::a,
							  mcal::gpio::pin_num::p10,
							  mcal::gpio::mode::input,
							  mcal::gpio::pull::up>;

infra::input::DigitalInputPin<mypin> pin;

int main(void) {

	mypin::init();

	pin.read();
	// mypin::set();
	
	return 0;
}