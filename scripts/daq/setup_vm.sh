#!/bin/bash
# DAQ VM test setup — verify logger.py on an amd64 Linux VM.
# Does NOT install InfluxDB or Grafana; logger uses cloud credentials from .env.
# Requires: Ubuntu 22.04+ in VirtualBox or VMware (NOT WSL2 — needs vcan kernel module).
# Run with: bash setup_vm.sh

set -e

echo "=== MAC Formula DAQ VM Test Setup ==="
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ── System packages ───────────────────────────────────────────────────────────
echo "[1/3] Installing system packages..."
sudo apt update -q
sudo apt install -y python3 python3-venv python3-pip can-utils

# ── Python environment ────────────────────────────────────────────────────────
echo "[2/3] Setting up Python virtual environment..."
cd "$SCRIPT_DIR"
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip --quiet
pip install -r requirements.txt --quiet
deactivate
echo "    venv ready. Activate with: source $SCRIPT_DIR/venv/bin/activate"

# ── Virtual CAN interfaces ────────────────────────────────────────────────────
echo "[3/3] Setting up virtual CAN interfaces..."
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan 2>/dev/null || true
sudo ip link set vcan0 up
sudo ip link add dev vcan1 type vcan 2>/dev/null || true
sudo ip link set vcan1 up
echo "    vcan0 and vcan1 are up"

# ── Done ──────────────────────────────────────────────────────────────────────
echo ""
echo "=== Setup complete ==="
echo ""
echo "Run the logger (writes to InfluxDB Cloud via .env credentials):"
echo ""
echo "  source $SCRIPT_DIR/venv/bin/activate"
echo "  python $SCRIPT_DIR/logger.py --interfaces vcan0 vcan1"
echo ""
echo "In a separate terminal, inject test CAN frames:"
echo ""
echo "  # Random frames (exercises CSV logging, won't trigger InfluxDB writes):"
echo "  cangen vcan0 -g 50 &"
echo "  cangen vcan1 -g 50 &"
echo ""
echo "  # Specific frames matching DBC IDs (triggers InfluxDB writes too):"
echo "  cansend vcan0 624#0000000000000000   # Pack_State (voltage, current)"
echo "  cansend vcan0 625#0000000000000000   # Pack_SOC"
echo "  cansend vcan0 283#0000000000000000   # Inv1_ActualValues1 (RPM)"
echo ""
echo "Verify:"
echo "  - CSV log files appear in: $SCRIPT_DIR/logs/"
echo "  - InfluxDB Cloud bucket 'macfe' receives data (check .env for URL)"
echo ""
