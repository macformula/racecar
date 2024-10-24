# Flashing Firmware

After [compiling a project](compile-project.md) for the `stm32f767` platform, you need to flash the firmware to the board. This article describes how to connect to a board and flash using [STM32CubeProgrammer](dev-setup.md/#stm32cubeprogrammer).

## Connect to the Board

We flash using ST-Link which is an interface to STM microcontrollers.

### Development Boards + Dashboard

Most development boards (such as the [Nucleo-144](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html) and [dashboard kit](https://www.st.com/en/evaluation-tools/stm32f7508-dk.html)) have built-in ST-Link hardware and a USB interface.

Simply connect the board to your laptop with a USB cable.

<figure markdown="span">
  ![nucleo stlink](nucleo144-f767zi.png){ width="100%" }
  <figcaption>ST-Link on Nucleo-144 F767ZI</figcaption>
</figure>

### Vehicle ECUs

The vehicle ECUs are not development boards, they are bare STM32F7 processors soldered to a circuit board. These boards do not have a built in ST-Link or USB interface so we must use a different connector.

You will need and external ST-Link and a USB cable from the blue tool chest in the Hatch bay. Some of the ST-Links are Micro-USB and others are USB-C. We should have cables for both.

1. Ensure the ECU is off (not receiving power) and the USB is not connected to your laptop.
2. Carefully connect the grey ribbon cable to the JTAG connector on the ECU board. There is a tab on the ribbon connector which must align with the notch on the JTAG connector.
3. Connect the other end of the ribbon to the ST-Link, aligning the connectors as before.
4. Power on the ECU. Talk to an upper year SW or Electrical member for help.
5. Connect your laptop to the ST-Link using the USB cable.

## Open CubeProgrammer
