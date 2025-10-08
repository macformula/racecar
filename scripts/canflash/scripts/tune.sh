#!/usr/bin/env bash

set -euo pipefail

show_help() {
  cat <<EOF
Usage: $(basename "$0") <aggressiveness 0-100> <dampness 0-100> [iface]

Arguments
  aggressiveness   Integer 0–100 (byte 1)
  dampness         Integer 0–100 (byte 0)
  iface            CAN interface to use (default: vcan0)

The script packs the two values into a 2-byte CAN frame with ID 0x104 and
sends it via 'cansend'.
EOF
}

# --- Parse args -------------------------------------------------------------
[[ $# -eq 0 || $1 == "-h" || $1 == "--help" ]] && { show_help; exit 0; }

if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "Error: need 2 or 3 arguments." >&2
  show_help
  exit 1
fi

AGGR=$1
DAMP=$2
IFACE=${3:-vcan0}

for val in "$AGGR" "$DAMP"; do
  [[ $val =~ ^[0-9]+$ ]] || { echo "Error: values must be integers." >&2; exit 1; }
  (( val >= 0 && val <= 100 )) || { echo "Error: values must be 0-100." >&2; exit 1; }
done

AGGR_HEX=$(printf '%02X' "$AGGR")
DAMP_HEX=$(printf '%02X' "$DAMP")

echo "Sending TuningParams on $IFACE: dampness=$DAMP, aggressiveness=$AGGR (0x${DAMP_HEX}${AGGR_HEX})"
cansend "$IFACE" 104#${DAMP_HEX}${AGGR_HEX}
echo "Message sent!"