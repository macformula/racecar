#!/bin/bash

# Infinite loop
while true
do
    # Send CAN message
    cansend vcan0 FFF#1101

    # Wait for 1 seconds
    sleep 1
done

