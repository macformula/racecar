# CAN Demo Projects

Control an LED with a button over CAN.

The `Send` node reads a button input and controls the `Receive` node's LED over CAN.

## DBC

The DBC file is very simple. It contains a single message `ButtonStatus` with one signal fields:

- Bit 0: State (boolean) - 1 if pressed, 0 if not

C++ code for the DBC is generated using `scripts/cangen`. The code is placed into `Send/generated/can/` and `Receive/generated/can/`.

## Platforms

### stm32f767 Nucleo

The `Send` node polls its built-in button and sends a message to the `Receive` node which sets its on-board LED to match the button. This looks like the `Send` button controlling the `Receive` LED.

Both Send and Receive have CAN RX and TX on pins PB3 and PB4, respectively. See the Nucleo pinouts (`datasheets/nucleo/nucleo_f767zi_zio_right.png`) for the physical pin. For a complete circuit, you will also need 2 CAN drivers like the Analog Devices MAX33042E (see datasheets folder).

### Linux

The projects communicate over virtual CAN (vcan). If using WSL, you need to recompile your kernel to enable vcan. See <https://macformula.github.io/racecar/tutorials/wsl-can/>.

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
| [3AC] 01
Reading DigitalInput "Button"
| Enter 0 for False, 1 for True: 0
| Value was false
CanBase vcan0: Sending
| [3AC] 00
```

```bash
$ ./build/Demo/CAN/Receive/linux/main
Initializing Linux
Opened vcan0 at index 4
CanBase vcan0: Received
| [3AC] 01
DigitalOutput "Indicator" => true
CanBase vcan0: Received
| [3AC] 00
DigitalOutput "Indicator" => false
```
