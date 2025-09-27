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
    import random
    randomValue1 = random.randint(1, 100)
    write_row_to_test_bucket("test1" , fields ={"value": randomValue1})  
    write_row_to_test_bucket("test2" , fields ={"value": random.randint(1, 100)})
    write_row_to_test_bucket("test3" , fields ={"value": random.randint(1, 100)})

    print(f"🏁 Test data written to bucket: {bucket}")

    