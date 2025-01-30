# Project Structure

<!-- Folder diagrams are created with https://tree.nathanfriend.com -->

A __project__ is a compilable program that will run on a single device. It should have one `main()` function. Each ECU in our vehicle has its own project.

The simplest project is Blink (see `projects/Demo/Blink`). This project toggles a digital output indefinitely, toggling the state every 1 second.

We will __recreate the Blink project from scratch__ for multiple platforms. You will learn how projects are structured, how to configure an CubeMX for the stm32f767 platform, and how to build and run a project.

    MyBlink/
    ├── README.md
    ├── main.cc
    ├── bindings.hpp
    ├── CMakeLists.txt
    └── platforms/
        ├── cli/
        │   ├── mcal_conf.cmake
        │   ├── bindings.cc
        │   └── CMakeLists.txt
        └── stm32f767/
            ├── mcal_conf.cmake
            ├── bindings.cc
            ├── CMakeLists.txt
            └── cubemx/
                └── board_config.ioc

!!! warning

    Knowledge of our firmware architecture is assumed. Read the [Architecture](../architecture/index.md) article before continuing.

!!! note

    Unless otherwise mentioned, all file paths are relative to `racecar/firmware`.

## Prepare the project folder

All projects are stored under the `projects/` directory, with each project organized in its own folder. Projects can also be grouped into subfolders for better organization. For example, the `FrontController` project is stored at `projects/FrontController/`, while demo projects can be grouped under `projects/Demo/`, such as the `Blink` project stored at `projects/Demo/Blink/`.

1. Create a new folder to hold the project at `projects/MyBlink`.

2. Add a `README.md` file explaining the purpose of this project.

    ```markdown title="projects/MyBlink/README.md"
    # MyBlink

    Recreate the `Demo/Blink` project by following the tutorial at
    <https://macformula.github.io/racecar/firmware/project-structure>.
    ```

3. Add a `platforms/` folder to the project. We will populate the platform implementations later.

You should now have the following directory structure.

    projects/
    └── MyBlink/
        ├── README.md
        └── platforms/

## Bindings contract

A key feature of our firmware architecture is the complete abstraction of the app-level code from any specific platform implementation (See the [Architecture](../architecture/index.md) article). The two layers are interfaced by the "bindings" contract which we will write first.

Create a new file `bindings.hpp` in the project directory. This is a header file since it only _declares_ the interface objects and methods rather than using or implementing them.

```c++ title="projects/MyBlink/bindings.hpp"
--8<--
inc/bindings.hpp:1:1

inc/bindings.hpp:5:5

inc/bindings.hpp:12:12
--8<--
```

!!! info

    `#!c++ #pragma once` Ensures the header file is only ever included once.

    We use the `bindings` namespace to indicate that the contained functions and objects form the interface between the app and platform layers.

We now add 3 elements to our contract.

1. A digital output which we will call `indicator`. Include the `shared/periph/gpio.hpp` to gain access to the app-level `DigitalOutput` class.

    ```c++ title="projects/MyBlink/bindings.hpp" hl_lines="3 7"
    --8<--
    inc/bindings.hpp:1:7

    inc/bindings.hpp:12:12
    --8<--
    ```

    > Notice the ampersand `&` in the type specifier. This makes `indicator` a _reference_ to a `DigitalOutput` object. This is necessary since `DigitalOutput` is a virtual class so its size-in-memory is not defined.

2. A function to wait between toggling. Time delay mechanisms are platform specific and thus must be included in the bindings contract. We will declare a `DelayMS` function which receives an `unsigned int` representing the number of milliseconds to delay for.

    ```c++ title="projects/MyBlink/bindings.hpp" hl_lines="9"
    --8<--
    inc/bindings.hpp:1:9

    inc/bindings.hpp:12:12
    --8<--
    ```

3. An initialization function. Some platforms must execute initialization code before they are ready to run. For example, stm32f767 has several `HAL_*_Init` functions which configure the peripherals. We include an `Initialize` function in bindings so that each platform can define its own behaviour.

    ```c++ title="projects/MyBlink/bindings.hpp" hl_lines="10"
    --8<-- "inc/bindings.hpp"
    ```

This completes the bindings contract. We can now write the application code, knowing that the platforms will all satisfy this contract.

## Application code

Our `main` method is very simple in this project:

1. Initialize the platform.
2. Turn `indicator` __on__.
3. Wait 1 second.
4. Turn `indicator` __off__.
5. Wait 1 second.
6. Return to step 2.

Create a `main.cc` file in the project folder.

```c++ title="projects/MyBlink/main.cc"
--8<-- "inc/main.cc"
```

In this simple project, all functions and peripherals are inside the `bindings` namespace. A more complex project could have app-level functions defined in `main.cc`.

The `SetHigh` and `SetLow` methods of `indicator` are declared in the virtual class under `shared/periph/gpio.hpp`.

### CMake Sources

Our build system needs help determining which source files to compile. This is configured using a `CMakeLists` file that adds our `main.cc` file as a source to the global `main` target.

```CMake title="projects/MyBlink/CMakeLists.txt"
target_sources(main PUBLIC main.cc)
```

This concludes the app-level code. Your project directory should look like

    projects/
    └── MyBlink/
        ├── README.md
        ├── bindings.hpp
        ├── main.cc
        ├── CMakeLists.txt
        └── platforms/

## Platform code

We will create a platform implementation for the command line interface and the stm32f767. Both of these platforms have an MCAL library under `racecar/firmware/mcal/`.

### Command Line Interface

Create a `cli` subfolder of `MyBlink/platforms`. We will satisfy the bindings contract in a `bindings.cc` file.

1. Configure the `indicator` output. The concrete peripheral implementation is provided by `mcal/cli/periph/gpio.hpp`. Include this header and create a peripheral.

    ```c++ title="projects/MyBlink/platforms/cli/bindings.cc"
    --8<--
    inc/cli/bindings.cc:6:6

    inc/cli/bindings.cc:9:13
    --8<--
    ```

    Now "bind" it to the app-level handle in the `bindings` namespace.

    ```c++ title="projects/MyBlink/platforms/cli/bindings.cc" hl_lines="1 3 11-15"
    --8<--
    inc/cli/bindings.cc:5:17
    inc/cli/bindings.cc:26:27
    --8<--
    ```

    > Again, note the ampersand `&` on the type specifier.

2. Implement `DelayMS`. Our CLI will use the POSIX `usleep` function from `unistd.h`.

    The `usleep` delay is measured in microseconds. Multiply our milliseconds parameter by 1000 to convert it to microseconds.

    ```c++ title="projects/MyBlink/platforms/cli/bindings.cc" hl_lines="1 17-19"
    --8<--
    inc/cli/bindings.cc:1:2
    inc/cli/bindings.cc:5:21
    inc/cli/bindings.cc:26:27
    --8<--
    ```

3. Implement the `Initialize` method. Our CLI does not need any initialization, but we can print a message to demonstrate that it is being executed.

    ```c++ title="projects/MyBlink/platforms/cli/bindings.cc" hl_lines="3 23-25"
    --8<-- "inc/cli/bindings.cc"
    ```

This completes the bindings implementation for the CLI.

#### CMake Configuration

We must add 2 files to help the build system find the correct sources.

```CMake title="projects/MyBlink/platforms/cli/CMakeLists.txt"
--8<-- "inc/cli/CMakeLists.txt"
```

The first line adds the `bindings.cc` file to the globally defined `bindings` target. The second links the required MCAL library to the target.

The second file tells CMake which MCAL library to include.

```CMake title="projects/MyBlink/platforms/cli/mcal_conf.cmake"
--8<-- "inc/cli/mcal_conf.cmake"
```

??? info "Historical Note"

    In 2023, the mcal was selected based on the platform folder name rather than by the `mcal_conf.cmake` file. However, this prevented us from defining two platform implementations using the same mcal, as the two folder names had to be unique but then couldn't reference the same mcal. `mcal_conf.cmake` was added to allow for this configuration.

> This conf file duplicates logic in the second line of `CMakeLists.txt`. Make a PR if you have a better solution.

Your `projects/MyBlink/platforms` directory should look like

    projects/MyBlink/platforms/
    └── cli/
        ├── bindings.cc
        ├── CMakeLists.txt
        └── mcal_conf.cmake

#### Test the CLI

!!! tip

    Read the [Compiling a Project](../compile-project.md) article for a full explanation of the compilation procedure.

To compile the MyBlink project with the CLI platform, open a terminal and navigate to the `racecar/firmware` directory. Run this command

```bash
make PROJECT=MyBlink PLATFORM=cli
```

This will compile the project to an executable which is stored in the `firmware/build/MyBlink/cli` directory.

Run the executable and see the indicator toggling every 1 second.

```bash
$ ./build/MyBlink/cli/main.exe
Initializing the CLI...
Setting DigitalOutput Channel Indicator to true
Setting DigitalOutput Channel Indicator to false
Setting DigitalOutput Channel Indicator to true
Setting DigitalOutput Channel Indicator to false
# repeats forever
```

### stm32f767 Platform

See [Create a CubeMX Project](../cubemx-project/index.md).
