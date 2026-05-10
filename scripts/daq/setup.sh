#!/bin/bash
# DAQ setup script for Raspberry Pi (ARM64).
# Installs InfluxDB 2.x and Grafana natively — no Docker.
# Run once on a fresh Pi: bash setup.sh

set -e

echo "=== MAC Formula DAQ Setup ==="
echo ""

# ── System packages ──────────────────────────────────────────────────────────
echo "[1/6] Installing system packages..."
sudo apt update -q
sudo apt install -y python3 python3-venv python3-pip can-utils curl gnupg2

# ── InfluxDB 2.x (native) ────────────────────────────────────────────────────
echo "[2/6] Installing InfluxDB 2.x..."
curl -fsSL https://repos.influxdata.com/influxdata-archive_compat.key \
  | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg
echo "deb [signed-by=/etc/apt/trusted.gpg.d/influxdata-archive_compat.gpg] \
  https://repos.influxdata.com/debian stable main" \
  | sudo tee /etc/apt/sources.list.d/influxdata.list
sudo apt update -q
sudo apt install -y influxdb2

sudo systemctl enable --now influxdb
echo "    InfluxDB running at http://localhost:8086"

# ── Grafana (from bundled ARM64 .deb) ────────────────────────────────────────
echo "[3/6] Installing Grafana from bundled .deb..."
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEB_FILE="$SCRIPT_DIR/grafana_11.0.0_arm64.deb"

if [ ! -f "$DEB_FILE" ]; then
  echo "ERROR: $DEB_FILE not found. Ensure the .deb is in the same directory as this script."
  exit 1
fi

sudo apt install -y "$DEB_FILE"
sudo systemctl enable --now grafana-server
echo "    Grafana running at http://localhost:3000  (admin / admin)"

# ── CAN interfaces ────────────────────────────────────────────────────────────
echo "[4/6] Configuring CAN interfaces..."
# Load the CAN kernel module on boot
if ! grep -q "^can$" /etc/modules 2>/dev/null; then
  echo "can" | sudo tee -a /etc/modules
fi
if ! grep -q "^can_raw$" /etc/modules 2>/dev/null; then
  echo "can_raw" | sudo tee -a /etc/modules
fi

# Bring up can0 at 500 kbit/s (vehicle CAN bus)
sudo ip link set can0 up type can bitrate 500000 2>/dev/null \
  && echo "    can0 up at 500 kbit/s" \
  || echo "    can0 not available (OK if running without hardware)"

# Bring up can1 at 500 kbit/s (powertrain CAN bus) — optional
sudo ip link set can1 up type can bitrate 500000 2>/dev/null \
  && echo "    can1 up at 500 kbit/s" \
  || echo "    can1 not available (OK if single-bus setup)"

# ── Python environment ────────────────────────────────────────────────────────
echo "[5/6] Setting up Python virtual environment..."
cd "$SCRIPT_DIR"
python3 -m venv venv
source venv/bin/activate
pip install --quiet -r requirements.txt
deactivate
echo "    venv created. Activate with: source $SCRIPT_DIR/venv/bin/activate"

# ── Final instructions ────────────────────────────────────────────────────────
echo ""
echo "[6/6] Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Open http://localhost:8086 in a browser"
echo "     → Complete InfluxDB initial setup: org=macformula, bucket=macfe"
echo "     → Generate an All Access API token"
echo "  2. Edit $SCRIPT_DIR/.env and fill in:"
echo "       INFLUX_URL=http://localhost:8086"
echo "       INFLUX_TOKEN=<your token>"
echo "       INFLUX_ORG=macformula"
echo "       INFLUX_BUCKET=macfe"
echo "  3. Open http://localhost:3000 (admin/admin) → import grafana_dashboard.json"
echo "  4. Run the logger:"
echo "       source $SCRIPT_DIR/venv/bin/activate"
echo "       python $SCRIPT_DIR/logger.py can0 --pt-interface can1"
echo ""
