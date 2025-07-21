# Dashboard

We used LVGL since it is open source and can compile for STM32 and PC.

Make sure you have all of the git submodules

```bash
git submodule update --init --recursive
```

See the LVGL docs <https://docs.lvgl.io/8.3/>. Make sure to read the v8.3 docs since there were a lot of changes in v9.

## STM32

We have the STM32F469I-DISCO board.

## Linux

You will need SDL2 to emulate. `TAB` and `SPACE` keys replace the SCROLL and ENTER buttons.

```bash
git clone https://github.com/libsdl-org/SDL
cd SDL
git checkout SDL2
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -CMAKE_INSTALL_PREFIX=/usr/local
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

If you are using WSL2, you will also need to install X11 to show the SDL2 screen on your Windows monitor. Dashboard uses CAN, so make sure you have CAN enabled <https://macformula.github.io/racecar/tutorials/wsl-can/>.

Then, compile with

```bash
make PROJECT=Dashboard PLATFORM=lnx
```

The platform name is `lnx` (not `linux`) because C/C++ consider `linux` a keyword which breaks imports.

Enable the SocketCAN interface then run the executable. In another terminal, run the FrontController mock script to provide replies to the Dashbaord's CAN messages.

```bash
source platforms/lnx/socketcan_setup.sh
./build/Dashboard/lnx/main
# In a separate terminal
python3 platforms/lnx/fc_sim.py
```

## Development

### To add a new screen

Use the existing screens in `inc/` and `src/` as examples.

1. Create a header and source file, eg. `inc/DemoScreen.hpp` and `src/DemoScreen.cc`.
2. In the header file, define the `DemoScreen` class and publicly inherit `Screen`. Declare overrides for the virtual functions from `inc/Screen.hpp`. Declare the constructor as

    ```c++
    public:
        DemoScreen(Display* Display);
    ```

3. Add the screen to the high level `Display`:
    1. Add a `DemoScreen` member to the `inc/Display.hpp` class and construct it in the initializer list in the `src/Display.cc` constructor.
    2. Define an enum value for DemoScreen by adding a new value to the `State` enum in `projects/veh.dbc`.
    3. Edit the `Display::ChangeState` function in `src/Display.cc` to switch to this screen when the new enum value is passed in.

4. Implement all methods in the `src/*.cc` file.

    1. Create the GUI elements in the `CreateGUI()` method. Use the `frame_` member as the top-levl lvgl object.
    2. Implement screen behaviour in the `Update()` method. This includes screen transitions and updating GUI fields.
        - The only external inputs are the ENTER / SCROLL buttons and the vehicle CAN bus. You can access these objects through the `display_` parent object.
        - For select/button events, you should likely use the `.PosEdge()` event for your select / scroll button events. See `inc/Button.hpp` for other events.
        - To read / send CAN messages, use the reference to the CAN bus object with `display_->veh_bus()`. Don't forget to verify that the optional message has a value!
        - To change screen, call `display_->ChangeState(state)` and pass in the enum value associated to this state in `Display::ChangeState()`.
