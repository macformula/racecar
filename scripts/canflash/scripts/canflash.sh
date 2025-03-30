#!/bin/bash

# Flash a binary file to an ECU using the CAN bus
# Runs on Raspberry Pi with a CAN hat attachment
# Requires canprog to be installed with python-can 3.3.2
# Usage: bash ./canflash.sh 'file_path' 'gpio_pin'

# Check arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: bash ./canflash.sh 'file_path' 'gpio_pin' 'ecu'"
    exit 1
fi

# Check if the ecu is a valid integer between 0 and 2
if ! [[ $3 =~ ^[0-2]$ ]]; then
    echo "Error: ECU must be an integer between 0 and 2"
    exit 1
fi

# Extract arguments
file_path=$1
gpio_pin=$2
ecu=$3

# Turn the GPIO pin high
sudo pinctrl set $gpio_pin op dh
sleep 1

# Enable the can0 interface at 500kbps message rate
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 500000
sleep 1

# Send reset CAN message
canprog -n can0 send 0x6D "$(printf "%02X" "$ecu")"
sleep 1

# Enable the can0 interface at 125kbps flash rate
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 125000
sleep 1

# Flash the file to the ECU
canprog -n can0 -f bin stm32 write "$file_path" -a 0x08000000 -ve

# Turn off the can0 interface
sudo ip link set can0 down

# Turn the GPIO pin low
sudo pinctrl set $gpio_pin op dl
