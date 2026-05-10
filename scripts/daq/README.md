# DAQ — Data Acquisition System

Real-time CAN telemetry for the MAC Formula Electric racecar.
Data flows from two CAN buses on the Raspberry Pi → InfluxDB → Grafana dashboard.

---

## Architecture

```
Racecar CAN buses
  can0  (Vehicle bus — FC, LVC, TMS, BMS, GPS)
  can1  (Powertrain bus — Inverter 1, Inverter 2)
         │
         ▼
  Raspberry Pi 4
  ┌─────────────────────────────────┐
  │  candump -L can0 / can1         │  raw CAN frames
  │       │                         │
  │  logger.py                      │  decode via veh.dbc + pt.dbc
  │       │                         │
  │  CSV logs  (logs/can_log_*.csv) │  full signal archive
  │  InfluxDB  (localhost:8086)     │  key signals + faults
  └─────────────────────────────────┘
         │
         ▼ (network / same machine)
  Grafana  (localhost:3000 or Grafana Cloud)
  ┌─────────────────────────────────┐
  │  Vehicle Safety row             │
  │  Performance row                │
  │  Segment Temperatures row       │
  └─────────────────────────────────┘
```

---


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

The script:
1. Installs `python3`, `can-utils`, `influxdb2`, and Grafana from the bundled `.deb`
2. Enables InfluxDB and Grafana as systemd services (auto-start on boot)
3. Brings up `can0` and `can1` at 500 kbit/s
4. Creates a Python virtualenv and installs requirements

After running `setup.sh`:

1. Open `http://localhost:8086` → complete InfluxDB setup:
   - Username: `admin`, Password: (choose one)
   - Organization: `macformula`
   - Bucket: `macfe`
   - Generate an **All Access API token**

2. Create `.env` in this directory:
   ```
   INFLUX_URL=http://localhost:8086
   INFLUX_TOKEN=<paste token here>
   INFLUX_ORG=macformula
   INFLUX_BUCKET=macfe
   ```

3. Open `http://localhost:3000` (admin/admin) → import `grafana_dashboard.json`

4. Start logging:
   ```bash
   source venv/bin/activate
   python logger.py
   ```

---

## Dev Setup (Windows — no Pi, no CAN bus)

Uses InfluxDB Cloud (free tier) and Grafana Cloud instead of local installs.

1. Create accounts at `cloud2.influxdata.com` and `grafana.com`
2. In InfluxDB Cloud: create bucket `macfe`, generate All Access token
3. Create `.env`:
   ```
   INFLUX_URL=https://<your-region>.aws.cloud2.influxdata.com
   INFLUX_TOKEN=<cloud token>
   INFLUX_ORG=<your org name>
   INFLUX_BUCKET=macfe
   ```
4. Install Python dependencies:
   ```bash
   python -m venv venv
   venv\Scripts\activate
   pip install -r requirements.txt
   ```
5. Run the simulated data writer to verify the connection:
   ```bash
   python test_influx.py
   ```
   Should print `Connection OK — test write succeeded.` and stream fake sensor values.

6. In Grafana Cloud: add InfluxDB datasource (Flux, same credentials as `.env`), then import `grafana_dashboard.json`.

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
