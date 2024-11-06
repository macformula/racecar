# CANErrors

This is the CAN Errors demo project. This defines the mechanism for reporting errors from the vehicle microcontrollers. This project will send errors over the CAN line and process them on a host machine.

## Generating The DBC Code
- Code must be generated using CanGen. Install CanGen in the racecar directory with "pip install -e scripts/cangen"
   - A venv isn't needed for this!
- Generate the code in the firmware directory with "cangen projects/Demo/CANErrors"

## Building the Project
- To build the project, run the following in the firmware directory "make PROJECT=Demo/CANErrors PLATFORM=cli"
   - This should create a build file with no errors!
- To execute the new build, run "./build/Demo/CANErrors/cli/main.exe"