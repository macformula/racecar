# Firmware Projects

See <https://macformula.github.io/racecar/architecture/software/> for an explanation of the project folder structure.

## [Front controller](front_controller/)

Primary control system. Handles driver IO, vehicle dynamics, and motor control.

## [LV controller](lvcontroller/)

Power management board. Handles bootup and shutdown. Enables power to all subsystems, controls accumulator and inverter contactors, and controls electrical systems like the brake light and powertrain cooling.

## [Dashboard](dashboard/)

Displays driver/event profile selection and runtime information like speed and accumulator state of charge. Primarily communicates with the front controller.

## [TMS](tms/)

Our simplest ECU, reads thermistors inside the accumulator to report temperatures to our off-the-shelf battery management system.

---

## [Demo/](demo/)

This is a great place to start working with our firmware repo! The `demo/` folder contains several projects which isolate features of our repo to serve as tutorials and examples.

Many Demo projects can be run locally in your CLI, so you can develop without having an STM32 board! In particular, check out the following projects:

| Demo Project | cli | linux | stm |
|:--|:---:|:--:|:--:|
| Blink | Y | Y | Y |
| BasicIO | Y | Y | Y |
| AnalogInput | Y | Y | Y |
| AnalogPWM | Y | Y | Y |
| Mappers | Y | | |
|CAN/Send,Receive | | Y | Y |
