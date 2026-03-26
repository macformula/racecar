#!/usr/bin/env python3
"""
Automated CAN frame sender for simulation: DBC-encode signals, send via cansend.

Runs in a loop until Ctrl+C. Manual `cansend` in another terminal still works.

Usage:
    python3 can_sender.py vcan0
    python3 can_sender.py vcan0 --interval 0.5 --all
    python3 can_sender.py vcan0 --log-dir /tmp/can_logs

Writes a timestamped CSV under scripts/daq/logs by default (same schema as logger.py).
"""

from __future__ import annotations

import argparse
import random
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

import cantools

from logger import TARGET_IDS, load_dbc, open_csv


# Default cycle: (message_name, {signal: value}) — matches dashboard / logger targets
DEFAULT_CYCLE: list[tuple[str, dict[str, Any]]] = [
    (
        "DashCommand",
        {
            "Speed": 30.0,
            "ConfigReceived": 1,
            "HvStarted": 1,
            "MotorStarted": 1,
            "DriveStarted": 1,
            "HvPrechargePercent": 100,
            "HvSocPercent": 80,
        },
    ),
    ("SuspensionTravel34", {"STP3": 64, "STP4": 128}),
    ("GnssAttitude", {"Roll": 5.0, "Pitch": 2.0, "Heading": 90.0}),
    (
        "GnssImu",
        {
            "AccelerationX": 0.5,
            "AccelerationY": 0.1,
            "AccelerationZ": 9.8,
            "AngularRateX": 0.0,
            "AngularRateY": 0.0,
            "AngularRateZ": 0.01,
        },
    ),
    ("Inv1_ActualValues1", {"ActualVelocity": 1500}),
    ("Inv2_ActualValues1", {"ActualVelocity": 1480}),
    (
        "Inv1_ActualValues2",
        {"TempMotor": 45.0, "TempInverter": 38.0, "TempIGBT": 40.0},
    ),
    (
        "Inv2_ActualValues2",
        {"TempMotor": 44.5, "TempInverter": 37.5, "TempIGBT": 39.5},
    ),
    ("Pack_SOC", {"Pack_SOC": 75.0}),
    (
        "BmsBroadcast",
        {"AvgThermValue": 30, "HighThermValue": 35, "LowThermValue": 25},
    ),
    (
        "ThermistorBroadcast",
        {"ThermValue": 28, "RelThermID": 0, "LowThermValue": 25, "HighThermValue": 32},
    ),
]

# Per-signal Gaussian jitter sigma (physical units); unlisted signals stay fixed.
JITTER: dict[tuple[str, str], float] = {
    ("DashCommand", "Speed"): 2.0,
    ("SuspensionTravel34", "STP3"): 3.0,
    ("SuspensionTravel34", "STP4"): 3.0,
    ("GnssAttitude", "Roll"): 0.5,
    ("GnssAttitude", "Pitch"): 0.3,
    ("GnssAttitude", "Heading"): 1.0,
    ("GnssImu", "AccelerationX"): 0.05,
    ("GnssImu", "AccelerationZ"): 0.05,
    ("GnssImu", "AngularRateZ"): 0.005,
    ("Inv1_ActualValues1", "ActualVelocity"): 20.0,
    ("Inv2_ActualValues1", "ActualVelocity"): 20.0,
    ("Inv1_ActualValues2", "TempMotor"): 0.3,
    ("Inv2_ActualValues2", "TempMotor"): 0.3,
    ("Pack_SOC", "Pack_SOC"): 0.2,
    ("BmsBroadcast", "AvgThermValue"): 0.5,
}


def _message_by_name(db: cantools.database.Database, name: str) -> cantools.database.can.message.Message | None:
    return next((m for m in db.messages if m.name == name), None)


def _signal_by_name(msg: cantools.database.can.message.Message, sig_name: str) -> Any:
    return next((s for s in msg.signals if s.name == sig_name), None)


def _clamp_to_signal(sig: Any, value: float) -> float:
    lo = getattr(sig, "minimum", None)
    hi = getattr(sig, "maximum", None)
    v = float(value)
    if lo is not None:
        v = max(v, float(lo))
    if hi is not None:
        v = min(v, float(hi))
    return v


def apply_jitter_for_message(
    state: dict[str, dict[str, Any]],
    db: cantools.database.Database,
    msg_name: str,
) -> None:
    """Add random.gauss(0, sigma) to this message's jittered signals; clamp to DBC limits."""
    if msg_name not in state:
        return
    m = _message_by_name(db, msg_name)
    if m is None:
        return
    for (j_msg, sig_name), sigma in JITTER.items():
        if j_msg != msg_name or sig_name not in state[msg_name]:
            continue
        sig = _signal_by_name(m, sig_name)
        if sig is None:
            continue
        cur = state[msg_name][sig_name]
        if not isinstance(cur, (int, float)):
            continue
        state[msg_name][sig_name] = _clamp_to_signal(sig, cur + random.gauss(0.0, sigma))


def _fill_payload(
    msg: cantools.database.can.message.Message,
    overrides: dict[str, Any],
) -> dict[str, Any]:
    """Build full signal dict for encode/log (same rules as original _fill_and_encode)."""
    payload: dict[str, Any] = {}
    for sig in msg.signals:
        if sig.name in overrides:
            payload[sig.name] = overrides[sig.name]
        else:
            payload[sig.name] = 0
    return payload


def _fill_and_encode(
    msg: cantools.database.can.message.Message,
    overrides: dict[str, Any],
) -> bytes:
    return msg.encode(_fill_payload(msg, overrides))


def log_message_to_csv(
    csv_file: Any,
    timestamp: float,
    msg_name: str,
    msg: cantools.database.can.message.Message,
    payload: dict[str, Any],
) -> None:
    """One row per signal; same columns as logger.py."""
    id_hex = f"0x{msg.frame_id:X}"
    for sig in msg.signals:
        if sig.name not in payload:
            continue
        val = payload[sig.name]
        unit = sig.unit or ""
        csv_file.write(
            f"{timestamp},{id_hex},{msg_name},{sig.name},{val},\"{unit}\"\n"
        )


def can_id_to_cansend_str(can_id: int) -> str:
    """Hex ID string as `cansend` expects.

    `cansend` requires 3 hex chars for standard IDs (SFF) and 8 hex chars
    for extended IDs (EFF).
    """
    if can_id > 0x7FF:
        return f"{can_id:08X}"
    return f"{can_id:03X}"


def send_frame(interface: str, can_id: int, data: bytes) -> None:
    id_str = can_id_to_cansend_str(can_id)
    data_hex = data.hex().upper()
    cmd = ["cansend", interface, f"{id_str}#{data_hex}"]
    subprocess.run(cmd, check=True, capture_output=True)


def main() -> int:
    parser = argparse.ArgumentParser(description="Automated DBC-based CAN sender (cansend)")
    parser.add_argument(
        "interface",
        nargs="?",
        default="vcan0",
        help="SocketCAN interface (default: vcan0)",
    )
    parser.add_argument(
        "--interval",
        type=float,
        default=1.0,
        metavar="SEC",
        help="Seconds between individual frame sends (default: 1.0)",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Cycle every encodable message in the DBC with zero defaults (not only TARGET_IDS preset)",
    )
    parser.add_argument(
        "--veh-dbc",
        type=Path,
        default=None,
        help="Path to veh.dbc (default: <script_dir>/dbc/veh.dbc)",
    )
    parser.add_argument(
        "--pt-dbc",
        type=Path,
        default=None,
        help="Path to pt.dbc (default: <repo_root>/projects/pt.dbc)",
    )
    parser.add_argument(
        "--log-dir",
        type=Path,
        default=None,
        help="Output directory for CSV logs (default: <script_dir>/logs)",
    )
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    veh_dbc = args.veh_dbc or (script_dir / "dbc" / "veh.dbc")
    pt_dbc = args.pt_dbc or (script_dir.parent.parent / "projects" / "pt.dbc")
    log_dir = args.log_dir if args.log_dir is not None else (script_dir / "logs")

    if not veh_dbc.exists():
        print(f"Error: veh.dbc not found at {veh_dbc}", file=sys.stderr)
        return 1

    db = load_dbc(veh_dbc, pt_dbc)

    log_path, csv_file = open_csv(log_dir)
    print(f"Logging to {log_path}", file=sys.stderr)

    try:
        if args.all:
            frames: list[tuple[cantools.database.can.message.Message, bytes]] = []
            for msg in db.messages:
                try:
                    payload = {sig.name: 0 for sig in msg.signals}
                    data = msg.encode(payload)
                    frames.append((msg, data))
                except Exception:
                    continue
            print(
                f"Sending {len(frames)} messages per round (--all), interval {args.interval}s. Ctrl+C to stop.",
                file=sys.stderr,
            )
            try:
                while True:
                    for m, data in frames:
                        ts = time.time()
                        payload = {sig.name: 0 for sig in m.signals}
                        log_message_to_csv(csv_file, ts, m.name, m, payload)
                        send_frame(args.interface, m.frame_id, data)
                        time.sleep(args.interval)
            except KeyboardInterrupt:
                print("\nStopped.", file=sys.stderr)
            return 0

        # TARGET_IDS preset cycle: live state + jitter each round, re-encode every send
        state: dict[str, dict[str, Any]] = {}
        for msg_name, overrides in DEFAULT_CYCLE:
            state[msg_name] = dict(overrides)

        cycle: list[tuple[str, cantools.database.can.message.Message, int]] = []
        for msg_name, _overrides in DEFAULT_CYCLE:
            m = _message_by_name(db, msg_name)
            if m is None:
                print(f"Warning: message {msg_name!r} not in database, skipping.", file=sys.stderr)
                continue
            if m.frame_id not in TARGET_IDS:
                print(
                    f"Warning: {msg_name} frame_id {m.frame_id} not in TARGET_IDS, skipping.",
                    file=sys.stderr,
                )
                continue
            try:
                _fill_and_encode(m, state[msg_name])
            except Exception as e:
                print(f"Warning: encode failed for {msg_name}: {e}", file=sys.stderr)
                continue
            cycle.append((msg_name, m, m.frame_id))

        if not cycle:
            print("Error: no frames to send. Check DBC paths and TARGET_IDS.", file=sys.stderr)
            return 1

        print(
            f"Sending {len(cycle)} message types per round, interval {args.interval}s. Ctrl+C to stop.",
            file=sys.stderr,
        )
        try:
            while True:
                for msg_name, m, fid in cycle:
                    apply_jitter_for_message(state, db, msg_name)
                    payload = _fill_payload(m, state[msg_name])
                    ts = time.time()
                    log_message_to_csv(csv_file, ts, msg_name, m, payload)
                    data = m.encode(payload)
                    send_frame(args.interface, fid, data)
                    time.sleep(args.interval)
        except KeyboardInterrupt:
            print("\nStopped.", file=sys.stderr)
        return 0
    finally:
        csv_file.close()


if __name__ == "__main__":
    sys.exit(main())
