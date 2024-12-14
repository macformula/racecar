# Demo Bitset Project

This is for Tamara's development on the CAN bitset issue.

Compile from the `racecar/firmware/` directory:

```bash
make PROJECT=Demo/Bitset PLATFORM=cli
```

Run the tests with

```bash
./build/Demo/Bitset/cli/main
```

## Development

The unpack / pack methods are in `firmware\shared\comms\can\msg.hpp`

You should add more messages / signals to the DBC. Edit the existing message & signal structure in `bitset.dbc`. Also see <https://www.csselectronics.com/pages/can-dbc-file-database-intro>

Test more complex packing like floats and numbers which have strange bit-lengths in the CAN frame.
