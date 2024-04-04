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

* STM32CubeMX 6.8.1 - [link](https://www.st.com/en/development-tools/stm32cubemx)
  * Select the 6.8.1 version in the __Get Software__ section.
  * Verify by running `stm32cubemx`. The application should open.

* clangd 16.0.2 - [link](https://github.com/clangd/clangd/releases/tag/16.0.2)
  * Download and extract `clang-<platform>-16.0.2.zip`
  * This is our "official" C/C++ language server.
  * Verify with ``clangd --version``.

## __clangd__ Setup

1. Install the __clangd__ vscode extension.

   * If you have the Microsoft __C/C++__ extension installed, disable it for the `racecar` workspace.

2. Go to the __clangd__ extension settings and provide the full path to `clangd.exe`. This execuable is in the `bin/` subdirectory of the extracted folder.

3. Create an empty file named `.clangd` in the `firmware/` directory.
   * This file indicates the "root" directory of the project, so it will look for a `compile_commands.json` file in `firmware/build/`. This json file is created by CMake in the project build directory (ex `build/DemoProject/stm32f767`) and copied to `build/` by the Makefile.
   * You may optionally configure __clangd__ with this file. See <https://clangd.llvm.org/config>
   * On windows, I had to use the following configuration to help __clangd__ find system header files.

      ```yaml
      CompileFlags:
        Add: --target=x86_64-w64-mingw64
      ```

   * On Mac M series chips, the following configuration should be used.

      ```yaml
        CompileFlags:
          Add: --target=arm64-apple-darwin22.6.0
      ```

### Using __clangd__

Whenever you build a project with the Makefile, __clangd__ will see the new `build/compile_commands.json` and immediately update the IDE's include paths. This means that, when switching which project or platform you are developing for, simply build the project and your development environment will be automatically prepared.
