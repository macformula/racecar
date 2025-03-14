#!/bin/bash

# connect GPIO pin from RPi GPIO 5 to STM CN11 Pin 7
#can prog is assumed to be installed with python-can 3.3.2
#can hat must be attached and can bus 0 is wired
# to run: sudo bash OTA.sh your_binary.bin 



# Check if a binary file was provided as an argument
if [ $# -eq 0 ]; then
    echo "Error: No binary file specified"
    echo "Usage: $0 <binary_file.bin>"
    exit 1
fi

BINARY_FILE=$1  # Store the first command line argument


cd /
cd sys/class/gpio/
echo 5 > export
echo out > gpio5/direction #gain control of GPIO pin
echo 1 > gpio5/value # GPIO signal high
sleep 1

##can prog 
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 125000
sudo ip link set can0 up

canprog -n can0 -f bin stm32 write "$BINARY_FILE" -a 0x08000000 -ve #erases memory on STM and writes binary

sleep 1
echo 0 > gpio5/value #GPIO signal low
echo 5 > unexport #release control of GPIO pin







