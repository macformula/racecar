#!/bin/bash
# Create Access Point and Local Wifi Network on Raspberry Pi

# Check for 2 inputs
if [ "$#" -ne 2 ]; then
    echo "Usage: bash ./setup.sh 'ssid' 'password'"
    exit 1
fi

# Extract inputs
ssid=$1
password=$2

# Create Python virtual environment
cd /home/macformula/racecar/scripts/canflash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

# Get latest dependencies
sudo apt update

# Install dependencies
sudo apt install -y libgtk-3-dev build-essential gcc g++ pkg-config make hostapd libqrencode-dev libpng-dev iptables

# Clone repo
cd ~
git clone https://github.com/lakinduakash/linux-wifi-hotspot
cd linux-wifi-hotspot

# Build binaries
make

# Install
sudo make install

# Create access point
sudo iw dev wlan0 interface add ap0 type __ap

# Create new wifi network
sudo pkexec --user root create_ap ap0 wlan0 "$ssid" "$password" --mkconfig /etc/create_ap.conf --freq-band 2.4

# Create a service to initialize access point and network on boot
sudo tee /etc/systemd/system/create_ap.service > /dev/null <<EOF
[Unit]
Description=Create AP Service
After=network.target

[Service]
Type=sim
ExecStartPre=/sbin/iw dev wlan0 interface add ap0 type __ap
ExecStart=/usr/bin/create_ap --config /etc/create_ap.conf
KillSignal=SIGINT
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

# Create a service to run the server on boot
sudo tee /etc/systemd/system/canflash.service > /dev/null <<EOF
[Unit]
Description=CanFlash Service
After=network.target

[Service]
Type=sim
WorkingDirectory=/home/macformula/racecar/scripts/canflash
ExecStart=.venv/bin/python3 main.py
KillSignal=SIGINT
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

# Create and enable the services
sudo systemctl daemon-reload
sudo systemctl enable create_ap
sudo systemctl enable canflash

# Restart the services
sudo systemctl restart create_ap
sudo systemctl restart canflash

# Reboot the system for changes to take effect
sudo reboot

# You should now see the new network emmitted from the Raspberry Pi
# It will still connect to known internet networks while still emitting this new one
# Connect using the default IP address 192.168.12.1 
# i.e. ssh macfe@192.168.12.1
