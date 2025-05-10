# Interrupt Handling Project

This project demonstrates how to set up interrupts which can be triggered by different platforms.

## Description

A DigitalOutput is toggled whenever the interrupt is received. A volatile boolean variable is used to set th e
On CLI, the interrupt is a `SIGINT` signal triggered by ctrl+C. The interrupt function is registered using `signal(SIGINT, AppInterruptHandler)`

On the nucleo f767 board, the interrupt is a rising edge on the user button and the DigitalOutput is the on-board LED. Pressing the button onces turns the LED, then pressing it again turns it off. The interrupt is registered by calling `AppInterruptHandler` in the CubeMX-generated interrupt handler.
