To test the virtual can and go script:

kill all existing instances of can-dump, can-listen, and end ip-links:

ps aux | grep candump
ps aux | grep can-listen
ip link show


kill <can-dump-id>
kill <can-listen-id>
sudo ip link delete <ip-link>



rerun (for now make CAN_PORT vcan0):

./setup_vcan.sh <CAN_PORT>
candump -L <CAN_PORT> > <LOG_FILE> &
./can_errgo/can-listen &

You can now run:

cansend <CAN_PORT> <MSG_ID>#<MSG_DATA> 
