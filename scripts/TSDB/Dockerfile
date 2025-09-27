# Use the official InfluxDB image
FROM influxdb:2.7

# Set environment variables for initial setup
ENV DOCKER_INFLUXDB_INIT_MODE=setup
ENV DOCKER_INFLUXDB_INIT_USERNAME=admin
ENV DOCKER_INFLUXDB_INIT_PASSWORD=password123
ENV DOCKER_INFLUXDB_INIT_ORG=racecar
ENV DOCKER_INFLUXDB_INIT_BUCKET=telemetry
ENV DOCKER_INFLUXDB_INIT_ADMIN_TOKEN=my-super-secret-auth-token

# Expose the default InfluxDB port
EXPOSE 8086

# The default command will start InfluxDB
CMD ["influxd"]
