# Interrupt Handling Project

This project demonstrates basic interrupt handling in C++ for CLI platforms, showcasing how to manage asynchronous events like `Ctrl + C` using signal interrupts.

## Key Parts:
- **Signal Setup**: Configures the signal handler for system interrupts such as `Ctrl + C` (SIGINT).
- **Interrupt Handler**: Captures and processes the signal, allowing the application to respond without exiting.
- **Application Logic**: Runs a loop that toggles a state variable when an interrupt occurs.

## Functionality:
1. **Signal Handling**: Listens for `Ctrl + C` (SIGINT) and triggers a handler without terminating the program.
2. **State Toggling**: Toggles a `volatile` state variable upon receiving the interrupt, demonstrating responsiveness.

## How to Use:
1. **Review the Signal Setup**: Check `bindings.cc` for the signal handler that manages `Ctrl + C`.
2. **Modify `AppLevelFunction`**: Customize behavior in `bindings.cc` for how the program reacts to interrupts.
3. **Enable Interrupt Handling**: Include `bindings.h` in your main file and call `bindings::setup_interrupt()`.


