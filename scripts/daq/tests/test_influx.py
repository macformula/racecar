"""Manually stream sample signals/faults to the configured InfluxDB bucket."""
import math
import os
import time
from itertools import count
from pathlib import Path

from dotenv import load_dotenv
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS


def signal(name, value, **tags):
    point = Point("car_data").tag("signal", name).field("value", float(value))
    for key, value in tags.items():
        point.tag(key, value)
    return point


def main():
    load_dotenv(Path(__file__).resolve().parents[1] / ".env", override=True)
    required = ("INFLUX_URL", "INFLUX_TOKEN", "INFLUX_ORG", "INFLUX_BUCKET")
    if not all(os.getenv(key) for key in required):
        raise SystemExit("Set INFLUX_URL, INFLUX_TOKEN, INFLUX_ORG, and INFLUX_BUCKET in .env.")
    url, token, org, bucket = (os.environ[key] for key in required)
    with InfluxDBClient(url=url, token=token, org=org) as client:
        writer = client.write_api(write_options=SYNCHRONOUS)
        print("Writing simulated telemetry to the configured bucket; Ctrl+C to stop.")
        try:
            for step in count():
                points = [
                    signal("Pack_Inst_Voltage", 490),
                    signal("Pack_Current", 27),
                    signal("Pack_SOC", 80),
                    signal("ActualVelocity", 2000 + 500 * math.sin(step / 5), inverter="inv1"),
                    signal("ActualVelocity", 1800 + 500 * math.sin(step / 5), inverter="inv2"),
                    signal("HighThermValue", 37, module="0"),
                ]
                if step and step % 15 == 0:
                    fault = Point("faults").tag("system", "CAN").tag("fault", "TX Error")
                    points.append(fault.tag("severity", "WARNING").field("active", 1))
                writer.write(bucket=bucket, record=points)
                time.sleep(1)
        except KeyboardInterrupt:
            print("Stopped.")


if __name__ == "__main__":
    main()
