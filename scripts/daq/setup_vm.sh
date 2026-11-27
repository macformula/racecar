#!/usr/bin/env bash
# Install CAN tools and create virtual interfaces for local testing.
set -euo pipefail

sudo apt-get update
sudo apt-get install -y can-utils iproute2 kmod
sudo modprobe vcan

for interface in vcan0 vcan1; do
    if ip link show dev "$interface" >/dev/null 2>&1; then
        if ! ip -details link show dev "$interface" | grep -qw vcan; then
            echo "$interface exists but is not a virtual CAN interface" >&2
            exit 1
        fi
    else
        sudo ip link add dev "$interface" type vcan
    fi
    sudo ip link set "$interface" up
done

echo 'Run: ./mac-daq --interfaces vcan0 vcan1'
echo 'Send a sample frame: cansend vcan0 624#58028813000000'
echo 'Virtual interfaces may need to be recreated after reboot.'
