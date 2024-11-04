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

2. Plug your board into your computer and ensure it is powered on.

3. Start the OpenOCD server:

    ```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-openocd
    ```

4. In a separate terminal, start GDB:

    ```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-gdb
    ```

5. Start debugging in the GDB terminal. See the list of common commands and an example below.

### GDB Commands

These commands are executed directly in the GDB terminal.

`help`: Displays a list of available commands and their syntax/descriptions.

`load`: Loads the program into the target device's memory. The `debug-gdb` command has been configured to do this automatically on startup.

`continue`: Resumes program execution until the next breakpoint or end.

`finish`: Continues execution until the current function finishes, then stops.

`next`: Executes the next line of code, stepping over any function calls.

`step`: Executes the next line of code, stepping into any function calls.

`break`: Sets a breakpoint at a specified line or function to pause execution.

`info breakpoints`: Lists all active breakpoints and their details.

`where`: Displays the current call stack, showing the active function and its caller hierarchy.

Refer to [VisualGDB](https://visualgdb.com/gdbreference/commands/) and [GDB Cheat Sheet](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf) for more commands and details.

### Example

The following commands show an example debugging session with the Blink project.

```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-openocd
```

```bash
    make PROJECT=Demo/Blink PLATFORM=stm32f767 debug-gdb
```

```text
    (gdb) break main        # Set a breakpoint at the main function
    (gdb) continue          # Continue the execution of the program
    (gdb) where             # Display the current call stack
    (gdb) next              # Execute the next line of code
    (gdb) quit              # Exit GDB
```

## Resources

Refer to additional documentation for more information on debugging with [OpenOCD](https://openocd.org/doc/html/index.html) and [GDB](https://sourceware.org/gdb/current/onlinedocs/gdb/).
