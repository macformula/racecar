sudo modprobe -a can can_raw vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0