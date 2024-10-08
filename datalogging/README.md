# Data Logging & Processing

## Setup
1. Ensure your Linux or WSL environment has SocketCAN enabled (Documentation WIP).

2. Run `sudo ./vcan_setup.sh` to setup a virtual CAN interface.

3. Run `go run can_receive.go <INTERFACE NAME>`. where `<INTERFACE NAME>` is the virtual CAN interface name. If `./vcan_setup.sh` was executed, `<INTERFACE NAME>` will be `vcan0`.

4. Within another window, test the connection by executing `cansend vcan0 01a#11223344AABBCCDD` or `cangen vcan0 -v` to send continuous CAN frames.