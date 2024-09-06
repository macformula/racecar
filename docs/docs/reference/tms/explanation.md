BusMaster is a software used to send & read CAN messages from veh CAN through the Kvaser. Replaces FrontController for testing. Use BusMaster to send contactor commands.

Use a Button w/ indicator to show power being supplied to everything LV in the accumulator (BMS, TMS, IMD (isolation monitoring device, keeps checking LV - vs HV - and LV - vs HV + to ensure minimum resistance exists being systems. Requires 500 ohm / volt * 600V  = 300 kOhm resistance by FSAE / general automotive ), HVDCDC (DCDC converter for 600V to 24V -> when car starts, uses 24V LV battery. LV Contoller has a multiplexer to switch from 24V to 600V-DCDC after startup. Currently we use power supply in place of 24 V for testing and plan to continue using 24V battery for now since HVDCDC ))

BMS

have 144 cell. Single BMS is compatible with 108 cells, cannot use all without violating insulation

- main (master)
  - controls all logic, communicates with remote
- remote (slave)
  - has same balance circuit
  - is a differet OTS part from orion

LV - is chassis ground
    GLV Ground, grounded low voltage = chassis ground

## Setup

Provide 24V to the harness breakout (see photo 3:05 pm)

##
