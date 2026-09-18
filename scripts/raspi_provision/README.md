# Racecar Pi setup

`provision.sh` updates the pi, configures the CAN hat, starts DAQ on boot, and creates the `macfe-raspberry-pi` hotspot. Pi must be set up with os before running script.

## 1. Install Raspberry Pi OS

Use [Raspberry Pi Imager](https://www.raspberrypi.com/software/) to select pi model and Raspberry Pi OS Lite (64-bit). Set:

| Setting | Value |
| --- | --- |
| Username | `macformula` |
| Hostname | `racecar` |
| Password | Your team-managed login password |
| SSH | Enabled with password authentication |
| Wi-Fi country / timezone | `CA` / America/Toronto |
| Internet | Temporary Wifi, or connect Ethernet |

Write and verify the SD card (imager will erase it so make sure it's empty). Insert it and attach the HAT while the pi is powered off, then boot. Confirm password login from your laptop:

```bash
ssh -o PubkeyAuthentication=no -o PreferredAuthentications=password macformula@racecar.local
```
 Finish this setup before running the script; it assumes the account, hostname, and SSH already work.

## 2. Run provisioning script

On your laptop, from the repository root:

```bash
COPYFILE_DISABLE=1 tar -czf /tmp/macfe-pi.tar.gz -C scripts \
    raspi_provision/provision.sh \
    daq/logger.py daq/requirements.txt daq/dbc/veh.dbc daq/dbc/pt.dbc
scp /tmp/macfe-pi.tar.gz macformula@racecar.local:~/
ssh macformula@racecar.local
```

On the Pi, with internet connected:

```bash
mkdir -p ~/macfe-pi
tar -xzf ~/macfe-pi.tar.gz -C ~/macfe-pi
cd ~/macfe-pi/raspi_provision
sudo ./provision.sh
```

Enter a hotspot password. This is separate from your login password. Wait for setup to finish, then:

```bash
sudo reboot
```

If setup fails, fix reported error and rerun. The complete boot config is embedded in the script; the first backup can be found at `/boot/firmware/config.txt.pre-macfe`.

## 3. Connect and check

Join 'macfe-raspberry-pi' on your laptop using the hotspot password, then:

```bash
ssh macformula@racecar.local
# If the name fails:
ssh macformula@192.168.12.1
```

The hotspot and local logging work without internet. CSV logs are stored in
`/opt/macfe/daq/logs/`. On the Pi, check:

```bash
systemctl status macfe-can macfe-daq --no-pager
journalctl -b -u macfe-can -u macfe-daq --no-pager
ls -l /opt/macfe/daq/logs/
```

Both CAN interfaces must be present and verify incoming CAN data produces CSVs and SSH works with internet disconnected.

## Optional internet and cloud upload

Wifi configured in Imager provides internet through `wlan0` during setup.
After reboot, `wlan0` hosts the hotspot instead. For internet alongside the
hotspot, use Ethernet or a second usb wifi adapter. For personal WPA wifi,
replace `wlan1` below with the second adapter’s name from `nmcli device status`:

```bash
sudo nmcli --ask device wifi connect 'INTERNET_SSID' ifname wlan1
```

For cloud telemetry, set the team credentials with `sudoedit /etc/macfe/daq.env`,
then run `sudo systemctl restart macfe-daq`. Otherwise, capture stays in local
CSVs under `/opt/macfe/daq/logs/`.
