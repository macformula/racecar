#!/usr/bin/env bash
#
# cantune – Send a 0‑100 % dashboard set‑point to the Front‑Controller
#
# Usage:
#   cantune <value 0‑100> [iface]
#
# Examples:
#   cantune 75          # send 75 % on can0
#   cantune 0 can1      # send 0 % on can1
#
# Needs:
#   * can‑utils (cansend)
#   * your CAN iface (e.g. can0) already up: 
#       sudo ip link set can0 up type can bitrate 500000
#

set -euo pipefail

show_help() {
  cat <<EOF
Usage: $(basename "$0") <value 0-100> [iface]

Sends a single Dash_Setpoint (CAN ID 0x102) with the specified percentage.
Arguments:
  value   Integer 0–100 inclusive.
  iface   CAN interface (default: can0).
EOF
}

# --- Parse args -------------------------------------------------------------
[[ $# -eq 0 || $1 == "-h" || $1 == "--help" ]] && { show_help; exit 0; }

VALUE=$1
IFACE=${2:-can0}

# --- Validate value ---------------------------------------------------------
if ! [[ $VALUE =~ ^[0-9]+$ ]]; then
  echo "Error: value must be an integer." >&2
  exit 1
fi

if (( VALUE < 0 || VALUE > 100 )); then
  echo "Error: value must be between 0 and 100." >&2
  exit 1
fi

# --- Convert to two‑digit hex ----------------------------------------------
HEX=$(printf '%02X' "$VALUE")

# --- Send frame -------------------------------------------------------------
cansend "$IFACE" 102#"$HEX"

echo "Sent Dash_Setpoint = $VALUE% (0x$HEX) on $IFACE"