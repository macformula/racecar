# Development Environment Setup

- [Development Environment Setup](#development-environment-setup)
  - [Dependencies](#dependencies)
    - [GNU Make](#gnu-make)
    - [CMake](#cmake)
    - [Arm GNU Toolchain](#arm-gnu-toolchain)
    - [STM32CubeMX](#stm32cubemx)
    - [Java Runtime Environment](#java-runtime-environment)
    - [STM32CubeProgrammer](#stm32cubeprogrammer)
    - [clangd](#clangd)
  - [IDE Setup](#ide-setup)
    - [VSCode](#vscode)

Follow these steps to begin developing in `racecar/firmware`.

## Dependencies

The Windows instructions assume that you have installed [Chocolatey](https://chocolatey.org/install).

### GNU Make

__Version__ 4.0 or newer

- Windows: `choco install make`
- Linux: `sudo apt-get install build-essential`

Verify with `make --version`

### CMake

__Version__ 3.27 or newer

- Windows: `choco install cmake`
- Linux: `sudo apt-get install cmake`
  - If this installs an old version (as it did for my Ubuntu 20.04 LTS), you may need to follow the steps at <https://apt.kitware.com/> to use the APT repository hosted by the developers of CMake.

Verify with `cmake --version`

### Arm GNU Toolchain

__Version__ 13.2.rel1 or newer.

Install from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>

Choose an installer from the "AArch32 bare-metal target (arm-none-eabi)" section. Ensure the executables are available on your PATH.  

Verify with `arm-none-eabi-gcc --version`

### STM32CubeMX

__Version__ _exactly_ 6.12.0

> You must install this version __exactly__. Using a different version will cause issues when opening files.

Install from <https://www.st.com/en/development-tools/stm32cubemx.html#st-get-software>

### Java Runtime Environment

__Version__ 17 or newer

- Windows: No installation needed as it is installed with CubeMX
- Linux: `sudo apt install openjdk-17-jre-headless`
  - See <https://github.com/macformula/racecar/issues/142> regarding removing this dependency.

### STM32CubeProgrammer

__Version__ 2.16.0 or newer

Install from <https://www.st.com/en/development-tools/stm32cubeprog.html>

### clangd

This is our official C/C++ language server. It is not required but will make your development experience much nicer. We have configured CMake to autogenerate the build configuration so that you never need to configure include paths or compiler flags.

Whenever you build a project with the Makefile, clangd will see the new build/compile_commands.json and immediately update the IDE's include paths. This means that when you switch which project or platform you are developing for, simply build the project and your development environment will be automatically prepared.

__Version__ _exactly_ 16.0.2. See <https://github.com/macformula/racecar/issues/93>

Download from <https://github.com/clangd/clangd/releases>. Place the binary somewhere convenient and reference it in your IDE (see [IDE Setup](#ide-setup)).

## IDE Setup

_If you use a different IDE, consider making a PR to add instructions for setting it up!_

### VSCode
