# Demo CAN Send Project

This project is a minimal CAN message sender. It should be executed alongside the Demo CAN Receive project.

## Supported Platforms

- `stm32f767`
- `raspi`

### CAN Nodes

`Send` (this ecu)

`Recv` (some other ecu)

### Messages

`Send` sends `TempSensors` message to `Recv` which replies with `TempSensorReply`.
