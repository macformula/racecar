# CAN Demo Projects

Control an LED with a button over CAN.

The `Send` node reads a button input then sends the input over CAN to the `Receive` node which then sets turns on an LED if the button is pressed.

## DBC

The DBC file is very simple. It contains a single message `ButtonStatus` with two fields:
- Byte 0: Button state (boolean)
- Byte 1-2: Message number (uint16)

C++ code for the DBC is generated using `scripts/cangen`. The code is placed into `Send/generated/can/` and `Receive/generated/can/`.

## Platforms

### Linux

The projects communicate over virtual CAN (vcan). If using WSL, you need to recompile your kernel to enable vcan (instructions coming soon).

Run `source vcan_setup.sh` to load the required modules.

Run the `Send` program in one shell and `Receive` in another. `Send` will read the "button" state by prompting you for a 1 or 0, then will send the state over vcan to `Receive` which will turn on the "led" by printing its new state to the screen.

```bash
$ ./build/Demo/CAN/Send/linux/main
Initializing Linux
Opened vcan0 at index 4
Reading DigitalInput "Button"
| Enter 0 for False, 1 for True: 1
| Value was true
CanBase vcan0: Sending
| [3AC] 01 01 00 00 00 00 00 00
Reading DigitalInput "Button"
| Enter 0 for False, 1 for True: 0
| Value was false
CanBase vcan0: Sending
| [3AC] 00 02 00 00 00 00 00 00
```

```bash
$ ./build/Demo/CAN/Receive/linux/main
Initializing Linux
Opened vcan0 at index 4
CanBase vcan0: Received
| [3AC] 01 01 00 00 00 00 00 00
DigitalOutput "Indicator" => true
CanBase vcan0: Received
| [3AC] 00 02 00 00 00 00 00 00
DigitalOutput "Indicator" => false
```