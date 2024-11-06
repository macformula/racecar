# CANErrors

This is the CAN Errors demo project. This defines the mechanism for reporting errors from the vehicle microcontrollers. This project will send errors over the CAN line and process them on a host machine.

## Generating The Code
- Code must be generated using CanGen. Install CanGen in the racecar directory with "pip install -e scripts/cangen"
   - A venv isn't needed for this!
- Generate the code in the firmware directory with "cangen projects/Demo/CANErrors"