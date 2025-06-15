# Architecture

Code in our monorepo is organized based on two binary traits: Project (ECU) Specificity and Platform Specificity.

![Architecture Organization](arch_diagram-Light.png#only-light){width=100%}
![Architecture Organization](arch_diagram-Dark.png#only-dark){width=100%}

Most of the architecture is described by the interplay in project-specific sections, i.e. the left column of this diagram.

Our project-specific firmware is separated in 2 layers:

The __application__ (app) layer describes the __what the project does__ at a high level. This includes:

- :octicons-cloud-16: Interacting with generic peripheral objects.
- :octicons-workflow-16: Task scheduling and program flow.
- :octicons-mail-16: Receiving and sending messages.

The __platform__ layer __configures hardware__ to run the app code. In this layer, you will find:

- :fontawesome-solid-mountain: Concrete peripheral implementations.
- :fontawesome-solid-gear: Peripheral configurations.
- :fontawesome-solid-power-off: Initialization functions.

The interface between the app and platform layers is a contract called __bindings__. This contract declares a handle for each peripheral and function required by the application. The platform *binds* a configured peripheral or function implementation to each handle.

## Why separate these layers?

There are two major motivations for strictly separating the application and platform code.

### Motivation 1: Platform Abstraction

Since the application code is not tied to any specific platform, it can __run on any platform__, provided that platform can fulfill the bindings contract.

I *cannot overstate* how significant this is. Being able to arbitrarily swap platforms enables:

- :octicons-terminal-16: Running vehicle firmware on your local machine through its command line interface.
- :material-test-tube: Testing application code with a HIL or SIL setup.
- :fontawesome-solid-microchip: Writing portable code, should we ever change our microcontroller.
- :fontawesome-solid-gears: Simultaneously having multiple hardware configurations for the same platform.

Platform abstraction means that each device executes the *exact same* application code. This greatly simplifies debugging:

- If there is a bug when running the vehicle but the test platform works fine, then you immediately know that the vehicle platform layer was configured incorrectly.
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

    Or, maybe the CLI always prints "LED Off" regardless of your input. Now you know that there is an error in the app code. You inspect the app code because the error is independent of the platform configuration. 

### Motivation 2: Modular Logic

Application behaviour can be completely defined without knowledge of the hardware setup, and vice-versa. This means we can develop, test, debug, and optimize a new project before we have the electrical system ready!

Alternatively, two developers can work in parallel: one concerned with configuring the hardware and the other with defining app level behaviour. The strict interface between the two layers means they can work independently without breaking each others' code.

## Implementation

!!! note

    Unless otherwise mentioned, all paths are relative to the `racecar/firmware/` directory.

### Shared Peripheral Interfaces

Each peripheral has a platform-independent interface defined as an [Abstract Class](https://learn.microsoft.com/en-us/cpp/cpp/abstract-classes-cpp?view=msvc-170). This class declares the high-level methods that the application can call on the peripheral. The methods are declared pure virtual and thus have no definition.

These peripheral definitions are located in [`shared/periph`](https://github.com/macformula/racecar/tree/main/firmware/shared/periph).

### Platform Peripheral Implementations

Each platform defines a class that inherits from this abstract peripheral class. It must provide a definition for each virtual function defined in the parent class. This child class can use platform-specific logic to accomplish the high-level goal.

The peripheral implementations are in the MCAL (Microcontroller Abstraction Layer) folder [`mcal/<platform-name>/periph/`](https://github.com/macformula/racecar/tree/main/firmware/mcal).

??? example "Interface Example"

    Let us create a simple `DigitalInput` abstract class and implement it for the `stm32f767` and `CLI` platforms.

    At the app level, `DigitalInput` has just a single method `Read()`. This method takes no parameters and should return a boolean indicating the state of the input.

    ```c++ title="shared/periph/gpio.hpp"
    #pragma once

    class DigitalInput {
    public:
        virtual bool Read() = 0;
    }
    ```

    The `virtual` specifier _allows_ the `Read` method to be overridden while the `= 0` syntax _requires_ the child class to override it by making it a "pure" virtual method.

    To implement this peripheral on the platforms, we inherit from the shared class and override `Read`.

    === "stm32f767 Platform"

        Digital inputs are read using the stm `HAL_GPIO_ReadPin` function. This function accepts a port and pin number and returns `true` (`false`) if the port pin input is HIGH (LOW).
        
        We do not specify a port or pin in this class. They are provided when an object is constructed in the project's platform layer, allowing this class to be reused for any stm32f767 digital input.

        ```c++ title="mcal/stm32f767/periph/gpio.hpp"
        #pragma once

        #include <cstdint>
        #include "shared/periph/gpio.hpp"
        #include "stm32f7xx_hal.h"

        namespace mcal::stm32f {

        class DigitalInput : public shared::periph::DigitalInput {
        public:
            DigitalInput(GPIO_TypeDef* gpio_port, uint16_t pin)
                : port_(gpio_port), pin_(pin) {}

            bool Read() override {
                return HAL_GPIO_ReadPin(port_, pin_);
            }

        private:
            GPIO_TypeDef* port_;
            uint16_t pin_;
        };
        }  // namespace mcal::stm32f
        ```

    === "CLI Platform"

        A command line interface does not have physical pins to read but the digital input behaviour can be implemented by prompting the user for a boolean input.

        ```c++ title="mcal/cli/gpio.hpp"
        #pragma once

        #include <iostream>
        #include <string>
        #include "shared/periph/gpio.hpp"

        namespace mcal::cli {

        class DigitalInput : public shared::periph::DigitalInput {
        public:
            DigitalInput(std::string name) : name_(name) {}

            bool Read() override {
                int value;
                std::cout << "Reading DigitalInput " << name_ << std::endl;
                std::cout << " | Enter 0 for False, 1 for True: ";
                std::cin >> value;
                std::cout << " | Value was " << (value ? "true" : "false") << std::endl;
                return value;
            }
        
        private:
            std::string name_;
        };

        }  // namespace mcal::cli
        ```

        We allow the developer to give a name to each digital input during construction. This name is included in the input prompt.

        Suppose a `DigitalInput` object is constructed with the name `"UserButton"`. When `Read()` is called for this `CLI` platform, the user is prompted to enter ++0++ or ++1++.

        ```console
        Reading DigitalInput UserButton
        | Enter 0 for False, 1 for True: 1
        | Value was true
        ```

    With this structure, the developer can write platform-agnostic app level code using the shared `DigitalInput` interface, knowing that both platforms have a matching implementation.

Both the peripheral interface and implementations can be used by multiple projects. To see how they are used in project-specific code, continue to [Project Structure](../project-structure/index.md).

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

    The project runs in the developer's terminal.
