# Firmware

## Building

A simple CMakeLists has been prepared. It only supports TMS on the stm32f767 board. There are many improvements to be made, but it works as of its initial commit.

To build, open Git Bash and `cd` into `.../racecar/firmware`

Run `make build` to generate the cmake files and compile form them. The output (TMS.elf) will be stored in `TMS/build`.

Run `make clean` to remove the build directory.