import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

token = os.environ.get("INFLUXDB_TOKEN")
org = "racecar"
url = "http://localhost:8086"

write_client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)

bucket="test"
write_api = write_client.write_api(write_options=SYNCHRONOUS)

def write_row_to_test_bucket(measurement, tags=None, fields=None, timestamp=None):
    """
    Write a row of data to the test bucket
    
    Args:
        measurement (str): The measurement name (like a table name)
        tags (dict): Dictionary of tag key-value pairs (indexed, string values)
        fields (dict): Dictionary of field key-value pairs (actual data, various types)
        timestamp (datetime, optional): Timestamp for the point, defaults to now
    """
    if tags is None:
        tags = {}
    if fields is None:
        fields = {}
    
    # Create a point
    point = Point(measurement)
    
    # Add tags
    for tag_key, tag_value in tags.items():
        point = point.tag(tag_key, str(tag_value))
    
    # Add fields
    for field_key, field_value in fields.items():
        point = point.field(field_key, field_value)
    
    # Add timestamp if provided
    if timestamp:
        point = point.time(timestamp)
    
    # Write to the bucket
    try:
        write_api.write(bucket=bucket, org=org, record=point)
        print(f"✅ Successfully wrote to {measurement}: tags={tags}, fields={fields}")
        return True
    except Exception as e:
        print(f"❌ Error writing to InfluxDB: {e}")
        return False

# Example usage - write some test data
if __name__ == "__main__":
    # Example 1: Car telemetry data
    write_row_to_test_bucket(
        measurement="car_telemetry",
        tags={"car_id": "racecar_01", "driver": "samuel", "track": "silverstone"},
        fields={"speed": 85.5, "rpm": 7200, "throttle": 0.8, "brake_pressure": 0.2}
    )
    
    # Example 2: Sensor data
    write_row_to_test_bucket(
        measurement="sensors",
        tags={"sensor_type": "temperature", "location": "engine"},
        fields={"value": 95.6, "status": "normal"}
    )
    
    # Example 3: Simple counter
    write_row_to_test_bucket(
        measurement="lap_counter",
        tags={"session": "practice"},
        fields={"lap_number": 1, "lap_time": 87.234}
    )
    
    print(f"🏁 Test data written to bucket: {bucket}")

