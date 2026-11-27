# Using VS Code and PlatformIO

## Open a project

Open the PlatformIO Home page from the VS Code status bar (:material-home:) or
with ++f1++ → `PlatformIO: PlatformIO Home`, then choose **Open Project** and
select a folder under `projects/`.

For the car firmware, open the ECU you are working on, for example
`projects/front_controller`. Each production project contains:

```text
front_controller/
├── Src/              application and CubeMX source
├── Inc/              headers and generated CAN code
├── lib/              symlinks to the shared libraries
├── config.yaml       CAN generation settings
└── platformio.ini    build environments
```

The application entry point is `Src/app.cc`. See
[Software layout](../../architecture/software.md) for the full repository
structure.

## Select an environment

Use the PlatformIO status-bar menu to choose an environment:

- **`vehicle`** builds for the real board and is the only environment used by
  the four production ECUs.
- Demo projects under `projects/demo/` may also offer host environments such as
  `cli` or `linux`. These are teaching and simulation aids.

## Build and upload

Use the PlatformIO buttons in the status bar:

- :material-check: **Build** compiles the selected environment.
- :material-arrow-right: **Upload** flashes the board over ST-Link.

The same actions from a terminal:

```bash
cd projects/front_controller
pio run -e vehicle
pio run -e vehicle -t upload
```

A build regenerates the CAN headers from the DBC files and embeds the current
Git hash. The first build installs the toolchain and libraries and is slower
than later builds.

To connect an ST-Link to a vehicle ECU, see
[Uploading firmware to STM](../flashing/index.md).
