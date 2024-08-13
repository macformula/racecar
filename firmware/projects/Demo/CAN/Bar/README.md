# DemoCanBar Project

The DemoCanBar project is best suited for the `raspi` and the `stm32f767` platform. It reads a dummy message and sends a reply with the same data back (created for testing purposes). To observe the full system at work, run alongside the DemoCanFoo project.

### Nodes:
`Bar` (this ecu)

`Foo` (some other ecu)

### Messages:

`Foo` sends `Bar` the `TempSensors` message.
`Bar` reads `TempSensors` message and replies with the `TempSensorsReply` message.