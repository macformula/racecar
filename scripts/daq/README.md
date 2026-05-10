# DAQ — Data Acquisition System

Real-time CAN telemetry for the MAC Formula Electric racecar.
Data flows from two CAN buses on the Raspberry Pi → InfluxDB → Grafana dashboard.

## Architecture

```
Racecar
  can0  (Vehicle bus — FC, LVC, TMS, BMS, GPS)
  can1  (Powertrain bus — Inverter 1, Inverter 2)
         │
         ▼
  Raspberry Pi 4
  ┌─────────────────────────────────┐
  │  candump -L can0 / can1         │  raw CAN frames
  │  logger.py                      │  decode via veh.dbc + pt.dbc
  │  CSV logs  (logs/can_log_*.csv) │  full signal archive on SD card
  └─────────────────────────────────┘
         │ 4G LTE (SIM card)
         ▼
  InfluxDB Cloud  (AWS us-east-1, free tier)
         │
         ▼
  Dock laptop — Grafana (local, free)
  ┌─────────────────────────────────┐
  │  Vehicle Safety row             │
  │  Performance row                │
  │  Segment Temperatures row       │
  └─────────────────────────────────┘
```

The Pi runs **only** logger.py — no InfluxDB or Grafana on the Pi itself. Data is written to InfluxDB Cloud over cellular. Grafana on the dock laptop queries InfluxDB Cloud independently — the Pi and laptop only need their own internet connections, not the same network.

---

## InfluxDB Data Schema

All telemetry goes into two measurements:

### `car_data` — key signal values
| Tag | Values | Description |
|---|---|---|
| `signal` | see table below | DBC signal name |
| `inverter` | `inv1`, `inv2` | set only for motor/inverter signals |
| `module` | `0`–`5` | set only for BmsBroadcast segment temps |

Field: `value` (float)

### `faults` — active fault events
| Tag | Example values | Description |
|---|---|---|
| `system` | `BMS`, `Motor`, `Inverter`, `IMD`, `APPS`, `CAN`, `Safety`, `Dashboard` | subsystem that raised the fault |
| `fault` | `"Low State of Charge"`, `"No INV1 CAN Comm"` | human-readable description |
| `severity` | `WARNING`, `CRITICAL` | impact level |

Field: `active` (int, always 1)

### Signal name reference

| DBC signal | Source message | CAN ID | Description |
|---|---|---|---|
| `Pack_Inst_Voltage` | Pack_State | 1572 | HV battery voltage (V) |
| `Pack_Current` | Pack_State | 1572 | HV pack current (A) |
| `Pack_SOC` | Pack_SOC | 1573 | State of charge (%) |
| `ActualVelocity` | Inv1/2_ActualValues1 | 643/644 | Motor RPM — tagged `inverter=inv1/inv2` |
| `TempMotor` | Inv1/2_ActualValues2 | 645/646 | Motor temperature (°C) — tagged by inverter |
| `TempInverter` | Inv1/2_ActualValues2 | 645/646 | Inverter temperature (°C) — tagged by inverter |
| `TempIGBT` | Inv1/2_ActualValues2 | 645/646 | IGBT temperature (°C) — tagged by inverter |
| `Speed` | DashCommand | 230 | Vehicle speed (mph from DBC) |
| `HighThermValue` | BmsBroadcast | 0x1839F380 | Segment max temp (°C) — tagged `module=0..5` |
| `LowThermValue` | BmsBroadcast | 0x1839F380 | Segment min temp (°C) — tagged `module=0..5` |
| `AvgThermValue` | BmsBroadcast | 0x1839F380 | Segment avg temp (°C) — tagged `module=0..5` |

---

## Fault Detection

Faults are **boolean bits** in two CAN messages, not a single fault code signal.

**FcAlerts (ID 202)** — Front Controller alerts:
| Signal | System | Description | Severity |
|---|---|---|---|
| `AppsImplausible` | APPS | Implausible Pedal Signal | CRITICAL |
| `AccumulatorLowSoc` | BMS | Low State of Charge | WARNING |
| `AccumulatorContactorWrongState` | BMS | Contactor Wrong State | CRITICAL |
| `MotorRetriesExceeded` | Motor | Retries Exceeded | CRITICAL |
| `LeftMotorStartingError` | Motor | Left Motor Start Error | CRITICAL |
| `RightMotorStartingError` | Motor | Right Motor Start Error | CRITICAL |
| `LeftMotorRunningError` | Motor | Left Motor Running Error | CRITICAL |
| `RightMotorRunningError` | Motor | Right Motor Running Error | CRITICAL |
| `DashboardBootTimeout` | Dashboard | Boot Timeout | WARNING |
| `CanTxError` | CAN | TX Error | WARNING |
| `EV47Active` | Safety | EV4.7 Rule Active | CRITICAL |
| `NoInv1Can` | Inverter | No INV1 CAN Comm | CRITICAL |
| `NoInv2Can` | Inverter | No INV2 CAN Comm | CRITICAL |

**LvStatus (ID 211)** — LV Controller status:
| Signal | System | Description | Severity |
|---|---|---|---|
| `ImdFault` | IMD | Isolation Fault | CRITICAL |
| `BmsFault` | BMS | BMS Fault | CRITICAL |

When any of these bits is `1`, `logger.py` writes a point to the `faults` measurement with the system, description, and severity. The Grafana Fault Log table shows all faults in the selected time range with colour-coded severity.

---

## Raspberry Pi Setup (Production)

Run once on a fresh Pi:

```bash
cd scripts/daq
bash setup.sh
```

The script installs `python3`, `can-utils`, configures `can0`/`can1` at 500 kbit/s, and creates a Python virtualenv.

After running `setup.sh`:

1. Copy `.env` onto the Pi (it's gitignored — copy it manually or `scp` it over):
   ```
   INFLUX_URL=https://us-east-1-1.aws.cloud2.influxdata.com
   INFLUX_TOKEN=<team token>
   INFLUX_ORG=macformula
   INFLUX_BUCKET=macfe
   ```

2. Start logging:
   ```bash
   source venv/bin/activate
   python logger.py
   ```

---

## Linux VM Setup (testing logger.py without a Pi)

Use this to verify logger.py works before deploying to the Pi. Requires Ubuntu 22.04+ in VirtualBox or VMware — **not WSL2** (WSL2 lacks the `vcan` kernel module).

```bash
cd scripts/daq
bash setup_vm.sh
```

The script installs Python deps and brings up `vcan0`/`vcan1` (virtual CAN interfaces). Uses the cloud InfluxDB credentials already in `.env` — no local InfluxDB needed.

```bash
# Terminal 1 — run the logger:
source venv/bin/activate
python logger.py --interfaces vcan0 vcan1

# Terminal 2 — inject test CAN frames:
cansend vcan0 624#1234567890ABCDEF   # Pack_State → writes to InfluxDB
cangen vcan0 -g 50 &                 # random frames → exercises CSV logging
```

Verify: CSV appears in `logs/`, and `car_data` measurement appears in InfluxDB Cloud Data Explorer.

---

## Dock Laptop Setup (Grafana)

Run once on the laptop you'll use at the dock:

1. Install Grafana via Docker:
   ```bash
   docker run -d -p 3000:3000 --name grafana grafana/grafana
   ```
   Open `http://localhost:3000` (admin / admin).

2. Add InfluxDB Cloud as a data source:
   - Type: **InfluxDB**, Query language: **Flux**
   - URL: value of `INFLUX_URL` from `.env`
   - Token: value of `INFLUX_TOKEN` from `.env`
   - Organisation: `macformula`, Default bucket: `macfe`

3. Import `grafana_dashboard.json` via **Dashboards → Import**.

The dashboard will live-update as the Pi writes data to InfluxDB Cloud. The laptop just needs any internet connection — it doesn't need to be near the Pi.

---

## Adding New Signals

1. Add the **exact DBC signal name** to `IMPORTANT_SIGNALS` in `logger.py`.
2. If the signal comes from Inv1/Inv2, no extra work needed — the inverter tag is added automatically based on CAN ID.
3. Add a corresponding Flux query panel to `grafana_dashboard.json` (or add it in the Grafana UI and export).

To add a new fault signal, add an entry to `FAULT_SIGNALS` in `logger.py` with `(system, description, severity)`. The signal must be a boolean bit (0/1) decoded from a message already in `TARGET_IDS`.

---

## Go Bindings (legacy notes)

The Go files (`main.go`, `can.go`, `heartbeat.go`, `telemetry.go`) implement a Go-based CAN reader for the heartbeat system. The DBC bindings are generated with:

```bash
go get -u go.einride.tech/can
cd dbc
go run go.einride.tech/can/cmd/cantool generate ./ ../generated/
```

Note: the `Reset` message was removed from DashCommand in the DBC to avoid name conflicts in the generated Go bindings.
