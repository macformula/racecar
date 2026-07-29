#!/bin/bash
# DAQ setup script for Raspberry Pi (ARM64) or amd64 Linux.
# Installs InfluxDB 2.x and Grafana natively — no Docker.
# Run once on a fresh system: bash setup.sh

set -e

echo "=== MAC Formula DAQ Setup ==="
echo ""

# ── Detect architecture ───────────────────────────────────────────────────────
ARCH=$(dpkg --print-architecture)
echo "Detected architecture: $ARCH"
echo ""

# ── System packages ───────────────────────────────────────────────────────────
echo "[1/6] Installing system packages..."
sudo apt update -q
sudo apt install -y \
    python3 \
    python3-venv \
    python3-pip \
    can-utils \
    curl \
    gnupg2

# ── InfluxDB 2.x ──────────────────────────────────────────────────────────────
echo "[2/6] Installing InfluxDB 2.x..."

sudo rm -f /etc/apt/sources.list.d/influxdata.list
sudo rm -f /usr/share/keyrings/influxdata-archive-keyring.gpg

curl -fsSL https://repos.influxdata.com/influxdata-archive_compat.key \
  | gpg --dearmor \
  | sudo tee /usr/share/keyrings/influxdata-archive-keyring.gpg > /dev/null

echo "deb [signed-by=/usr/share/keyrings/influxdata-archive-keyring.gpg] \
https://repos.influxdata.com/debian stable main" \
  | sudo tee /etc/apt/sources.list.d/influxdata.list

sudo apt update -q
sudo apt install -y influxdb2

sudo systemctl enable --now influxdb
echo "    InfluxDB running at http://localhost:8086"

# ── Grafana ───────────────────────────────────────────────────────────────────
echo "[3/6] Installing Grafana..."

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$ARCH" = "arm64" ]; then
    DEB_FILE="$SCRIPT_DIR/grafana_11.0.0_arm64.deb"

    if [ ! -f "$DEB_FILE" ]; then
        echo "ERROR: ARM64 Grafana .deb not found: $DEB_FILE"
        exit 1
    fi

    # dpkg installs the package; apt install -f resolves any missing dependencies
    sudo dpkg -i "$DEB_FILE" || true
    sudo apt install -f -y

elif [ "$ARCH" = "amd64" ]; then
    sudo apt install -y software-properties-common
    sudo mkdir -p /etc/apt/keyrings/

    curl -fsSL https://apt.grafana.com/gpg.key \
      | sudo gpg --dearmor -o /etc/apt/keyrings/grafana.gpg

    echo "deb [signed-by=/etc/apt/keyrings/grafana.gpg] https://apt.grafana.com stable main" \
      | sudo tee /etc/apt/sources.list.d/grafana.list

    sudo apt update -q
    sudo apt install -y grafana
else
    echo "Unsupported architecture: $ARCH"
    exit 1
fi

sudo systemctl enable --now grafana-server
echo "    Grafana running at http://localhost:3000 (admin/admin)"

# ── CAN interfaces ────────────────────────────────────────────────────────────
echo "[4/6] Configuring CAN interfaces..."

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
echo "[5/6] Setting up Python virtual environment..."

cd "$SCRIPT_DIR"
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip --quiet
pip install -r requirements.txt --quiet
deactivate
echo "    venv created. Activate with: source $SCRIPT_DIR/venv/bin/activate"

# ── Final instructions ────────────────────────────────────────────────────────
echo ""
echo "[6/6] Setup complete!"
echo ""
echo "Next steps:"
echo ""
echo "1. Open InfluxDB at http://localhost:8086"
echo "   Create: org=macformula, bucket=macfe"
echo "   Generate an All Access API token"
echo ""
echo "2. Edit $SCRIPT_DIR/.env:"
echo "   INFLUX_URL=http://localhost:8086"
echo "   INFLUX_TOKEN=<your token>"
echo "   INFLUX_ORG=macformula"
echo "   INFLUX_BUCKET=macfe"
echo ""
echo "3. Open Grafana at http://localhost:3000 (admin/admin)"
echo "   Import grafana_dashboard.json"
echo ""
echo "4. Run the logger:"
echo "   source $SCRIPT_DIR/venv/bin/activate"
echo "   python $SCRIPT_DIR/logger.py"
echo ""
