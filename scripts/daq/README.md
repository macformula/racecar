# DAQ

## Previous architecture

The Pi ran a Python logger. `candump` read `can0` and `can1`, and the logger decoded frames using the vehicle and powertrain DBCs. It saved decoded signals to CSV on the pi and sent selected signals and fault events to InfluxDB when configured. Grafana read the InfluxDB data.

The directory also contained an unfinished Go app. It read CAN frames into a SQLite cache and sent a heartbeat; its telemetry upload was incomplete.

## Current architecture

The Go app reads SocketCAN frames, decodes them using embedded DBCs, and writes signals to CSV. It can also send selected signals and fault events to InfluxDB for Grafana.

- `main.go`: CAN readers, command-line options, and shutdown
- `logger.go`: DBC decoding and CSV output
- `telemetry.go`: signal selection, tags, and fault thresholds
- `influxdb.go`: optional InfluxDB configuration and delivery

CSV logging continues without InfluxDB. Telemetry is best effort: failed batches are dropped and are not replayed. The app records active fault events, not cleared states.

## How to use

### Prepare the Pi

The Pi needs CAN hardware and drivers that expose `can0` and `can1`. Install the tools once:

```sh
sudo apt update
sudo apt install can-utils iproute2 ca-certificates
```

Check the interfaces with `ip -details link show can0` and `ip -details link show can1`. If an interface is down, bring it up with its correct bitrate, for example:

```sh
sudo ip link set can0 up type can bitrate 500000
```

Repeat for `can1`. Manual CAN setup may need to be repeated after reboot.

### Build and copy

From the repository root on a laptop with Go 1.24+, build for a Pi that reports `aarch64` from `uname -m`:

```sh
cd scripts/daq
GOOS=linux GOARCH=arm64 CGO_ENABLED=0 go build -mod=readonly -o /tmp/mac-daq .
scp /tmp/mac-daq <user>@<pi-host>:mac-daq
```

On the Pi:

```sh
mkdir -p "$HOME/daq-data"
install -m 755 "$HOME/mac-daq" "$HOME/daq-data/mac-daq"
cd "$HOME/daq-data"
```

### Configure and run

For InfluxDB telemetry, copy the team's `.env` into the working directory and restrict access with `chmod 600 .env`:

```dotenv
INFLUX_URL=https://your-existing-influx-host
INFLUX_TOKEN=your-existing-token
INFLUX_ORG=macformula
INFLUX_BUCKET=macfe
```

The app reads `.env` from the current working directory. Its values override exported environment variables. Plain and quoted values work; expansion and multiline values do not. If any InfluxDB variable is missing, the app logs to CSV only. The Pi also needs a network route to the configured InfluxDB host.

```sh
./mac-daq                         # Default: can0 and can1
./mac-daq --interfaces can0       # One interface
./mac-daq --interfaces vcan0 vcan1 # Virtual CAN interfaces
./mac-daq --all                   # Decode all known messages
```

Stop with Ctrl+C or SIGTERM. CSV files are written under `logs/` in the working directory. Monitor disk usage. Import `grafana_dashboard.json` into Grafana if needed.

## Testing

From `scripts/daq`:

```sh
go test -mod=readonly ./...
go vet -mod=readonly ./...
```

For a Linux VM with `can-utils`, `iproute2`, and kernel `vcan` support, create a virtual bus and run DAQ against it:

```sh
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set vcan0 up
./mac-daq --interfaces vcan0
```

In another terminal, send a sample frame:

```sh
cansend vcan0 624#58028813000000
```

Check the CSV in `logs/` for decoded signals. Create `vcan1` the same way to test two interfaces.

Optional manual Python tools live in `tests/`. Install `cantools`, `influxdb-client`, and `python-dotenv`, then run `python3 tests/test_encode.py` or `python3 tests/test_influx.py`. The InfluxDB test writes simulated data to the bucket configured in `.env`.

The README previously recorded comparisons with the Python decoder and virtual CAN. Physical Pi, live InfluxDB/Grafana, and Wi-Fi coverage still need testing.

## Next steps

- Set up automatic startup and persistent CAN configuration on the Pi.
- Add offline telemetry buffering and replay. Test a pit hotspot so the Pi can upload when the car returns within range.
- Set a CSV retention policy for the Pi's storage.
- Evaluate LoRa for live telemetry. Choose signals and rates, build a pit gateway, and test range and coverage.
