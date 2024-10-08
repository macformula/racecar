#!/bin/bash

modprobe -a can can_raw vcan
ip link add dev vcan0 type vcan
ip link set up vcan0
ip link show vcan0