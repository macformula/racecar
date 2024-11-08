# Linux MCAL

A more advanced CLI. CAN messages are sent over virtual can (vcan) to enable true CAN communication between processes.

## Note on Namespace Name

The namespace is `mcal::lnx`, not `mcal::linux` as one might expect. This is because g++ defines `linux=1`, preventing `linux` from being used as an identifier.

## WSL Compatability

You can run this platform through the Windows Subsystem for Linux but must recompile your WSL kernel to include the CAN modules. See <https://macformula.github.io/racecar/tutorials/wsl-can/>.
