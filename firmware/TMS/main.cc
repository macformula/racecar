#include "project.h"

int main(void) {
	Initialize();


	while (true) {
		
		led.Set(btn.Read());

	}
	
	return 0;
}