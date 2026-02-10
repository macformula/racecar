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

# Check if CAN flash is allowed by reading FcStatus message (ID 0xC9 = 201)
echo "Checking if FrontController is in safe state..."
fc_status=$(timeout 2 candump can0,0C9:7FF -n 1 2>/dev/null | head -n 1)

if [ -z "$fc_status" ]; then
    echo "Error: Could not read FrontController status. Is the FrontController online?"
    sudo ip link set can0 down
    sudo pinctrl set $gpio_pin op dl
    exit 1
fi

# Extract the CanFlashAllowed bit (bit 41, which is bit 1 of byte 5)
# CAN message format: can0  0C9   [8]  B0 B1 B2 B3 B4 B5 B6 B7
#                      $1    $2   $3   $4 $5 $6 $7 $8 $9
# We need byte 5 (awk field $9), bit 1
byte5=$(echo "$fc_status" | awk '{print $9}')
can_flash_allowed=$(( (0x$byte5 >> 1) & 0x01 ))

if [ "$can_flash_allowed" -ne 1 ]; then
    echo "Error: CAN flash not allowed. FrontController is not in a safe state."
    echo "Please ensure the vehicle is in START_DASHBOARD or WAIT_DRIVER_SELECT state."
    sudo ip link set can0 down
    sudo pinctrl set $gpio_pin op dl
    exit 1
fi

echo "CAN flash allowed. Proceeding..."

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
