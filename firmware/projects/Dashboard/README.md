# Dashboard

We used LVGL since it is open source and can compile for STM32 and PC.

Make sure you have all of the git submodules

```bash
git submodule update --init --recursive
```

## STM32

We have the STM32F469I-DISCO board.

## Linux

You will need SDL2 to emulate.

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

To run, enable the vcan interface then run the executable.

```bash
source platforms/lnx/vcan_setup.sh
./build/Dashboard/lnx/main
```
