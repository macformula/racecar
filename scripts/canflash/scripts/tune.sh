#!/usr/bin/env bash

set -euo pipefail

show_help() {
  cat <<EOF
Usage: $(basename "$0") <aggressiveness 0-100> [iface]

Sends a 2-byte TuningParams message (CAN ID 0x104) with:
- damping        = 0
- aggressiveness = provided value

Arguments:
  value   Integer 0–100 inclusive.
  iface   CAN interface (default: vcan0).
EOF
}

# --- Parse args -------------------------------------------------------------
[[ $# -eq 0 || $1 == "-h" || $1 == "--help" ]] && { show_help; exit 0; }

AGGRESSIVENESS=$1
IFACE=${2:-vcan0}

# --- Validate value ---------------------------------------------------------
if (( AGGRESSIVENESS < 0 || AGGRESSIVENESS > 100 )); then
  echo "Error: value must be between 0 and 100." >&2
  exit 1
fi

# --- Convert both damping (0) and aggressiveness (user input) to hex --------
DAMPING_HEX=00
AGGR_HEX=$(printf '%02X' "$AGGRESSIVENESS")

echo "Sending TuningParams on $IFACE: damping=0, aggressiveness=$AGGRESSIVENESS (0x$AGGR_HEX)"
cansend "$IFACE" 104#${DAMPING_HEX}${AGGR_HEX}

echo "Message sent!"
