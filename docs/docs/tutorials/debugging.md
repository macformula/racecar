# :material-bug: Hardware Debugging

## Background

For debugging our boards, we use OpenOCD and GDB.

[OpenOCD (Open On-Chip Debugger)](https://openocd.org/) is a tool that enables on-chip debugging, flash programming, and testing for microcontrollers. It will start a server that listens for commands from various sources such as GDB.

[GDB (GNU Debugger)](https://www.sourceware.org/gdb/) is a debugging tool that enables inspection, modification, and control of program execution on microcontrollers.

## Installation

Use the following command to install OpenOCD:

=== "Windows"
    ```text
    <!-- TODO: Rida documentation for OpenOCD -->
    ```

=== "Linux / Mac"
    ```text
    brew install openocd
    ```

It is likely the ARM GNU Toolchain containing GDB is already installed if you have followed the [Firmware Development Setup](../firmware/dev-setup.md) guide. Refer to this guide if it is not installed.

### Verify Installation

Check that both programs are installed and have an acceptable version.

Do not copy the `# version comments`.

```text
openocd --version               # >= 0.12.0
arm-none-eabi-gdb --version     # >= 13.0
```

## Usage

### Debugging

To debug a program, the OpenOCD server must be started and GDB connected to it.

You must have a compiled binary file to be able to load it onto the board for debugging. If you have not yet compiled your program, refer to the [Compiling a Project](../firmware/compile-project.md) guide.

Once you have compiled your program, plug in your board and run the following commands. They follow a similar structure to the `make` commands used for compiling.

!!! warning
    The OpenOCD command must be run before the GDB command.

    Each command should be run in a separate terminal.

```bash
make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-openocd
```

```bash
make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-gdb
```


### GDB Commands

Below are some common GDB commands used for debugging:

`load`: Loads the program onto the target device for debugging.

`continue`: Resumes program execution until the next breakpoint or end.

`finish`: Continues execution until the current function finishes, then stops.

`break`: Sets a breakpoint at a specified line or function to pause execution.

`info breakpoints`: Lists all active breakpoints and their details.

`next`: Executes the next line of code, stepping over any function calls.

`step`: Executes the next line of code, stepping into any function calls.

`where`: Displays the current call stack, showing the active function and its caller hierarchy.

Refer to the [VisualGDB](https://visualgdb.com/gdbreference/commands/) documentation for more commands.

## Resources

Refer to additional documentation for more information on debugging with [OpenOCD](https://openocd.org/doc/html/index.html) and [GDB](https://sourceware.org/gdb/current/onlinedocs/gdb/).