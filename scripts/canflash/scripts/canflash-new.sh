#!/bin/bash

# Flash a binary file to an ECU using the CAN bus
# Runs on Raspberry Pi with a CAN hat
# Requires canprog to be installed with python-can 3.3.2
# Usage: bash ./canflash.sh 'file_path' 'gpio_pin'

# Check arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: bash ./canflash.sh 'file_path' 'gpio_pin'"
    exit 1
fi

# Extract arguments
file_path=$1
gpio_pin=$2

# Turn the GPIO pin high
sudo raspi-gpio set $gpio_pin op dh

# Enable the can0 interface at 500kbps message rate
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 500000

# TODO: Send reset CAN message

# Enable the can0 interface at 125kbps flash rate
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 125000

# Flash the file to the ECU
canprog -n can0 -f bin stm32 write "$BINARY_FILE" -a 0x08000000 -ve

# Turn off the can0 interface
sudo ip link set can0 down

# Turn the GPIO pin low
sudo raspi-gpio set $gpio_pin op dl
