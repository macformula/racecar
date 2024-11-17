# To test the virtual can and go script

Kill all existing instances of ip-links:

`ip link show`
`sudo ip link delete <ip-link>`

Setup the can network:

`./setup_vcan.sh <CAN_PORT>`

Start the CLI viewer:
`go run error_tui.go -i <CAN_PORT>`

You can now run:
`cansend <CAN_PORT>  <MSG_ID>#<MSG_DATA>`
