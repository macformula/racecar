# Data Logging & Processing

## Setup
1. Ensure your Linux or WSL environment has SocketCAN enabled (Documentation WIP)

2. Run `./vcan_setup.sh` to setup a virtual CAN interface.

3. Run `go run can_recieve.go <INTERFACE NAME>`. where `<INTERFACE NAME>` is the virtual CAN interface name. If `./vcan_setup.sh` was executed, `<INTERFACE NAME>` will be `vcan0`.

4. Test the connection with by executing `cansend vcan0 01a#11223344AABBCCDD` or `cangen vcan0 -v` within another window to send continuous CAN frames.