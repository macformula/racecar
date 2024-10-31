# :material-bug: Hardware Debugging

## Background

For debugging our boards, we use OpenOCD and GDB.

[OpenOCD (Open On-Chip Debugger)](https://openocd.org/) is a tool that enables on-chip debugging, flash programming, and testing for microcontrollers. It will start a server that listens for commands from various sources such as GDB.

[GDB (GNU Debugger)](https://www.sourceware.org/gdb/) is a debugging tool that enables inspection, modification, and control of program execution on microcontrollers.

## Installation

=== "Windows"
    ```text
    choco install openocd
    ```

=== "Linux"
    ```text
    sudo apt-get install openocd
    ```

=== "Mac"
    ```text
    brew install openocd
    ```

The ARM GNU Toolchain containing GDB is already installed if you followed the [Firmware Development Setup](../firmware/dev-setup.md) guide. Refer to that if it is not installed.

### Verify Installation

Check that both programs are installed and have an acceptable version.

Do not copy the `# version comments`.

```text
openocd --version               # >= 0.12.0
arm-none-eabi-gdb --version     # >= 13.0
```

## Usage

To debug a program, the OpenOCD server must be started and GDB connected to it.

1. Build your project for the `stm32f767` platform. See [Compiling a Project](../firmware/compile-project.md) for more details:

    ```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 build
    ```

2. Start the OpenOCD server:

    ```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-openocd
    ```

3. In a separate terminal, start GDB:

    ```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-gdb
    ```

4. Start debugging in the GDB terminal. See the list of common commands below.

### GDB Commands

Below are some common GDB commands used for debugging:

`load`: Loads the program into the target device's memory.

`continue`: Resumes program execution until the next breakpoint or end.

`finish`: Continues execution until the current function finishes, then stops.

`break`: Sets a breakpoint at a specified line or function to pause execution.

`info breakpoints`: Lists all active breakpoints and their details.

`next`: Executes the next line of code, stepping over any function calls.

`step`: Executes the next line of code, stepping into any function calls.

`where`: Displays the current call stack, showing the active function and its caller hierarchy.

Refer to [VisualGDB](https://visualgdb.com/gdbreference/commands/) and [GDB Cheat Sheet](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf) for more commands and details.

## Resources

Refer to additional documentation for more information on debugging with [OpenOCD](https://openocd.org/doc/html/index.html) and [GDB](https://sourceware.org/gdb/current/onlinedocs/gdb/).
