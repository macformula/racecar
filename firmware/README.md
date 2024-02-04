# Firmware

This documentation is still a work-in-progress. Some project-specific documentation is available in the README of `projects/DemoProject`.

## Required Dependencies

The following dependecies must be installed __and added to your PATH variable.__

* GNU Make
  * On Windows, the recommended terminal is Git Bash which will include `make`
  * Verify with `make --version`
  
* CMake 3.27 or newer - [link](https://cmake.org/download/)
  * Verify with `cmake --version`

* arm-none-eabi toolchain 10.3-2021.10 - [link](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  * Verify with `arm-none-eabi-gcc --version`

* Java SE Runtime Environment 13.0.2 or newer - [link](https://www.oracle.com/java/technologies/javase/jdk13-archive-downloads.html)
  * Verify with `java --version`

* STM32CubeMX - [link](https://www.st.com/content/st_com/en/stm32cubemx.html)
  * Verify by running `stm32cubemx`. The application should open.
