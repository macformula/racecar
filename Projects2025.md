# Firmware / Software Project Descriptions

## Metrics and Observability

Data is key in a large scale software system like our electric vehicle. This is a new project for 2025-2026 so you will get to:

- Design a time series database and host it with cloud-computing
- Create Grafana dashboards to display real-time data
- Write scripts for a Raspberry Pi to wirelessly record sensor data from the running vehicle and push it to the database.

Ultimately, the dashboards and metrics will be used by all MAC FE members to study and debug the vehicle.

Relevant technologies: Databases, Grafana, Scripting (Go, Python), Raspberry Pi, Wireless Networking

## HIL & SIL

We have a Software-In-the-Loop (SIL) test system which simulates our firmware to validate its behaviour.

- Defining tests for all of our firmware controllers
- Adding features to the SIL app to improve its testing capabilities
- Working with the Metrics and Observability team to push test results to our database
- Working with the CI/CD team to run the SIL in GitHub actions to test incoming firmware PRs
- Turn the SIL into a HIL! The existing SIL code only performs virtual firmware simulations, but we can extend it to support Hardware-In-the-loop tests that run on actual circuits!

Relevant technologies: Go, C++, Serialization, Websockets, Matlab Simulink, Speedgoat, GitHub Actions

## Devops + CI/CD

Devops supports all of our projects by making it easy to develop and manage our code.

- Create GitHub actions to automatically compile and test incoming Pull Requests
- Manage our internal software devices (such as Raspberry Pis) by writing provisioning scripts and setting up / maintaining Linux servers.

Relevant technologies: GitHub, Linux, Scripting, Ansible, Networking

## Vehicle Firmware

There are 4 microcontrollers in our vehicle which all need C++ firmware to safely and accurately manage the vehicle subsystems, such as the motors, driver controls, battery, and lights. You will use FreeRTOS

- Use CAN to communicate statuses and control commands between multiple microcontrollers.
- Configure and use microcontroller peripherals like GPIO, ADC, Flash memory, and timers.
- Create intuitive dashboard screens to help the driver control the vehicle.
- Implement in-vehicle sensor calibration sequences.
- Support electral sub-team testing.

Relevant technologies: C++, CMake, STM32, Reading electrical schematics

## Over-the-Air Firmware Updates

Currently, to upload new firmware to a vehicle controller, we must physically plug a laptop into the controller. This is slow and difficult when the car is fully assembled, as some connctors are not readily accessible.

STM32 microcontrollers support CAN flash - a feature where you can upload new programs using the existing CAN communications wires already in the vehicle. Connect CAN to a WiFi enabled Raspberry Pi and you can perform wireless updates!

This project involves writing CAN commands to package and synchronize controllers on the CAN network, creating a server on the Raspberry Pi to accept new Firmware binaries, and creating a small frontend to make it easy to use.

Relevant technologies: C++, Linux, CAN, STM32 (including the bootloader), Frontend interface design
