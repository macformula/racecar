# Firmware

This documentation is still a work-in-progress. Some project-specific documentation is available in the README of `projects/DemoProject`.

## Required Dependencies

The following dependecies must be installed __and added to your PATH variable.__

* GNU Make
  * On Windows, the recommended terminal is Git Bash which will include `make`
  * Verify with `make --version`
  
* CMake 3.27 or newer - [link](https://cmake.org/download/)
  * Verify with `cmake --version`

* arm-none-eabi toolchain 13.2.Rel1 - [link](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  * Verify with `arm-none-eabi-gcc --version`

* STM32CubeMX 6.11.0 - [link](https://www.st.com/en/development-tools/stm32cubemx#get-software)
  * Verify by running `stm32cubemx`. The application should open.

* Java SE Runtime Environment 17 or newer - [link](https://www.oracle.com/java/technologies/java-se-glance.html)
  * This is a dependency of CubeMX.
  * If you don't already have a compliant version installed, get the latest version.
  * Verify java with `java --version`.
  * Ensure `java -jar <path/to/STM32CubeMX.exe>` opens CubeMX.

## sil

* gRPC c++ dependancies - [link](https://grpc.io/docs/languages/cpp/quickstart/)
  * Follow the guide to build the example project.
  * Add the `CMAKE_PREFIX_PATH` environment variable to your machine
    * `export CMAKE_PREFIX_PATH="$HOME/.local"`
  

## clangd

This is our official C/C++ language server. It is not required but will make your development experience _much_ nicer. We have configured CMake to autogenerate the build configuration so that you never need to configure include paths or compiler flags.

Whenever you build a project with the Makefile, __clangd__ will see the new `build/compile_commands.json` and immediately update the IDE's include paths. This means that when you switch which project or platform you are developing for, simply build the project and your development environment will be automatically prepared.

Install clangd 16.0.2 - [link](https://github.com/clangd/clangd/releases/tag/16.0.2)

* You must use this exact version.
* Download and extract `clang-<platform>-16.0.2.zip`
* Verify with ``clangd --version``.

### clangd Setup (VS Code)

0. Install clangd (see above).
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
