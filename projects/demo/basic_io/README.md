# Demo Project

This project serves as a starting point for any new firmware project. It has all of the code and build system infrastructure ready and can be used to test your development environment.

To start a new firmware project, make a copy of the DemoProject folder within the same `projects/` folder, then rename the folder appropriately.

## Program Description

DemoProject is a simple program which polls a boolean input and sets a boolean output to match the input state.

Notice how `main.cc` describes this high-level behaviour without any platform-specific details. It is highly desirable to separate logic and implemenation as it allows us to test code on different platforms and isolate problems easier.

Part of the demo is to show how our architecture abstracts away the platform details.

> __Platform__ refers to the device running the program.

When built for the `stm32f767` platform, `PC13` is the input and `PB7` is the output. On the NUCLEO stm32f767zi development board, the input is the User button and the output is the blue LED, so the program can be tested without any external circuitry.

When built for `cli`, the digital input is a command line prompt and the digital output is a command line response.
