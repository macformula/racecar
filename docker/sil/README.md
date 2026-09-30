# SIL Docker Container

A minimal, reproducible container for building and running the `sil`
PlatformIO environment locally, without installing PlatformIO, Python, or a
C++ toolchain natively.

## Usage

Build the image (from repo root):

```bash
docker build -t racecar-sil docker/sil
```

Build the `lvcontroller` `sil` environment:

```bash
docker run --rm -v "$(pwd):/workspace" -w /workspace/projects/lvcontroller \
    racecar-sil pio run -e sil
```

Or drop into an interactive shell:

```bash
docker run --rm -it -v "$(pwd):/workspace" -w /workspace \
    racecar-sil bash
```

Once a `test/` directory exists for a `sil` environment (see [#309](https://github.com/macformula/racecar/issues/309)/[#310](https://github.com/macformula/racecar/issues/310)),
`pio test -e sil` will work the same way.

## Does this need SocketCAN / vcan?

No. `lib/mcal/sil/can.hpp`'s CAN implementation is currently a stub - it's
constructed with `"vcan0"` but never opens a real SocketCAN socket. Actual SIL
I/O runs over a plain TCP socket (COBS + nanopb) to an external harness
process, so no SocketCAN, kernel modules, `--privileged`, or
`--cap-add=NET_ADMIN` are required here. If you want the compiled SIL binary
to reach a harness on your own host machine (rather than a remote device),
use `--network host` (Linux) since the container has its own loopback
namespace.

This is unrelated to the [Virtual CAN on WSL2](https://macformula.github.io/racecar/tutorials/wsl-can/)
tutorial, which covers the separate `linux` platform's real SocketCAN setup
(used by the CAN demo/dashboard projects) - not this `sil` environment.

## Development

Other projects can add their own `[env:sil]` in their `platformio.ini` and
reuse this same image - no changes to `docker/sil/` are required for that.
