#!/usr/bin/env python3
"""
Raspberry Pi CAN Data Logger.

Reads CAN messages from one or two SocketCAN interfaces via candump, decodes
them using the vehicle and powertrain DBC files, and writes decoded signals
to a timestamped CSV file.

Usage:
    python logger.py [interface] [--pt-interface PT_INTERFACE]
    python logger.py vcan0
    python logger.py can0 --pt-interface can1
"""

import argparse
import queue
import re
import signal
import subprocess
import sys
import threading
from datetime import datetime
from pathlib import Path

import cantools


# ---------------------------------------------------------------------------
# Target message IDs — what we care about logging.
#
# How these IDs work:
#   Standard CAN frames use 11-bit IDs (0x000–0x7FF).
#   Extended CAN frames use 29-bit IDs.
#
#   In the DBC file, extended messages are written with bit 31 set, e.g.:
#       BO_ 2553934720 BmsBroadcast ...
#                                        (2553934720 = 0x9839F380, bit 31 set)
#
#   cantools strips bit 31 when it parses the DBC and stores the actual
#   29-bit frame ID:
#       BmsBroadcast.frame_id == 0x1839F380 == 406451072
#
#   candump prints extended IDs as 8 hex digits without bit 31:
#       (t) vcan0 1839F380#...
#
#   So the IDs here must match cantools (29-bit, no bit-31 flag).
# ---------------------------------------------------------------------------
TARGET_IDS = {
    230,        # DashCommand          (veh) - Speed
    300,        # Accumulator_Soc      (veh) - battery state
    310,        # SuspensionTravel34   (veh) - linear pots
    401,        # AppsDebug            (veh) - accelerator pedal pots
    402,        # BppsSteerDebug       (veh) - brake pedal + steering pots
    643,        # Inv1_ActualValues1   (pt)  - left motor velocity
    644,        # Inv2_ActualValues1   (pt)  - right motor velocity
    645,        # Inv1_ActualValues2   (pt)  - motor/inverter temps
    646,        # Inv2_ActualValues2   (pt)  - motor/inverter temps
    773,        # GnssAttitude         (veh) - IMU roll/pitch/heading
    777,        # GnssImu              (veh) - IMU accelerations + rates
    1572,       # Pack_State           (veh) - battery current/voltage
    1573,       # Pack_SOC             (veh) - battery state of charge
    406451072,  # BmsBroadcast         (veh) - battery pack temps (extended ID 0x1839F380)
    419361278,  # ThermistorBroadcast  (veh) - thermistor temps    (extended ID 0x18FEF1FE)
}

# candump -L log format: (timestamp) interface hex_id#hex_data
CANDUMP_LINE_RE = re.compile(
    r'\((\d+\.\d+)\)\s+\S+\s+([0-9A-Fa-f]+)#([0-9A-Fa-f]*)'
)


def load_dbc(veh_dbc: Path, pt_dbc: Path | None) -> cantools.database.Database:
    """Load veh.dbc and optionally pt.dbc into a single cantools Database."""
    db = cantools.database.load_file(str(veh_dbc))
    if pt_dbc and pt_dbc.exists():
        with open(pt_dbc, encoding='utf-8') as f:
            db.add_dbc(f)
    return db


def open_csv(log_dir: Path) -> tuple[Path, object]:
    """Create logs dir, open CSV with header, return (path, file)."""
    log_dir.mkdir(parents=True, exist_ok=True)
    ts = datetime.now().strftime('%Y%m%d_%H%M%S')
    path = log_dir / f'can_log_{ts}.csv'
    f = open(path, 'w', newline='', encoding='utf-8')
    f.write('timestamp_epoch_s,message_id_hex,message_name,signal_name,value,unit\n')
    return path, f


def parse_candump_line(line: str) -> tuple[float, int, bytes] | None:
    """
    Parse a candump -L line into (timestamp_s, can_id, data_bytes).

    candump output format:
        (1709650000.123456) vcan0 136#8E91
        (1709650000.456789) vcan0 1839F380#C2001816060A0041

    The hex ID is printed as-is by candump — no bit-31 flag, just the raw
    value. For extended frames this is 8 hex digits; for standard frames
    it is up to 3 hex digits. We parse it directly so it matches cantools.
    """
    m = CANDUMP_LINE_RE.match(line.strip())
    if not m:
        return None
    ts_str, hex_id, hex_data = m.groups()
    if len(hex_data) % 2 != 0:
        return None
    return float(ts_str), int(hex_id, 16), bytes.fromhex(hex_data)


def decode_frame(
    db: cantools.database.Database,
    can_id: int,
    data: bytes,
) -> tuple[str, dict[str, tuple]] | None:
    """
    Look up the message in the DBC by frame ID and decode the raw bytes.

    Returns (message_name, {signal_name: (value, unit)}) or None if the
    message is not in the DBC or decoding fails.
    """
    msg = next((m for m in db.messages if m.frame_id == can_id), None)
    if msg is None:
        return None
    try:
        decoded = msg.decode(data)
    except Exception:
        return None
    out = {
        sig.name: (decoded[sig.name], sig.unit or '')
        for sig in msg.signals
        if sig.name in decoded and decoded[sig.name] is not None
    }
    return (msg.name, out) if out else None


def _candump_reader(
    interface: str,
    out_queue: queue.Queue,
    processes: list,
) -> None:
    """
    Spawn candump on `interface`, forward each line to out_queue.
    Puts a None sentinel when done so the consumer knows one source finished.
    """
    try:
        proc = subprocess.Popen(
            ['candump', '-L', interface],
            stdout=subprocess.PIPE,
            text=True,
            bufsize=1,
        )
    except FileNotFoundError:
        print(
            "Error: 'candump' not found.\n"
            "On the Raspberry Pi install can-utils: sudo apt install can-utils",
            file=sys.stderr,
        )
        out_queue.put(None)
        return

    processes.append(proc)
    for line in proc.stdout:
        out_queue.put(line)
    out_queue.put(None)


def main() -> int:
    parser = argparse.ArgumentParser(
        description='CAN data logger: candump -> DBC decode -> CSV'
    )
    parser.add_argument(
        'interface',
        nargs='?',
        default='vcan0',
        help='Primary SocketCAN interface (default: vcan0)',
    )
    parser.add_argument(
        '--pt-interface',
        default=None,
        metavar='INTERFACE',
        help='Optional second interface for powertrain bus (e.g. can1)',
    )
    parser.add_argument(
        '--veh-dbc',
        type=Path,
        default=None,
        help='Path to veh.dbc (default: <script_dir>/dbc/veh.dbc)',
    )
    parser.add_argument(
        '--pt-dbc',
        type=Path,
        default=None,
        help='Path to pt.dbc (default: <repo_root>/projects/pt.dbc)',
    )
    parser.add_argument(
        '--log-dir',
        type=Path,
        default=None,
        help='Output directory for CSV logs (default: <script_dir>/logs)',
    )
    parser.add_argument(
        '--all',
        action='store_true',
        help='Log every decodable message, not just target signals',
    )
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    veh_dbc = args.veh_dbc or (script_dir / 'dbc' / 'veh.dbc')
    pt_dbc  = args.pt_dbc  or (script_dir.parent.parent / 'projects' / 'pt.dbc')
    log_dir = args.log_dir or (script_dir / 'logs')

    if not veh_dbc.exists():
        print(f'Error: veh.dbc not found at {veh_dbc}', file=sys.stderr)
        return 1

    db = load_dbc(veh_dbc, pt_dbc)
    log_path, csv_file = open_csv(log_dir)

    interfaces = [args.interface]
    if args.pt_interface:
        interfaces.append(args.pt_interface)

    print(f'Logging to {log_path}', file=sys.stderr)
    print(f'Interfaces: {", ".join(interfaces)}. Press Ctrl+C to stop.', file=sys.stderr)

    # Use a queue so frames from multiple candump processes merge into one loop.
    line_queue: queue.Queue = queue.Queue()
    processes: list = []

    for iface in interfaces:
        t = threading.Thread(
            target=_candump_reader,
            args=(iface, line_queue, processes),
            daemon=True,
        )
        t.start()

    def cleanup(_sig=None, _frame=None):
        for p in processes:
            p.terminate()
            p.wait()
        csv_file.close()
        sys.exit(0)

    signal.signal(signal.SIGINT, cleanup)
    signal.signal(signal.SIGTERM, cleanup)

    # Count how many None sentinels we expect (one per interface thread).
    num_interfaces = len(interfaces)
    done = 0

    while done < num_interfaces:
        line = line_queue.get()
        if line is None:
            done += 1
            continue

        parsed = parse_candump_line(line)
        if not parsed:
            continue
        timestamp, can_id, data = parsed

        if not args.all and can_id not in TARGET_IDS:
            continue

        result = decode_frame(db, can_id, data)
        if not result:
            continue

        msg_name, signals = result
        id_hex = f'0x{can_id:X}'
        for sig_name, (value, unit) in signals.items():
            csv_file.write(
                f'{timestamp},{id_hex},{msg_name},{sig_name},{value},"{unit}"\n'
            )

    csv_file.close()
    return 0


if __name__ == '__main__':
    sys.exit(main())
