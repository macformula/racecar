# Demo CAN Receive Project

This project is a minimal CAN message receiver. It should be executed alongside the Demo CAN Send project.

## Supported Platforms

- `stm32f767`
- `raspi`

### CAN Nodes

`Recv` (this ecu)

`Send` (some other ecu)

### Messages

`Recv` waits for the `TempSensors` message from `Send` then replies with `TempSensorsReply`.
