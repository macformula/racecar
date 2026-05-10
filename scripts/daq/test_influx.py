#!/usr/bin/env python3
"""
Simulates CAN signal data being written to InfluxDB.
Run this to verify the Python to InfluxDB connection without needing a CAN bus.
Uses the same measurement/tag/field structure as logger.py.
"""

import os
import time
import math
import random
import urllib3
from dotenv import load_dotenv
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

load_dotenv(override=True)

client = InfluxDBClient(
    url=os.getenv("INFLUX_URL"),
    token=os.getenv("INFLUX_TOKEN"),
    org=os.getenv("INFLUX_ORG"),
    verify_ssl=False,
)
write_api = client.write_api(write_options=SYNCHRONOUS)
bucket = os.getenv("INFLUX_BUCKET")


def write_signal(sig_name, value):
    point = (
        Point("car_data")
        .tag("signal", sig_name)
        .field("value", float(value))
    )
    write_api.write(bucket=bucket, record=point)


def write_fault(system, fault, severity):
    point = (
        Point("faults")
        .tag("system", system)
        .tag("fault", fault)
        .tag("severity", severity)
        .field("active", 1)
    )
    write_api.write(bucket=bucket, record=point)


def main():
    print(f"Connecting to InfluxDB at {os.getenv('INFLUX_URL')} ...")
    print(f"Org: {os.getenv('INFLUX_ORG')}  Bucket: {bucket}\n")

    # Verify connection with a test write
    try:
        test_point = Point("car_data").tag("signal", "_test").field("value", 0.0)
        write_api.write(bucket=bucket, record=test_point)
        print("Connection OK — test write succeeded.\n")
    except Exception as e:
        print(f"ERROR: Could not write to InfluxDB — {e}")
        print("Check your token, org, bucket, and URL in .env")
        return

    print("Writing simulated CAN signals (Ctrl+C to stop)...\n")

    t = 0
    try:
        while True:
            # Simulate values using actual DBC signal names so the dashboard queries match.
            inv1_rpm      = 2000 + 1500 * math.sin(t * 0.3) + random.uniform(-50, 50)
            inv2_rpm      = 1800 + 1400 * math.sin(t * 0.3 + 0.2) + random.uniform(-50, 50)
            battery_v     = 400 + 10 * math.sin(t * 0.05) + random.uniform(-1, 1)
            soc           = max(20, 90 - t * 0.2 + random.uniform(-0.5, 0.5))
            motor_temp    = 40 + 20 * (1 - math.exp(-t * 0.02)) + random.uniform(-1, 1)
            inverter_temp = 35 + 15 * (1 - math.exp(-t * 0.015)) + random.uniform(-1, 1)
            igbt_temp     = inverter_temp + random.uniform(-2, 2)
            pack_current  = 50 + 30 * math.sin(t * 0.4) + random.uniform(-5, 5)
            speed         = 60 + 25 * math.sin(t * 0.05) + random.uniform(-2, 2)

            # Scalar signals (no extra tags needed)
            for name, value in [
                ("Pack_Inst_Voltage", battery_v),
                ("Pack_Current",      pack_current),
                ("Pack_SOC",          soc),
                ("Speed",             speed),
            ]:
                write_signal(name, value)

            # Motor signals — write once per inverter with the inverter tag
            for inverter, rpm, mtemp, itemp, igbt in [
                ("inv1", inv1_rpm, motor_temp,       inverter_temp,       igbt_temp),
                ("inv2", inv2_rpm, motor_temp + 1.5, inverter_temp + 1.0, igbt_temp + 1.0),
            ]:
                for sig_name, val in [
                    ("ActualVelocity", rpm),
                    ("TempMotor",      mtemp),
                    ("TempInverter",   itemp),
                    ("TempIGBT",       igbt),
                ]:
                    point = (
                        Point("car_data")
                        .tag("signal", sig_name)
                        .tag("inverter", inverter)
                        .field("value", float(val))
                    )
                    write_api.write(bucket=bucket, record=point)

            # Battery segment temps — 6 modules, each with High/Low/Avg
            base_temp = 35 + 5 * (1 - math.exp(-t * 0.01))
            for module in range(6):
                spread = module * 0.8 + random.uniform(-0.5, 0.5)
                for sig_name, val in [
                    ("HighThermValue", base_temp + spread + 2),
                    ("LowThermValue",  base_temp + spread - 2),
                    ("AvgThermValue",  base_temp + spread),
                ]:
                    point = (
                        Point("car_data")
                        .tag("signal", sig_name)
                        .tag("module", str(module))
                        .field("value", float(val))
                    )
                    write_api.write(bucket=bucket, record=point)

            # Occasionally write a simulated fault
            if t > 0 and int(t) % 15 == 0:
                write_fault("BMS", "Low State of Charge", "WARNING")
                print(f"  t={t:.0f}s  Wrote fault: BMS / Low State of Charge / WARNING")
            if t > 0 and int(t) % 30 == 0:
                write_fault("Motor", "Left Motor Running Error", "CRITICAL")
                print(f"  t={t:.0f}s  Wrote fault: Motor / Left Motor Running Error / CRITICAL")

            print(
                f"  t={t:.0f}s  inv1={inv1_rpm:.0f}rpm  inv2={inv2_rpm:.0f}rpm  "
                f"v={battery_v:.1f}V  soc={soc:.1f}%  "
                f"mtemp={motor_temp:.1f}°C  curr={pack_current:.1f}A  spd={speed:.1f}mph"
            )

            t += 1
            time.sleep(1)

    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        client.close()


if __name__ == "__main__":
    main()
