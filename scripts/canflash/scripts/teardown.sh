#!/bin/bash
# Teardown Access Point and Local Wifi Network on Raspberry Pi

# Stop and disable the service
echo "Stopping and disabling create_ap service..."
sudo systemctl stop create_ap
sudo systemctl disable create_ap
sudo systemctl daemon-reload

# Remove the service file
echo "Removing create_ap service file..."
sudo rm -f /lib/systemd/system/create_ap.service

# Remove installed packages
echo "Removing installed packages..."
sudo apt remove --purge -y libgtk-3-dev build-essential gcc g++ pkg-config make hostapd libqrencode-dev libpng-dev iptables
sudo apt autoremove -y
sudo apt clean

# Remove linux-wifi-hotspot repository
echo "Removing linux-wifi-hotspot repository..."
cd ~
sudo rm -rf linux-wifi-hotspot

# Remove the access point interface
echo "Removing access point interface..."
sudo iw dev ap0 del

# Delete the create_ap configuration file
echo "Deleting create_ap configuration file..."
sudo rm -f /etc/create_ap.conf

# Reboot the system for changes to take effect
sudo reboot
