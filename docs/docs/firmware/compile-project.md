# Compiling a Project

We have a CMake build system and Makefile front end. The Makefile expects 2 variables from the command line.

!!! note "Virtual Environment"

    These instructions assume that your [`cangen` virtual environment is active](dev-setup.md/#install-cangen).

## Makefile Variables

1. `#!bash PROJECT=<project-name>` is the name of the project folder relative to `firmware/projects`.
2. `#!bash PLATFORM=<platform-name>` is the name of the platform _folder_ relative to `firmware/projects/<project_name>/platforms/`.

For example, to compile the Blink project for the CLI platform, you would execute

```bash
make PROJECT=Demo/Blink PLATFORM=cli build
```

## Makefile commands

Including `build`, there are 5 (Phony) targets defined in the Makefile.

### `build` (default target)

Compile the sources using the CMake `build` command.

This generates an executable or binary artifact depending on the `PLATFORM` variable. The artifact will be placed in `firmware/build/<project-name>/<platform-name>`.

### `config`

Configure CMake and clangd for a project. This creates the build directory and a `compile_commands.json` file which is used by clangd.

> `config` is a prerequisite of `build`.

### `clean`

Deletes the project and platform build directory. Forces CMake to reconfigure.

### `deepclean`

This command only applies when `#!bash PLATFORM=stm32f767`. It deletes all "gitignored" files in the project's `cubemx/` directory, forcing CubeMX to regenerate all code.

### `st-flash`

Calls `build` then downloads the firmware to a connected stm device.

!!! tip
    You should use STM32CubeProgrammer instead of `st-flash`. See [Flashing Firmware](flashing/index.md).
