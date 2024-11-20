#!/bin/bash


CAN_PORT=$1

if [ -z "$CAN_PORT" ]; then
    echo "Error: CAN_PORT is required as an argument."
    echo "Usage: $0 <CAN_PORT>"
    exit 1
fi

sudo modprobe -a can can_raw vcan
sudo ip link add dev $CAN_PORT type vcan
sudo ip link set up $CAN_PORT
