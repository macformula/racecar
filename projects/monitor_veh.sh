# Only works on Linux
# Requires cantools: pip install cantools
# Connect Channel 2 of the Kvaser to Vehicle CAN

sudo ip link set can1 type can bitrate 500000
sudo ip link set can1 up

cantools monitor -c can1 --interface socketcan veh.dbc
