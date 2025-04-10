# firmware

## Compiling

First, make sure you have [set up your development environment](https://macformula.github.io/racecar/firmware/dev-setup/) and [activated your `cangen` environment](https://macformula.github.io/racecar/firmware/dev-setup/#install-cangen).

```bash
make PROJECT=<PROJECT NAME> PLATFORM=<PLATFORM NAME> [build, clean, deepclean]
```

Where `<PROJECT NAME>` is the name of a folder under `firmware/projects` and `<PLATFORM NAME>` is the name of a folder in the project's `platforms` directory.

- `build` compiles to a binary. This is the default, so omitting a target will build.
- `clean` deletes the build folder.
- `deepclean` removes the generated CubeMX files (forces CubeMX regeneration), as well as performing `clean`.

The output is placed in `build/<PROJECT NAME>/<PLATFORM NAME>`. When compiled for STM32, the binary is named `PROJECT NAME.bin`, while for other platforms it is just `main` or `main.exe`.

_For more detail, see <https://macformula.github.io/racecar/firmware/compile-project/>_

## Flashing to an ECU

See <https://macformula.github.io/racecar/firmware/flashing/>.

## Navigation

_For a complete description of the `firmware/` architecture, see <https://macformula.github.io/racecar/firmware/architecture/>._

### [projects/](projects/)

Each "project" is a program which runs on a single ECU. See <https://macformula.github.io/racecar/firmware/project-structure/>.

### [mcal/](mcal/)

Concrete peripheral implementations which enable different platforms to run a project.

### [shared/](shared/)

Platform-independent utilities and interfaces used across multiple projects.

### [cmake/](cmake/)

Helper scripts and CMake functions assist our build system.
