# CAN Error TUI

This is a Tangible User Interface program designed to track CAN Errors including their count, recency, description, and name.

Users can:

- Acknowledge the error ( Hide and resets the error until it is sent again)
- Ignore the error ( Hides, resets, and stops reading all future occurrences of the error)
- View ignored errors and choose to unignore those errors

## Setup

Install required dependencies:

```bash
sudo  apt-get  update
sudo  apt-get  upgrade
sudo  apt  install  net-tools  iproute2  can-utils  linux-modules-extra-$(uname  -r)
```

Kill all existing instances of ip-links:

```bash
ip  link  show
sudo  ip  link  delete <ip-link>
```

Setup the can network:

```bash
./setup_vcan.sh <CAN_PORT>
```

## Usage

Start the CLI viewer for single use:

```bash
go  run  error_tui.go  -i <CAN_PORT> -w <WARN_TIMER>
```

OR

Build the go file:

```bash
go build -o <BUILD_NAME>
./<BUILD_NAME> -i <CAN_PORT> -w <WARN_TIMER>
```

You can now run:

```bash
cansend <CAN_PORT> <MSG>
```
