#!/usr/bin/env bash

# Racecar Pi setup (CAN, data logger, and wi-Fi hotspot)
#
# Requires Raspberry Pi OS Lite (64bit) installed, macformula user, racecar hostname,
# password SSH, and internet access. See README.md for OS setup.
#
# Run on the Pi: sudo ./provision.sh, then sudo reboot.

# ------------------------------------------------------------------------
# config

WIFI_COUNTRY=CA
CAN_BITRATE=500000 # 500 kbit/s on both buses

# Set watch for errors
set -Eeuo pipefail
trap 'printf "Setup failed at line %s\n" "$LINENO" >&2' ERR
fail() {
    printf 'ERROR: %s\n' "$*" >&2
    exit 1
}

# Get DAQ source and destination paths (ran in daq repo)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DAQ_SOURCE="${SCRIPT_DIR}/../daq"
DAQ_DIR=/opt/macfe/daq
AP_PROFILE=/etc/NetworkManager/system-connections/macfe-hotspot.nmconnection

# Check status before starting
[[ ${EUID} -eq 0 ]] || fail 'run with sudo enabled' # sudo check
id macformula >/dev/null 2>&1 || fail 'create macformula user in imager' # user check
[[ $(hostname) == racecar ]] || fail 'set hostname to racecar in imager' # hostname check
[[ -f /boot/firmware/config.txt ]] || fail 'missing /boot/firmware/config.txt' # config.txt check
[[ -d /sys/class/net/wlan0/wireless ]] || fail 'wlan0 wifi interface is missing' # wlan0 check
[[ ${WIFI_COUNTRY} =~ ^[A-Z]{2}$ ]] || fail 'WIFI_COUNTRY must be two uppercase letters' # WIFI_COUNTRY check
[[ ${CAN_BITRATE} =~ ^[1-9][0-9]*$ ]] || fail 'CAN_BITRATE must be a positive integer' # CAN_BITRATE check
for file in logger.py requirements.txt dbc/veh.dbc dbc/pt.dbc; do
    [[ -f ${DAQ_SOURCE}/${file} ]] || fail "missing DAQ file: ${DAQ_SOURCE}/${file}" # DAQ file check
done

# Ask for hotspot password
read -r -s -p 'Hotspot password: ' AP_PASSWORD
printf '\n'
[[ ${AP_PASSWORD} =~ ^[A-Za-z0-9._-]{8,63}$ ]] || fail 'invalid hotspot password'


# ------------------------------------------------------------------------
# script

# System packages
# Update packages first (preserve existing package configurations during update)
printf '\n[1/6] Updating OS and installing tools...\n'
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get -y \
    -o Dpkg::Options::=--force-confdef -o Dpkg::Options::=--force-confold full-upgrade
# Install CAN tools, python, and hotspot/name-discovery services
DEBIAN_FRONTEND=noninteractive apt-get install -y \
    -o Dpkg::Options::=--force-confdef -o Dpkg::Options::=--force-confold \
    can-utils iproute2 python3 python3-pip python3-venv \
    network-manager dnsmasq-base avahi-daemon raspi-config rfkill

# CAN HAT boot configuration
# Back up once, then replace the full file with OS defaults and HAT settings
printf '\n[2/6] Writing CAN HAT boot configuration...\n'
for overlay in mcp2515-can0 mcp2515-can1; do
    [[ -f /boot/firmware/overlays/${overlay}.dtbo ]] || fail "missing CAN overlay: ${overlay}"
done
if [[ ! -f /boot/firmware/config.txt.pre-macfe ]]; then
    cp /boot/firmware/config.txt /boot/firmware/config.txt.pre-macfe
fi

# Complete /boot/firmware/config.txt
# Defaults: https://github.com/RPi-Distro/pi-gen/blob/master/stage1/00-boot-files/files/config.txt
# Additions: explicit 64-bit mode and CAN settings
cat >/boot/firmware/config.txt <<'BOOT_CONFIG'
# Enable optional hardware interfaces
#dtparam=i2c_arm=on
dtparam=spi=on
dtparam=audio=on

# Detect cameras and displays
camera_auto_detect=1
display_auto_detect=1

# Load initramfs
auto_initramfs=1

# Graphics defaults
dtoverlay=vc4-kms-v3d
max_framebuffers=2
disable_fw_kms_setup=1

# 64-bit kernel and display defaults
arm_64bit=1
disable_overscan=1

# Allow maximum supported clock speed
arm_boost=1

[cm4]
otg_mode=1

[cm5]
dtoverlay=dwc2,dr_mode=host

[pi5]
dtoverlay=nospi10

[all]
# Dual MCP2515 HAT: 16 MHz oscillators, BCM interrupt GPIOs 23/25
dtoverlay=mcp2515-can0,oscillator=16000000,interrupt=23
dtoverlay=mcp2515-can1,oscillator=16000000,interrupt=25
# From issue #538; absent from current official overlays. SPI is enabled above.
dtoverlay=spi-bcm2835-overlay
BOOT_CONFIG
chmod 0644 /boot/firmware/config.txt

# Data logger
printf '\n[3/6] Installing the data logger...\n'
# Stop capture before updating code
if systemctl is-active --quiet macfe-daq; then
    systemctl stop macfe-daq
fi
# install DAQ files to /opt/macfe/daq with correct permissions
install -d -m 0755 "${DAQ_DIR}/dbc"
for file in logger.py requirements.txt dbc/veh.dbc dbc/pt.dbc; do
    install -m 0644 "${DAQ_SOURCE}/${file}" "${DAQ_DIR}/${file}"
done
# Allow macformula to write CSV logs
install -d -o macformula -g "$(id -gn macformula)" -m 0750 "${DAQ_DIR}/logs"
# Create python venv
python3 -m venv "${DAQ_DIR}/venv"
"${DAQ_DIR}/venv/bin/python" -m pip install --upgrade pip
"${DAQ_DIR}/venv/bin/python" -m pip install -r "${DAQ_DIR}/requirements.txt"

# Create optional cloud settings once. A blank token keeps logging local.
install -d -m 0755 /etc/macfe
if [[ ! -f /etc/macfe/daq.env ]]; then
    (
        umask 077 # Root-only credentials
        cat >/etc/macfe/daq.env <<'DAQ_ENV'
INFLUX_URL=https://us-east-1-1.aws.cloud2.influxdata.com
INFLUX_TOKEN=
INFLUX_ORG=macformula
INFLUX_BUCKET=macfe
DAQ_ENV
    )
fi
chmod 0600 /etc/macfe/daq.env

# Boot services
# Configure both CAN buses before DAQ
printf '\n[4/6] Creating CAN and DAQ boot services...\n'
IP_COMMAND="$(command -v ip)"
cat >/etc/systemd/system/macfe-can.service <<CAN_SERVICE
[Unit]
Description=Configure racecar CAN buses
Requires=sys-subsystem-net-devices-can0.device sys-subsystem-net-devices-can1.device
After=sys-subsystem-net-devices-can0.device sys-subsystem-net-devices-can1.device
Before=macfe-daq.service

[Service]
Type=oneshot
ExecStartPre=${IP_COMMAND} link show can0
ExecStartPre=${IP_COMMAND} link show can1
ExecStart=${IP_COMMAND} link set can0 down
ExecStart=${IP_COMMAND} link set can0 type can bitrate ${CAN_BITRATE} restart-ms 100
ExecStart=${IP_COMMAND} link set can0 up
ExecStart=${IP_COMMAND} link set can1 down
ExecStart=${IP_COMMAND} link set can1 type can bitrate ${CAN_BITRATE} restart-ms 100
ExecStart=${IP_COMMAND} link set can1 up
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
CAN_SERVICE

# Start DAQ as macformula after CAN setup
cat >/etc/systemd/system/macfe-daq.service <<'DAQ_SERVICE'
[Unit]
Description=Racecar data acquisition
Requires=macfe-can.service
After=macfe-can.service

[Service]
Type=simple
User=macformula
WorkingDirectory=/opt/macfe/daq
EnvironmentFile=/etc/macfe/daq.env
Environment=PYTHONUNBUFFERED=1
Environment=PYTHON_DOTENV_DISABLED=1
ExecStart=/opt/macfe/daq/venv/bin/python /opt/macfe/daq/logger.py
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
DAQ_SERVICE
chmod 0644 /etc/systemd/system/macfe-{can,daq}.service

# Wifi hotspot
# Use wlan0 for the hotspot
printf '\n[5/6] Configuring macfe-raspberry-pi wifi...\n'
raspi-config nonint do_wifi_country "${WIFI_COUNTRY}"
rfkill unblock wifi
nmcli radio wifi on
install -d -m 0700 /etc/NetworkManager/system-connections
# Preserve the connection ID on reruns
AP_UUID="$(sed -n 's/^uuid=//p' "${AP_PROFILE}" 2>/dev/null || true)"
[[ -n ${AP_UUID} ]] || AP_UUID="$(cat /proc/sys/kernel/random/uuid)"
# Write hotspot profile with root-only permissions (to hide password)
(
    umask 077
    cat >"${AP_PROFILE}.tmp" <<WIFI_PROFILE
[connection]
id=macfe-hotspot
uuid=${AP_UUID}
type=wifi
interface-name=wlan0
autoconnect=true
# Prefer the hotspot at boot.
autoconnect-priority=999
autoconnect-retries=0

[wifi]
mode=ap
ssid=macfe-raspberry-pi
band=bg
channel=6
powersave=2

[wifi-security]
key-mgmt=wpa-psk
proto=rsn
psk=${AP_PASSWORD}

[ipv4]
# Fixed Pi address with DHCP/DNS for laptops
address1=192.168.12.1/24
method=shared
never-default=true

[ipv6]
method=disabled
WIFI_PROFILE
)
chmod 0600 "${AP_PROFILE}.tmp"
mv "${AP_PROFILE}.tmp" "${AP_PROFILE}"
# Add local DNS for racecar
install -d -m 0755 /etc/NetworkManager/dnsmasq-shared.d
printf 'host-record=racecar,192.168.12.1\n' >/etc/NetworkManager/dnsmasq-shared.d/macfe-hotspot.conf
chmod 0644 /etc/NetworkManager/dnsmasq-shared.d/macfe-hotspot.conf

# Enable services
# Reboot to load the CAN overlays and start capture.
printf '\n[6/6] Enabling services for the next boot...\n'
systemctl daemon-reload
systemctl enable macfe-can.service macfe-daq.service NetworkManager
systemctl enable --now avahi-daemon

printf "\nSetup complete, run 'sudo reboot'\n"
printf 'Then join macfe-raspberry-pi and run: ssh macformula@racecar.local\n'
printf 'IP fallback: ssh macformula@192.168.12.1\n'
printf 'CSV logs: /opt/macfe/daq/logs/\n'
printf 'Optional cloud credentials: sudoedit /etc/macfe/daq.env\n'
