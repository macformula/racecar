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

It is likely the ARM GNU Toolchain is already installed if you have followed the [Firmware Development Setup](../firmware/dev-setup.md) guide. Refer to this guide if it is not installed.

### Verify Installation

Check that both programs are installed and have an acceptable version.

Do not copy the `# version comments`.

```text
openocd --version               # >= 0.12.0
arm-none-eabi-gdb --version     # >= 13.0
```

## Usage

### Debugging

<!-- TODO: TBD once we decide on terminal or script usage -->

Terminal 1:
```text
openocd -f interface/stlink.cfg -f target/stm32f7x.cfg
```

Terminal 2:
```text
arm-none-eabi-gdb your_program.elf -ex "target remote :3333"
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

### Example

Here is an example of debugging a program using GDB:

<!-- TODO: TBD once we decide on terminal or script usage -->

## Resources

Refer to additional documentation for more information on debugging with [OpenOCD](https://openocd.org/doc/html/index.html) and [GDB](https://sourceware.org/gdb/current/onlinedocs/gdb/).