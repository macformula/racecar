# Architecture

Our firmware is separated in 2 layers: application and platform.

The __application__ layer describes the __what the project does__ at a high level. This includes:

- :octicons-cloud-16: Interacting with generic peripheral objects.
- :octicons-workflow-16: Task scheduling and program flow.
- :octicons-mail-16: Receiving and sending messages.

The __platform__ layer __configures hardware__ to run the app code. In this layer, you will find:

- :fontawesome-solid-mountain: Concrete peripheral implementations.
- :fontawesome-solid-gear: Peripheral configurations.
- :fontawesome-solid-power-off: Initialization functions.

The interface between the layers is a contract called __bindings__. The application specifies handles for each required peripheral and function, then the platform _binds_ peripheral and method implementations to these handles.

## Why separate these layers?

There are two major reasons why we strictly separate the application and platform code.

### Platform Abstraction

Since the application code is not tied to any specific platform, it can __run on _any_ platform__, provided that platform can support all necessary behaviour.

I __cannot overstate__ how significant this is. Being able to arbitrarily swap platforms enables:

- :octicons-terminal-16: Running vehicle firmware on your local machine through its command line interface.
- :material-test-tube: Testing application code with a HIL or SIL setup.
- :fontawesome-solid-microchip: Writing portable code, should we ever change our microcontroller.
- :fontawesome-solid-gears: Simultaneously having multiple hardware configurations for the same platform.

Platform abstraction means that the application code running on each device is the _exact_ same. This greatly simplifies debugging:

- If one platform has a bug but another works fine, then you immediately know that the former was configured incorrectly in the platform layer.
- If all platforms have the same bug, then you know that all hardware peripherals are responding and the bug exists at the app layer.

??? example "Debugging Example"
    Imagine you are creating a simple project to turn on an LED whenever a button is pressed (this is the purpose of the `Demo/BasicIO` project). There are four primary elements of this code:

    1. Read the button state (platform layer).
    2. Store the button state in a boolean variable (app).
    3. Set the LED with the boolean (platform).
    4. Loop to repeat this process indefinitely (app).

    Without platform abstraction, if the code "didn't work", then you would be wondering:

    - Does my button circuit work?
    - Is the variable being stored properly?
    - Did I choose the right GPIO pins for the button and LED?
    - Is the LED burnt out?
    - Did my program crash and fail to loop?

    Addressing all of these potential errors would lead you down a long road of circuit testing and code modifications, trying to find other ways to see if the LED is working of if there was a crash.

    Now suppose that you have used platform abstraction. When your project doesn't work, you can quickly configure a CLI platform to emulate the physical button and LED. The button state is "read" by prompting for a `true`/`false` input and the LED state is "set" by printing a message to the screen.

    When you run the program in the CLI, you may find everything works as expected. When you type `true`, the program prints "LED On" and `false` causes "LED Off", and the program loops as expected. This indicates the the physical hardware was configured incorrectly.

    Or, maybe the CLI always prints "LED Off" regardless of your input. Now you know that there is an error in the app code. You know to inspect the code because the error exists independent of the platform configuration. 

### Modular Logic

The project's application behaviour can be completely defined without knowdledge of the hardware setup, and vice-versa. This means we can develop, test, debug, and optimize a new project before we have the electrical system ready!

Alternatively, two developers can work in parallel: one concerned with configuring the hardware and the other with defining app level behaviour. The strict interface between the two layers means they can work independently without breaking each others' code.

## Example: TMS Layers

The TMS functionality is separated into these 2 layers (+ bindings) as follows.
> This is an example only and does not represent how the actual TMS is configured.

### Application Layer

- Read the battery temperature with analog sensor `temp_sensor`.
- Calculate a fan speed to cool the battery.
- Control a fan via `fan_control`.
- Generate a summary CAN message.
- Transmit the message over the `vehicle_can` bus.

### Bindings Contract

The application needs:

- ADC input `temp_sensor`
- PWM output `fan_control`
- CAN Bus `vehicle_can`

### Platform Layer

We can run and debug the TMS on any platform satisfying this contract.

=== "stm32f767"

    - `temp_sensor` = Channel 10 of ADC1
    - `fan_control` = Channel 1 of hardware timer 4
    - `vehicle_can` = CAN2

    Functions are executed through the stm32 HAL.

=== "SIL"

    - `temp_sensor` = gRPC analog input signal named `TempSensor`
    - `fan_control` = gRPC analog output signal named `FanSpeed`
    - `vehicle_can` = SocketCAN interface called `vcan0`
  
    The project runs on a Raspberry Pi.

=== "CLI"

    - `temp_sensor` = Numeric input from the user
    - `fan_control` = Print speed to the screen
    - `vehicle_can` = Print CAN message contents

    The project runs in the developers terminal.

## Implementation

Being statically typed, C++ requires that everything be defined tis architecture is implemented in C++ using class polymorphism.
