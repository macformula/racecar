#!/usr/bin/env python3
"""
Raspberry Pi CAN Data Logger.

Reads CAN messages from one or two SocketCAN interfaces via candump, decodes
them using the vehicle and powertrain DBC files, writes decoded signals to a
timestamped CSV file, and streams key telemetry and fault events to InfluxDB.

Usage:
    python logger.py                                   # Pi: reads can0 can1
    python logger.py --interfaces vcan0 vcan1          # VM: virtual CAN
    python logger.py --interfaces can0                 # single interface
"""

import argparse
import queue
import re
import signal
import subprocess
import sys
import threading
import os
from datetime import datetime
from pathlib import Path

import cantools
from influxdb_client import InfluxDBClient, Point
from dotenv import load_dotenv

load_dotenv(override=True)

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
    202,        # FcAlerts             (veh) - 13 boolean fault flags
    211,        # LvStatus             (veh) - ImdFault, BmsFault
    214,        # LvDcdc               (veh) - LV battery/bus voltage + bus current
    230,        # DashCommand          (veh) - Speed
    300,        # Accumulator_Soc      (veh) - battery state
    310,        # SuspensionTravel34   (veh) - linear pots
    401,        # AppsDebug            (veh) - accelerator pedal pots
    402,        # BppsSteerDebug       (veh) - brake pedal + steering pots
    643,        # Inv1_ActualValues1   (pt)  - left motor velocity
    644,        # Inv2_ActualValues1   (pt)  - right motor velocity
    645,        # Inv1_ActualValues2   (pt)  - left motor/inverter temps
    646,        # Inv2_ActualValues2   (pt)  - right motor/inverter temps
    773,        # GnssAttitude         (veh) - IMU roll/pitch/heading
    777,        # GnssImu              (veh) - IMU accelerations + rates
    1572,       # Pack_State           (veh) - battery current/voltage
    1573,       # Pack_SOC             (veh) - battery state of charge
    406451072,  # BmsBroadcast         (veh) - pack temps per segment (extended ID 0x1839F380)
    419361278,  # ThermistorBroadcast  (veh) - thermistor temps        (extended ID 0x18FEF1FE)
}

# Signal names written to InfluxDB — must match DBC signal names exactly.
IMPORTANT_SIGNALS = {
    "Pack_Inst_Voltage",  # HV battery voltage (V)    — Pack_State (1572)
    "Pack_Current",       # HV pack current (A)       — Pack_State (1572)
    "Pack_SOC",           # state of charge (%)       — Pack_SOC (1573)
    "ActualVelocity",     # motor RPM                 — Inv1/2_ActualValues1 (643/644)
    "TempMotor",          # motor temperature (°C)    — Inv1/2_ActualValues2 (645/646)
    "TempInverter",       # inverter temperature (°C) — Inv1/2_ActualValues2 (645/646)
    "TempIGBT",           # IGBT temperature (°C)     — Inv1/2_ActualValues2 (645/646)
    "Speed",              # vehicle speed (mph)       — DashCommand (230)
    "HighThermValue",     # segment max temp (°C)     — BmsBroadcast (0x1839F380)
    "LowThermValue",      # segment min temp (°C)     — BmsBroadcast
    "AvgThermValue",      # segment avg temp (°C)     — BmsBroadcast
    "Apps1Percent",       # APPS sensor 1 position (%) — AppsDebug (401)
    "Apps2Percent",       # APPS sensor 2 position (%) — AppsDebug (401)
    "BppsPercent",        # brake pedal position (%)   — BppsSteerDebug (402)
    "LvBatteryVoltage",   # LV battery voltage (V)     — LvDcdc (214)
    "BusVoltage",         # DCDC bus voltage (V)        — LvDcdc (214)
    "BusCurrent",         # DCDC bus current (A)        — LvDcdc (214)
}

# Boolean fault signals from FcAlerts (202) and LvStatus (211).
# Value == 1 means the fault is active. Each maps to (system, description, severity).
FAULT_SIGNALS = {
    # FcAlerts (202)
    "AppsImplausible":                ("APPS",      "Implausible Pedal Signal",     "CRITICAL"),
    "AccumulatorLowSoc":              ("BMS",       "Low State of Charge",          "WARNING"),
    "AccumulatorContactorWrongState": ("BMS",       "Contactor Wrong State",        "CRITICAL"),
    "MotorRetriesExceeded":           ("Motor",     "Retries Exceeded",             "CRITICAL"),
    "LeftMotorStartingError":         ("Motor",     "Left Motor Start Error",       "CRITICAL"),
    "RightMotorStartingError":        ("Motor",     "Right Motor Start Error",      "CRITICAL"),
    "LeftMotorRunningError":          ("Motor",     "Left Motor Running Error",     "CRITICAL"),
    "RightMotorRunningError":         ("Motor",     "Right Motor Running Error",    "CRITICAL"),
    "DashboardBootTimeout":           ("Dashboard", "Boot Timeout",                 "WARNING"),
    "CanTxError":                     ("CAN",       "TX Error",                     "WARNING"),
    "EV47Active":                     ("Safety",    "EV4.7 Rule Active",            "CRITICAL"),
    "NoInv1Can":                      ("Inverter",  "No INV1 CAN Comm",             "CRITICAL"),
    "NoInv2Can":                      ("Inverter",  "No INV2 CAN Comm",             "CRITICAL"),
    # LvStatus (211)
    "ImdFault":                       ("IMD",       "Isolation Fault",              "CRITICAL"),
    "BmsFault":                       ("BMS",       "BMS Fault",                    "CRITICAL"),
}
SIGNAL_THRESHOLDS = {
    "Pack_Current":      (None,  59.0,  "BMS",      "Pack Overcurrent",               "CRITICAL"),
    "Pack_Inst_Voltage": (360.0, 600.0, "BMS",      "Pack Voltage Out of Range",      "CRITICAL"),
    "TempMotor":         (None,  70.0,  "Motor",    "Motor Overtemperature",          "CRITICAL"),
    "TempInverter":      (None,  60.0,  "Inverter", "Inverter Overtemperature",       "CRITICAL"),
    "TempIGBT":          (None,  60.0,  "Inverter", "IGBT Overtemperature",           "CRITICAL"),
    "HighThermValue":    (None,  60.0,  "BMS",      "Battery Segment Overtemperature","CRITICAL"),
    "LvBatteryVoltage":  (18.0,  29.0,  "LV",       "LV Battery Voltage Out of Range","CRITICAL"),
}

# CAN IDs that produce the same signal names for left and right — tagged "inverter".
INV1_IDS = {643, 645}
INV2_IDS = {644, 646}

# candump -L log format: (timestamp) interface hex_id#hex_data
CANDUMP_LINE_RE = re.compile(
    r'\((\d+\.\d+)\)\s+\S+\s+([0-9A-Fa-f]+)#([0-9A-Fa-f]*)'
)

""" Influx DB Helper Functions """
REQUIRED_INFLUX_ENV = ("INFLUX_URL", "INFLUX_TOKEN", "INFLUX_ORG", "INFLUX_BUCKET")
client: InfluxDBClient | None = None
write_api = None
influx_warning_printed = False


def init_influx() -> bool:
    """Initialize optional InfluxDB telemetry; CSV logging does not depend on it."""
    missing = [name for name in REQUIRED_INFLUX_ENV if not os.getenv(name)]
    if missing:
        print(
            f"Warning: InfluxDB disabled; missing environment variables: {', '.join(missing)}\n"
            "CSV logging will continue locally.",
            file=sys.stderr,
        )
        return False

    global client, write_api
    try:
        client = InfluxDBClient(
            url=os.getenv("INFLUX_URL"),
            token=os.getenv("INFLUX_TOKEN"),
            org=os.getenv("INFLUX_ORG"),
            verify_ssl=False,
        )
        write_api = client.write_api()
    except Exception as exc:
        print(
            f"Warning: InfluxDB disabled; client setup failed: {exc}\n"
            "CSV logging will continue locally.",
            file=sys.stderr,
        )
        client = None
        write_api = None
        return False

    print("InfluxDB telemetry enabled.", file=sys.stderr)
    return True


def _write_influx_point(point: Point) -> None:
    """Best-effort InfluxDB write; never interrupt local CSV logging."""
    global influx_warning_printed
    if write_api is None:
        return

    try:
        write_api.write(bucket=os.getenv("INFLUX_BUCKET"), record=point)
    except Exception as exc:
        if not influx_warning_printed:
            print(
                f"Warning: InfluxDB write failed; continuing CSV logging only: {exc}",
                file=sys.stderr,
            )
            influx_warning_printed = True


def write_to_influx(sig_name: str, value: float, timestamp: float, extra_tags: dict = None) -> None:
    if write_api is None:
        return

    point = (
        Point("car_data")
        .tag("signal", sig_name)
        .field("value", float(value))
        .time(int(timestamp * 1e9))
    )
    if extra_tags:
        for k, v in extra_tags.items():
            point = point.tag(k, v)
    _write_influx_point(point)


def write_fault_to_influx(system: str, fault: str, severity: str, timestamp: float) -> None:
    if write_api is None:
        return

    point = (
        Point("faults")
        .tag("system", system)
        .tag("fault", fault)
        .tag("severity", severity)
        .field("active", 1)
        .time(int(timestamp * 1e9))
    )
    _write_influx_point(point)
# -------------------------------------------------------------------------

def load_dbc(veh_dbc: Path, pt_dbc: Path | None) -> cantools.database.Database:
    """Load veh.dbc and pt.dbc into a single cantools Database."""
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
    f = open(path, 'w', newline='', encoding='utf-8', buffering=1)
    f.write('timestamp_epoch_s,message_id_hex,message_name,signal_name,value,unit\n')
    return path, f


def parse_candump_line(line: str) -> tuple[float, int, bytes] | None:
    """
    Parse a candump -L line into (timestamp_s, can_id, data_bytes).

    candump output format:
        (1709650.123456) vcan0 136#8E91
        (1709650.456789) vcan0 1839F380#C2001816060A0041

    The hex ID is printed as-is by candump — no bit-31 flag, just the raw
    value. For extended frames this is 8 hex digits; for standard frames
    it is up to 3 hex digits. We parse it directly so it matches cantools.
    """
    m = CANDUMP_LINE_RE.match(line.strip())
    if not m:
        return None
    ts_str, hex_id, hex_data = m.groups()
    if len(hex_data) % 2 != 0: # valid hex has to be even number of chars
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

    can id - data 
    """
    msg = next((m for m in db.messages if m.frame_id == can_id), None) #cycle thru dbc to find matched can id
    if msg is None:
        return None
    try:
        decoded = msg.decode(data) # decode into three sections
    except Exception:
        return None
    out = {
        sig.name: (decoded[sig.name], sig.unit or '') 
        for sig in msg.signals
        if sig.name in decoded and decoded[sig.name] is not None
    } # add units to parsed output (ex. v, A, C)
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
        out_queue.put(("error", interface))
        out_queue.put(None)
        return

    processes.append(proc)
    for line in proc.stdout:
        out_queue.put(line) # put can message into queue
    out_queue.put(None)


def main() -> int:
    parser = argparse.ArgumentParser(
        description='CAN data logger: candump -> DBC decode -> CSV + InfluxDB'
    )

    parser.add_argument(
        '--all',
        action='store_true',
        help='Log every decodable message, not just target signals',
    )
    parser.add_argument(
        '--interfaces',
        nargs='+',
        default=['can0', 'can1'],
        metavar='IFACE',
        help='CAN interfaces to read from (default: can0 can1)',
    )

    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent

    veh_dbc = script_dir / 'dbc' / 'veh.dbc'
    pt_dbc  = script_dir / 'dbc' / 'pt.dbc'
    log_dir = script_dir / 'logs'

    interfaces = args.interfaces

    if not veh_dbc.exists():
        print(f'Error: veh.dbc not found at {veh_dbc}', file=sys.stderr)
        return 1

    init_influx()

    db = load_dbc(veh_dbc, pt_dbc)

    log_path, csv_file = open_csv(log_dir)

    print(f'Logging to {log_path}', file=sys.stderr)
    print(f'Interfaces: {", ".join(interfaces)}. Press Ctrl+C to stop.', file=sys.stderr)

    line_queue: queue.Queue = queue.Queue()
    processes: list = []

    # Start one CAN reader thread per interface
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
        if client is not None:
            client.close()
        sys.exit(0)

    signal.signal(signal.SIGINT, cleanup)
    signal.signal(signal.SIGTERM, cleanup)

    num_interfaces = len(interfaces)
    done = 0
    reader_errors = 0

    while done < num_interfaces:
        line = line_queue.get()
        if isinstance(line, tuple) and line[0] == "error":
            reader_errors += 1
            continue
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

        # Build extra tags for signals that need inverter or segment identity.
        extra_tags: dict = {}
        if can_id in INV1_IDS:
            extra_tags["inverter"] = "inv1"
        elif can_id in INV2_IDS:
            extra_tags["inverter"] = "inv2"
        elif can_id == 406451072 and "ThermModuleNum" in signals:
            extra_tags["module"] = str(int(signals["ThermModuleNum"][0]))

        for sig_name, (value, unit) in signals.items():
            csv_file.write(
                f'{timestamp},{id_hex},{msg_name},{sig_name},{value},"{unit}"\n'
            )

            if sig_name in FAULT_SIGNALS and value == 1:
                system, fault_desc, severity = FAULT_SIGNALS[sig_name]
                write_fault_to_influx(system, fault_desc, severity, timestamp)

            if sig_name in IMPORTANT_SIGNALS:
                fval = float(value)
                write_to_influx(sig_name, fval, timestamp, extra_tags)

                if sig_name in SIGNAL_THRESHOLDS:
                    lo, hi, sys_name, desc, sev = SIGNAL_THRESHOLDS[sig_name]
                    checked = abs(fval) if sig_name == "Pack_Current" else fval
                    if (lo is not None and checked < lo) or (hi is not None and checked > hi):
                        write_fault_to_influx(sys_name, desc, sev, timestamp)

    csv_file.close()
    if client is not None:
        client.close()
    return 1 if reader_errors else 0


if __name__ == '__main__':
    sys.exit(main())
