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

## Compiling this Project

A master CMake build system is used for all `racecar/firmware` projects, including DemoProject. Therefore, the following instructions will also work for any project.

1. In a terminal window, navigate to `racecar/firmware`
2. Run the following command.

    ```console
    make PROJECT=<project> PLATFORM=<platform>
    ```

    Replace `<project>` with one of the folder names under `racecar/firmware/projects`, in this case use `DemoProject`.

    Replace `<platform>` with one of the available platforms, such as `cli` or `stm32f767`.

    The `make` command calls CMake with the necessary parameters.
3. The project will build. The output will be placed in `racecar/firmware/build/DemoProject/<platform>`

## Running / Flashing the Project

This step is dependent on the platform. It assumes you have already compiled for the desired platform using the previous instructions.

### CLI (Windows / Unix)

The build process generates a `main.exe` executable in the build folder listed above. Simply execute this file and the program will run in your terminal.

### stm32f767

The build process generates a `main.bin` binary file which must be flashed to the board. To flash, execute the same `make` command as above and append `st-flash`

```console
make PROJECT=DemoProject PLATFORM=stm32f767 st-flash
```

## Cleaning Build Files

Sometimes you may want to perform a clean build where all sources are rebuilt. To clear the build folder for a specific project and platform, execute

```console
make PROJECT=DemoProject PLATFORM=<platform> clean
```

This will delete the `build/DemoProject/<platform>` folder.

Optionally, you can omit the `PLATFORM` flag to delete the entire `build/DemoProject` folder, or omit both flags and simply call `make clean` to delete all build files for all projects and platforms.

The `build` folder is not tracked by git so you can delete it anytime.

## Troubleshooting

Ensure you have all dependencies installed. See `racecar/firmware/README.md` for a complete list of dependencies.
