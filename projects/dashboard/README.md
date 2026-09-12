# Formula Electric Dashboard

A racing dashboard built with **LVGL 9.3** for STM32 hardware and Linux development.

---

## First Time Setup (Do Once)

```bash
./setup.sh
```

Enter your password when asked. Wait ~2 minutes.

**What this installs:**

- Git submodules (lv_drivers)
- Python3, pip3, and python3-venv
- Build tools (gcc, make, etc.)
- PlatformIO (build system)
- X11 development headers (for Linux display)
- CAN utilities (for SocketCAN interface)
- SDL2 (needed for displaying the GUI on Linux)
- Python CAN libraries (numpy, cantools, python-can)
- Virtual CAN network interface

---

### Running the Dashboard (Every Time)

## Option 1: Single Command (Recommended)

./run.sh

## Option 2: Two Terminals (Manual)**

```text

┌─────────────────────────────┐  ┌─────────────────────────────┐
│   TERMINAL 1: Simulator     │  │   TERMINAL 2: Dashboard     │
├─────────────────────────────┤  ├─────────────────────────────┤
│                             │  │                             │
│ $ ./run_simulator.sh        │  │ $ ./run_dashboard.sh        │
│                             │  │                             │
│ Waiting for Profile         │  │ [GUI Window Opens]          │
│ Selection...                │  │                             │
│                             │  │ (See Keyboard Controls      │
│ (Leave this running)        │  │  section below)             │
│                             │  │                             │
└─────────────────────────────┘  └─────────────────────────────┘
```

**That's it!** The GUI window will appear.

### Keyboard Controls

When the dashboard GUI window is open:

- **Tab** - Scroll through options
- **Space** - Select/Confirm/Enter
- **Ctrl+C** (in terminal) - Quit the dashboard
- **X button** (on window) - Close the dashboard window

---

## After Rebooting Your Computer

The CAN interface needs to be set up again. Just run:

```bash
sudo src/platforms/linux/socketcan_setup.sh
```

Then use `./run_simulator.sh` and `./run_dashboard.sh` as normal.

---

## Quick Reference (Already Set Up)

If you've already run setup before:

**Two Terminals (Recommended):**

```bash
# Terminal 1
./run_simulator.sh

# Terminal 2
./run_dashboard.sh
```

**Single Terminal (Both in one):**

```bash
./run.sh
```

---

## What You Need Before Setup

Make sure these are installed **before** running `./setup.sh`:

1. **PlatformIO** - For building the project

   ```bash
   pip install platformio
   ```

   Or see: <https://platformio.org/install/cli>

2. **Python 3** - Usually pre-installed on Ubuntu/WSL2

   ```bash
   python3 --version  # Should show 3.8 or newer
   ```

3. **X11 Server** (WSL2 users only)
   - **Windows 11:** Already built-in!
   - **Windows 10:** Install [VcXsrv](https://sourceforge.net/projects/vcxsrv/) or Xming

Everything else (SDL2, Python packages, CAN tools) is installed automatically by `./setup.sh`

**Why do we need SDL2?**  
SDL2 is what creates the GUI window on Linux/WSL2. LVGL 9.3 has a built-in SDL driver that we use to display the dashboard. On the actual STM32 hardware, we use the built-in LCD instead.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `No such device vcan0` | Run: `sudo src/platforms/linux/socketcan_setup.sh` |
| Dashboard window doesn't appear | Make sure X11 is running (WSL2) |
| Simulator won't start | Run: `source venv/bin/activate` first |
| Build errors | Make sure PlatformIO is installed: `pio --version` |
| Permission denied on scripts | Run: `chmod +x *.sh` |

---

## Technical Details

### LVGL Version

This project uses **LVGL 9.3**. See docs at <https://docs.lvgl.io/9.3/>

### Platforms

**STM32:** STM32F469I-DISCO board with built-in LCD display

**Linux/WSL2:** SDL2-based emulation for development and testing

### Architecture

- `src/` - Screen implementations and main program
- `include/` - Header files
- `src/platforms/linux/` - Linux-specific platform code (SDL, SocketCAN)
- `src/platforms/stm32/` - STM32-specific platform code

---

## Development

### To Add a New Screen

Use the existing screens in `include/` and `src/` as examples.

1. Create a header and source file, eg. `include/DemoScreen.hpp` and `src/DemoScreen.cpp`.

2. In the header file, define the `DemoScreen` class and publicly inherit `Screen`. Declare overrides for the virtual functions from `include/Screen.hpp`. Declare the constructor as:

    ```cpp
    public:
        DemoScreen(Display* Display);
    ```

3. Add the screen to the high level `Display`:
    1. Add a `DemoScreen` member to the `include/Display.hpp` class and construct it in the initializer list in the `src/Display.cpp` constructor.
    2. Define an enum value for DemoScreen by adding a new value to the `State` enum in `projects/veh.dbc`.
    3. Edit the `Display::InnerChangeState` function in `src/Display.cpp` to switch to this screen when the new enum value is passed in.

4. Implement all methods in the `src/*.cpp` file:
    1. Create the GUI elements in the `CreateGUI()` method. Use the `frame_` member as the top-level lvgl object.
    2. Implement screen behaviour in the `Update()` method. This includes screen transitions and updating GUI fields.
        - The only external inputs are the ENTER / SCROLL buttons and the vehicle CAN bus. You can access these objects through the `display_` parent object.
        - For select/button events, you should likely use the `.PosEdge()` event for your select / scroll button events. See `include/Button.hpp` for other events.
        - To read / send CAN messages, use the reference to the CAN bus object with `display_->veh_bus`. Don't forget to verify that the optional message has a value!
        - To change screen, call `display_->ChangeState(state)` and pass in the enum value associated to this state in `Display::InnerChangeState()`.

### Files Overview

- `setup.sh` - One-time setup script (installs all dependencies)
- `run.sh` - Run both simulator and dashboard (recommended)
- `run_simulator.sh` - Start only the FrontController simulator
- `run_dashboard.sh` - Build and run only the dashboard
- `src/main.cc` - Main entry point
- `src/Display.cpp` - State machine and screen management
- `src/Screen.cpp` - Base class for all screens
- `src/platforms/linux/bindings.cc` - Linux platform initialization (SDL, CAN)
- `src/platforms/linux/lv_conf.h` - LVGL 9.3 configuration
