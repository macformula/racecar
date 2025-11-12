#!/usr/bin/env bash
# ----------------------------------------------------------------------
# Provisioning script for a self-hosted GitHub Actions runner (Ubuntu)
#
# Purpose:
#   Install and configure a hardened, idempotent GitHub Actions runner
#   intended for SIL testing with optional SocketCAN support (virtual
#   vcan0 or physical CAN adapters).
#
# Key features:
#   - Installs runner binaries and registers the runner against a
#     repository using GH_RUNNER_TOKEN.
#   - Creates a least-privileged user `github-runner` (no sudo).
#   - Installs security tooling: ufw, fail2ban, apparmor, auditd,
#     unattended-upgrades, and systemd service hardening.
#   - Optional SocketCAN support:
#       CAN_TYPE=virtual  -> creates vcan0
#       CAN_TYPE=physical -> installs CAN utilities and serial CAN tools
#   - Installs Python tools useful for SIL: python-can, cantools.
#   - Applies AppArmor profile and systemd limits to mitigate malicious
#     code (fork bombs, disk flooding, privilege escalation).
#   - Idempotent: safe to run multiple times; performs checks before
#     making changes.
#
# Usage:
#   sudo GH_REPO_URL="https://github.com/OWNER/REPO" \
#        GH_RUNNER_TOKEN="REPLACE_ME" \
#        GH_RUNNER_NAME="sil-runner-01" \
#        GH_RUNNER_LABELS="self-hosted,linux,sil,socketcan" \
#        RUNNER_ARCH="x64" \
#        CAN_TYPE="virtual" \
#        bash provision_runner.sh
#
# RUNNER_ARCH options: x64, arm64, arm
# CAN_TYPE options: virtual, physical, none
#
# Required:
#   - Internet access to download packages and runner archive
#   - GH_RUNNER_TOKEN (from repository Settings → Actions → Runners)
# ----------------------------------------------------------------------

set -euo pipefail

# ---- Config from env ----
: "${GH_REPO_URL:?Set GH_REPO_URL to https://github.com/OWNER/REPO}"
: "${GH_RUNNER_TOKEN:?Set GH_RUNNER_TOKEN from GitHub Runners page}"
GH_RUNNER_NAME="${GH_RUNNER_NAME:-sil-runner-01}"
GH_RUNNER_LABELS="${GH_RUNNER_LABELS:-self-hosted,linux,sil}"
RUNNER_ARCH="${RUNNER_ARCH:-x64}"
CAN_TYPE="${CAN_TYPE:-none}"
RUNNER_VER="${RUNNER_VER:-2.329.0}"

# Validate architecture
case "$RUNNER_ARCH" in
  x64|arm64|arm) ;;
  *)
    echo "Invalid RUNNER_ARCH: $RUNNER_ARCH (must be x64, arm64, or arm)"
    exit 1
    ;;
esac

# Validate CAN type
case "$CAN_TYPE" in
  virtual|physical|none) ;;
  *)
    echo "Invalid CAN_TYPE: $CAN_TYPE (must be virtual, physical, or none)"
    exit 1
    ;;
esac

# ---- Base packages + security tools ----
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y curl tar jq git ca-certificates ufw fail2ban unattended-upgrades \
  apparmor apparmor-utils auditd

# CAN tooling (install if any CAN is needed)
if [ "$CAN_TYPE" != "none" ]; then
  apt-get install -y iproute2 can-utils || true
  
  # Python and CAN libraries for SIL
  apt-get install -y python3 python3-pip python3-venv || true
  pip3 install --break-system-packages python-can cantools || true
  
  # Verify SocketCAN kernel modules
  if ! lsmod | grep -q "^can"; then
    modprobe can || echo "Warning: CAN kernel module not available"
  fi
  if ! lsmod | grep -q "^can_raw"; then
    modprobe can_raw || echo "Warning: CAN_RAW kernel module not available"
  fi
  if ! lsmod | grep -q "^vcan"; then
    modprobe vcan || echo "Warning: VCAN kernel module not available"
  fi
  
  # Load CAN modules on boot
  cat >/etc/modules-load.d/can.conf <<'EOF'
can
can_raw
vcan
can_dev
EOF

  # Physical CAN additional tools
  if [ "$CAN_TYPE" = "physical" ]; then
    # Install slcand for serial CAN adapters
    apt-get install -y can-utils slcan-utils || true
  fi
fi

# ---- Enable automatic security updates ----
cat >/etc/apt/apt.conf.d/50unattended-upgrades <<'EOF'
Unattended-Upgrade::Allowed-Origins {
    "${distro_id}:${distro_codename}-security";
};
Unattended-Upgrade::AutoFixInterruptedDpkg "true";
Unattended-Upgrade::MinimalSteps "true";
Unattended-Upgrade::Remove-Unused-Kernel-Packages "true";
Unattended-Upgrade::Remove-Unused-Dependencies "true";
Unattended-Upgrade::Automatic-Reboot "false";
EOF

cat >/etc/apt/apt.conf.d/20auto-upgrades <<'EOF'
APT::Periodic::Update-Package-Lists "1";
APT::Periodic::Unattended-Upgrade "1";
APT::Periodic::AutocleanInterval "7";
EOF

systemctl enable unattended-upgrades
systemctl restart unattended-upgrades

# ---- Least-privileged runner user (NO SUDO) ----
if ! id -u github-runner >/dev/null 2>&1; then
  adduser --disabled-password --gecos "" github-runner
fi

# Ensure runner has NO sudo access
rm -f /etc/sudoers.d/github-runner

# Strict user limits to prevent fork bombs and resource exhaustion
cat >/etc/security/limits.d/github-runner.conf <<'EOF'
github-runner soft nproc 256
github-runner hard nproc 512
github-runner soft nofile 4096
github-runner hard nofile 8192
github-runner soft fsize 2097152
github-runner hard fsize 2097152
github-runner soft cpu 60
github-runner hard cpu 120
EOF

# Create runner directory with restricted permissions
install -d -o github-runner -g github-runner -m 0750 /opt/actions-runner

# ---- AppArmor profile for runner ----
if [ ! -f /etc/apparmor.d/opt.actions-runner.Runner.Listener ]; then
cat >/etc/apparmor.d/opt.actions-runner.Runner.Listener <<'EOF'
#include <tunables/global>

/opt/actions-runner/bin/Runner.Listener {
  #include <abstractions/base>
  #include <abstractions/nameservice>
  
  # Allow runner to execute
  /opt/actions-runner/** r,
  /opt/actions-runner/bin/* ix,
  
  # Allow work directory
  /opt/actions-runner/_work/** rw,
  
  # Network access for GitHub API
  network inet stream,
  network inet6 stream,
  
  # SocketCAN access
  network can raw,
  /sys/class/net/vcan0/** r,
  /sys/class/net/can0/** r,
  /dev/ttyUSB* rw,
  
  # Required system access
  /proc/sys/kernel/random/uuid r,
  /sys/devices/virtual/net/vcan0/** r,
  
  # Deny dangerous paths
  deny /etc/shadow r,
  deny /etc/sudoers* r,
  deny /root/** rw,
  deny /home/*/.ssh/** rw,
  
  # Deny writing to system locations
  deny /boot/** w,
  deny /etc/** w,
  deny /sys/** w,
}
EOF
fi

# Load AppArmor profile
apparmor_parser -r /etc/apparmor.d/opt.actions-runner.Runner.Listener || true

# ---- Restrictive firewall ----
ufw --force reset >/dev/null 2>&1 || true
ufw default deny incoming
ufw default allow outgoing
ufw limit OpenSSH
echo "y" | ufw enable >/dev/null 2>&1 || true

# ---- SSH hardening with fail2ban ----
if [ ! -f /etc/fail2ban/jail.local ]; then
cat >/etc/fail2ban/jail.local <<'EOF'
[DEFAULT]
bantime = 3600
findtime = 600
maxretry = 3

[sshd]
enabled = true
port = ssh
logpath = %(sshd_log)s
backend = %(sshd_backend)s
EOF
fi

systemctl enable fail2ban
systemctl restart fail2ban

# ---- Optional: SocketCAN vcan0 ----
if [ "$CAN_TYPE" = "virtual" ]; then
  modprobe vcan || true
  cat >/etc/systemd/system/vcan0.service <<'EOF'
[Unit]
Description=Bring up vcan0 (virtual CAN)
After=network.target
[Service]
Type=oneshot
ExecStart=/sbin/ip link add dev vcan0 type vcan || true
ExecStart=/sbin/ip link set up vcan0
RemainAfterExit=yes
[Install]
WantedBy=multi-user.target
EOF
  systemctl daemon-reload
  systemctl enable --now vcan0.service || true
fi

# ---- Audit logging ----
systemctl enable auditd
systemctl restart auditd

# Add rules to monitor runner activity (idempotent check)
if ! grep -q "runner_changes" /etc/audit/rules.d/runner.rules 2>/dev/null; then
cat >>/etc/audit/rules.d/runner.rules <<'EOF'
# Monitor runner directory changes
-w /opt/actions-runner/ -p wa -k runner_changes

# Monitor dangerous syscalls
-a always,exit -F arch=b64 -S execve -F euid=github-runner -k runner_exec
EOF
fi

augenrules --load || true

# ---- Download runner ----
if [ ! -f /opt/actions-runner/bin/Runner.Listener ]; then
su - github-runner -c "cd /opt/actions-runner && \
  curl -fsSL -o actions-runner-linux-${RUNNER_ARCH}-${RUNNER_VER}.tar.gz \
  https://github.com/actions/runner/releases/download/v${RUNNER_VER}/actions-runner-linux-${RUNNER_ARCH}-${RUNNER_VER}.tar.gz && \
  tar xzf actions-runner-linux-${RUNNER_ARCH}-${RUNNER_VER}.tar.gz"
fi

# ---- Register runner with labels ----
if [ ! -f /opt/actions-runner/.runner ]; then
su - github-runner -c "cd /opt/actions-runner && \
  ./config.sh --url '${GH_REPO_URL}' \
              --token '${GH_RUNNER_TOKEN}' \
              --name '${GH_RUNNER_NAME}' \
              --labels '${GH_RUNNER_LABELS}' \
              --work _work \
              --unattended"
fi

# ---- Install as a service with resource limits ----
if ! systemctl is-active --quiet actions.runner.*; then
  /opt/actions-runner/svc.sh install github-runner || true
fi

# Apply strict systemd limits (idempotent)
mkdir -p /etc/systemd/system/actions.runner.*.service.d/
cat >/etc/systemd/system/actions.runner.*.service.d/limits.conf <<'EOF'
[Service]
# Security hardening
NoNewPrivileges=yes
PrivateTmp=yes
ProtectSystem=strict
ProtectHome=yes
ReadWritePaths=/opt/actions-runner/_work

# Resource limits (prevent fork bombs, disk flooding, CPU exhaustion)
LimitCORE=0
LimitNOFILE=4096
LimitNPROC=512
LimitFSIZE=2147483648
CPUQuota=150%
MemoryMax=2G
TasksMax=256

# Prevent runaway processes
TimeoutStopSec=30

# Network isolation (allow only outbound)
RestrictAddressFamilies=AF_INET AF_INET6 AF_UNIX AF_CAN

# Prevent privilege escalation
CapabilityBoundingSet=
AmbientCapabilities=

# Additional protections against malicious code
ProtectKernelModules=yes
ProtectKernelTunables=yes
ProtectControlGroups=yes
ReadOnlyPaths=/opt/actions-runner/bin
SystemCallFilter=@system-service
SystemCallFilter=~@privileged @resources

# Prevent access to sensitive files
InaccessiblePaths=/root /home /etc/shadow /etc/sudoers /boot

# Disk quota for work directory (prevent disk flooding)
# Note: Requires quotas enabled on filesystem
EOF

systemctl daemon-reload

# Start or restart service
if systemctl is-active --quiet actions.runner.*; then
  systemctl restart actions.runner.* || true
else
  /opt/actions-runner/svc.sh start || true
fi

# ---- Cleanup job for old work directories ----
if ! sudo -u github-runner crontab -l 2>/dev/null | grep -q "_work"; then
  (sudo -u github-runner crontab -l 2>/dev/null; echo '15 3 * * 0 find /opt/actions-runner/_work -mindepth 1 -maxdepth 1 -type d -mtime +7 -exec rm -rf {} \;') | sudo -u github-runner crontab -
fi

echo "Runner '${GH_RUNNER_NAME}' provisioned with security hardening."
echo "Verify in GitHub → Settings → Actions → Runners."