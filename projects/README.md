# Firmware Projects

See <https://macformula.github.io/racecar/firmware/project-structure/> for an explanation of the project folder structure.

## [FrontController](FrontController/)

Primary control system. Handles driver IO, vehicle dynamics, and motor control.

## [LVController](LVController/)

Power management board. Handles bootup and shutdown. Enables power to all subsystems, controls accumulator and inverter contactors, and controls electrical systems like the brake light and powertrain cooling.

## [Dashboard](Dashboard/)

Displays driver/event profile selection and runtime information like speed and accumulator state of charge. Primarily communicates with FrontController.

## [TMS](tms/)

Our simplest ECU, reads thermistors inside the accumulator to report temperatures to our off-the-shelf battery management system.

---

## [Demo/](Demo/)

This is a great place to start working with our firmware repo! The `Demo/` folder contains several projects which isolate features of our repo to serve as tutorials and examples.

Many Demo projects can be run locally in your CLI, so you can develop without having an STM32 board! In particular, check out the following projects:

| Demo Project | cli | linux | stm |
|:--|:---:|:--:|:--:|
| Blink | Y | Y | Y |
| BasicIO | Y | Y | Y |
| AnalogInput | Y | Y | Y |
| AnalogPWM | Y | Y | Y |
| Mappers | Y | | |
|CAN/Send,Receive | | Y | Y |
