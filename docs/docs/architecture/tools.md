# Build and tools

Everything needed to build, test, flash and observe the car lives in the
repository. The four ECU projects share the same build model; the supporting
tools are separate.

## Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/) (`pio`) for the
  ECU firmware.
- Python 3.13+ for the CAN tools and documentation.
- An ST-Link for wired flashing.

PlatformIO downloads the STM32 toolchain, framework and libraries on the first
build.

## Build and upload an ECU

Run PlatformIO from the project directory. Each production project has a
`vehicle` environment for the real board:

| Project | PlatformIO board | Environment |
|---|---|---|
| `projects/front_controller` | Nucleo-F767ZI | `vehicle` |
| `projects/lvcontroller` | Nucleo-F767ZI | `vehicle` |
| `projects/dashboard` | STM32F469I Discovery | `vehicle` |
| `projects/tms` | Nucleo-F767ZI | `vehicle` |

The board names are PlatformIO definitions. The FC, LV controller and TMS run
on custom STM32F767 boards; only the dashboard is a Discovery kit.

```bash
cd projects/front_controller
pio run -e vehicle              # build
pio run -e vehicle -t upload    # build and flash
```

The build regenerates CAN headers, compiles the CubeMX startup code and links
the shared libraries through the project's `lib/` symlinks. The Git hash is
embedded at build time and sent over CAN by the FC, LV controller and TMS. The
dashboard defines a Git-hash message but does not send it.

Demo projects under `projects/demo/` have additional host environments
(such as `cli` and `linux`) for teaching and simulation. They are not part of
the vehicle firmware.

## Tests

Host-side unit tests live in the library's `test/` folder and run in the
library's `native` environment:

```bash
pio test --project-dir lib/lookup_table
pio test --project-dir lib/moving_average
pio test --project-dir lib/sls095
```

The tests cover numerical code such as lookup tables, moving averages and the
SLS095 encoder. There are no automated hardware-in-the-loop tests in this
repository.

Continuous integration builds every project with a `vehicle` environment and
runs the tests of every library that has a `test/` folder and a
`platformio.ini`.

## Static analysis

`pio check` is configured to use `clang-tidy`:

```bash
pio check --project-dir projects/front_controller \
  --flags "--checks=bugprone-*,clang-analyzer-*,google-*,performance-*" \
  --fail-on-defect=high
```

`scripts/static_analysis/static_analysis.sh` runs the same check for projects
with changed files.

## CAN code generation

`scripts/cangen` reads a project's `config.yaml` and the DBC files, then
generates C++ message and bus classes into `Inc/generated/can/`.

```yaml
canGen:
  outputPath: Inc/generated/can
  busses:
    - name: veh
      node: FC
      dbcFile: "../veh.dbc"
```

`cangen` is registered as a PlatformIO library dependency, so the headers are
regenerated automatically before every build. To regenerate by hand:

```bash
cd scripts/cangen
poetry install
poetry run cangen ../../projects/front_controller
```

The generated code is written for one node at a time. The front controller
gets vehicle and powertrain messages; the LV and TMS get vehicle messages
only.

## Watch the CAN buses

The two monitor scripts configure a SocketCAN interface at 500 kbit/s and open
`cantools monitor` with the matching DBC:

```bash
cd projects
bash monitor_veh.sh    # vehicle bus, can1, veh.dbc
bash monitor_pt.sh     # powertrain bus, can0, pt.dbc
```

They require Linux, `iproute2` and `cantools`. The interface names `can0` and
`can1` are local conventions. The DAQ setup uses the opposite naming, so check
which channel is wired to which bus before trusting a dump.

`scripts/can_errgo/` provides a terminal interface for watching and
acknowledging CAN errors on a virtual or physical interface.

## Flash over CAN

`scripts/canflash/` runs a web server on the Raspberry Pi inside the car. With
the Pi powered and on its local network, open `http://192.168.12.1:8000/`,
choose an ECU, choose a firmware binary and start the flash.

- Supported targets in the web interface: front controller, LV controller, TMS.
- The dashboard is not flashed through this tool.
- Flashing uses the STM32 CAN bootloader; the Pi controls each ECU's boot pin.
- In this revision only the TMS application checks for the CAN-flash request,
  so validate the other two targets before relying on them.
- The server starts automatically on the Pi. See `scripts/canflash/README.md`
  for running it locally.

## Data acquisition

`scripts/daq/` is the separate telemetry stack that runs on the Raspberry Pi:

```text
vehicle + powertrain CAN
        │
        ▼
logger.py → CSV on the SD card
        └──→ InfluxDB Cloud → Grafana on the dock laptop
```

It reads raw frames, decodes selected signals with the same DBC files, writes
decoded rows to CSV and uploads key values over cellular. See
`scripts/daq/README.md` for the Pi setup, the Linux VM test setup and the
Grafana dashboard. There are two installers: `setup_pi.sh` for the cloud
deployment and `setup.sh` for a local InfluxDB/Grafana install.

The DAQ is not part of the ECU firmware and does not affect vehicle control.

## Documentation

The site is built with MkDocs Material:

```bash
cd docs
.venv/bin/mkdocs serve     # live preview at http://127.0.0.1:8000
.venv/bin/mkdocs build     # generate site/ for deployment
```

Install the Python requirements from `docs/requirements.txt` if the virtual
environment does not exist yet. The GitHub Actions workflow builds and deploys
the site when changes reach `main`.
