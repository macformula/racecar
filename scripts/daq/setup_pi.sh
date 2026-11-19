#!/usr/bin/env bash
# Install CAN tools and bring up available Pi interfaces.
set -euo pipefail

bitrate="${CAN_BITRATE:-500000}"
if (($# == 0)); then
    interfaces=(can0 can1)
else
    interfaces=("$@")
fi

sudo apt-get update
sudo apt-get install -y can-utils iproute2 ca-certificates

for interface in "${interfaces[@]}"; do
    if ! ip link show dev "$interface" >/dev/null 2>&1; then
        echo "$interface not found; check the CAN hardware and driver" >&2
        exit 1
    fi
    if ip -o link show dev "$interface" | grep -Eq '<([^>]*,)?UP(,|>)'; then
        echo "$interface is already up; leaving its settings unchanged"
        continue
    fi
    sudo ip link set "$interface" up type can bitrate "$bitrate"
    echo "$interface is up at $bitrate bit/s"
done

echo "CAN setup complete. Run the Go app with: ./mac-daq --interfaces ${interfaces[*]}"
echo "Interface settings may need to be reapplied after reboot."
