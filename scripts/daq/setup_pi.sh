#!/bin/bash
# DAQ setup for Raspberry Pi — cloud deployment.
# Installs only what logger.py needs: Python env + CAN interfaces.
# InfluxDB and Grafana run in the cloud, not on the Pi.
# Run once on a fresh Pi: bash setup_pi.sh

set -e

echo "=== MAC Formula DAQ Pi Setup ==="
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ── System packages ───────────────────────────────────────────────────────────
echo "[1/3] Installing system packages..."
sudo apt update -q
sudo apt install -y python3 python3-venv python3-pip can-utils

# ── CAN interfaces ────────────────────────────────────────────────────────────
echo "[2/3] Configuring CAN interfaces..."

if ! grep -q "^can$" /etc/modules 2>/dev/null; then
    echo "can" | sudo tee -a /etc/modules
fi
if ! grep -q "^can_raw$" /etc/modules 2>/dev/null; then
    echo "can_raw" | sudo tee -a /etc/modules
fi

sudo ip link set can0 up type can bitrate 500000 2>/dev/null \
  && echo "    can0 up at 500 kbit/s" \
  || echo "    can0 not available (OK if running without hardware)"

sudo ip link set can1 up type can bitrate 500000 2>/dev/null \
  && echo "    can1 up at 500 kbit/s" \
  || echo "    can1 not available (OK for single-bus setup)"

# ── Python environment ────────────────────────────────────────────────────────
echo "[3/3] Setting up Python virtual environment..."

cd "$SCRIPT_DIR"
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip --quiet
pip install -r requirements.txt --quiet
deactivate
echo "    venv created."

# ── Done ──────────────────────────────────────────────────────────────────────
echo ""
echo "=== Setup complete ==="
echo ""
echo "Next steps:"
echo ""
echo "1. Copy .env to $SCRIPT_DIR/.env with your InfluxDB Cloud credentials:"
echo "   INFLUX_URL=https://us-east-1-1.aws.cloud2.influxdata.com"
echo "   INFLUX_TOKEN=<team token>"
echo "   INFLUX_ORG=macformula"
echo "   INFLUX_BUCKET=macfe"
echo ""
echo "2. Run the logger:"
echo "   source $SCRIPT_DIR/venv/bin/activate"
echo "   python $SCRIPT_DIR/logger.py"
echo ""
