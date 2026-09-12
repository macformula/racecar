# Only works on Linux
# Requires cantools: pip install cantools
# Connect Channel 1 of the Kvaser to Powertrain CAN

sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up

cantools monitor -c can0 --interface socketcan pt.dbc
