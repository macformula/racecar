# Firmware Demo Project: Digital Analog Converter 🎛️

## System Overview

### Components
**Digital Inputs:** ⏯️
- `button_increment`: Increments the value of the selected analog output.
- `button_decrement`: Decrements the value of the selected analog output. 
- `button_confirm`: Confirms the current value and moves to the next analog output.

**Digital Outputs:** 🚨🚨
- `led_position0`: Indicates the least significant bit (LSB) of the current analog output position.
- `led_position1`: Indicates the most significant bit (MSB) of the current analog output position.

**PWM Output:** 🎚️ ⚡ 
- `pwm_output0`: Reflects the current value of the selected analog output as a PWM duty cycle.

**Digital Analog Converter:** 🎛️
- `dac0`: A group containing the four analog outputs, which can be simultaneously updated.

## Operation
**1. Initialization** 🚀
   - All analog outputs are initialized to a default value of 0.0V
   - The PWM output reflects the value of the first analog output (analog_output0)
   - The LEDs indicate the current analog output position (e.g., 00 for analog_output0)

**2. Increment/Decrement** ⬆️ ⬇️
   - Press button_increment to increase the value of the selected analog output
   - Press button_decrement to decrease the value of the selected analog output
   - The PWM duty cycle updates in real-time to reflect the current value

**3. Confirm and Move to Next Output** ✅ ➡️
   - Press button_confirm to lock in the current value and move to the next analog output
   - The LEDs update to indicate the new position (e.g., 01 for analog_output1)

**4. Voltage Output** 🔌
   - After setting all four analog outputs, the system simultaneously loads the values into the analog_output_group

## System Diagram

```
+----------------+ +------------------+------------------+ 
| button_confirm | | button_increment | button_decrement | 
+----------------+ +------------------+------------------+ 
         |                 |                              
         v                 v              
+-------------------------------+  +---------------------+
| Control Logic                 |  |  dac0               |
| - Incr/Decr selected output   |  |  - analog_output0   |
| - Update PWM duty cycle       |->|  - analog_output1   |
| - Update LED position         |  |  - analog_output2   |
| - Update analog_output_group  |  |  - analog_output3   |
+-------------------------------+  +---------------------+
         |                 |  
         v                 v                         
+------------------+ +----------------+----------------+
|  pwm_output0     | |  led_position1 |  led_position0 |
| (PWM Duty Cycle) | | (MSB Position) | (LSB Position) |
+------------------+ +----------------+----------------+
```                                      






