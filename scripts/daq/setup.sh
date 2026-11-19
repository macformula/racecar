#!/usr/bin/env bash
# Install local InfluxDB 2 and Grafana on Debian-based systems.
set -euo pipefail

sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg
sudo install -d -m 0755 /etc/apt/keyrings

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT
curl -fsSL https://repos.influxdata.com/influxdata-archive_compat.key -o "$tmpdir/influx.key"
gpg --batch --dearmor -o "$tmpdir/influx.gpg" "$tmpdir/influx.key"
sudo install -m 0644 "$tmpdir/influx.gpg" /etc/apt/keyrings/influxdata.gpg
printf '%s\n' 'deb [signed-by=/etc/apt/keyrings/influxdata.gpg] https://repos.influxdata.com/debian stable main' | sudo tee /etc/apt/sources.list.d/influxdata.list >/dev/null

curl -fsSL https://apt.grafana.com/gpg.key -o "$tmpdir/grafana.key"
gpg --batch --dearmor -o "$tmpdir/grafana.gpg" "$tmpdir/grafana.key"
sudo install -m 0644 "$tmpdir/grafana.gpg" /etc/apt/keyrings/grafana.gpg
printf '%s\n' 'deb [signed-by=/etc/apt/keyrings/grafana.gpg] https://apt.grafana.com stable main' | sudo tee /etc/apt/sources.list.d/grafana.list >/dev/null

sudo apt-get update
sudo apt-get install -y influxdb2 grafana
sudo systemctl enable --now influxdb grafana-server

echo 'Configure InfluxDB at http://localhost:8086 and Grafana at http://localhost:3000.'
echo 'Add the InfluxDB credentials to the DAQ working directory .env file.'
