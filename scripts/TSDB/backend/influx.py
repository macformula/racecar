import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from dotenv import load_dotenv

load_dotenv()
token = os.environ.get("INFLUXDB_TOKEN")
org = os.environ.get("INFLUX_ORG")
url = os.environ.get("INFLUX_URL")
bucket = os.environ.get("INFLUX_BUCKET")

print(f"Debug - URL: {url}")
print(f"Debug - Token: {'***' if token else None}")
print(f"Debug - Org: {org}")
print(f"Debug - Bucket: {bucket}")
write_client = InfluxDBClient(url=url, token=token, org=org)
write_api = write_client.write_api(write_options=SYNCHRONOUS)


def write_row_to_test_bucket(measurement: str, tags: dict = None, fields: dict = None) -> bool:

    point = Point(measurement).time(int(time.time() * 1e9), WritePrecision.NS)
    if tags:
        for k, v in tags.items():
            point = point.tag(k, v)
    if fields:
        for k, v in fields.items():
            point = point.field(k, v)
    write_api.write(bucket=bucket, org=org, record=point)
    return True


if __name__ == "__main__":
    import random
    print("InfluxDB URL:", url)

    randomValue1 = random.uniform(0, 100)
    write_row_to_test_bucket("test1", fields={"value": randomValue1})