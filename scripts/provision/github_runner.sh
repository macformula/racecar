#!/usr/bin/env bash
# ==============================================================================
# GitHub Actions Self-Hosted Runner Provisioning Script (Ubuntu)
# ==============================================================================
#
# PURPOSE:
#   Provisions a hardened, security-focused GitHub Actions self-hosted runner
#   for Software-in-the-Loop (SIL) testing with optional SocketCAN support.
#
# SECURITY FEATURES:
#   1. Least-Privilege User: Runner executes as 'github-runner' (NO sudo access)
#   2. Resource Limits: CPU, memory, process, and disk quotas prevent DoS attacks
#   3. Network Isolation: Firewall (ufw) restricts all inbound except SSH
#   4. Intrusion Detection: fail2ban protects SSH; auditd monitors runner activity
#   5. Mandatory Access Control: AppArmor profile restricts file/network access
#   6. Automatic Updates: unattended-upgrades applies security patches
#   7. Audit Logging: Tracks all runner executions and file modifications
#   8. Process Isolation: systemd sandboxing (PrivateTmp, ProtectSystem, etc.)
#
# DEFENDS AGAINST:
#   - Privilege escalation (no sudo, capability dropping, NoNewPrivileges)
#   - Fork bombs (nproc limits, TasksMax in systemd)
#   - Disk flooding (FSIZE limits, work directory cleanup cron)
#   - CPU exhaustion (CPUQuota, cpu time limits)
#   - Memory exhaustion (MemoryMax in systemd)
#   - Malicious code execution (AppArmor profile, filesystem restrictions)
#
# USAGE:
#   sudo GH_REPO_URL="https://github.com/OWNER/REPO" \
#        GH_RUNNER_TOKEN="YOUR_TOKEN_HERE" \
#        GH_RUNNER_NAME="sil-runner-01" \
#        GH_RUNNER_LABELS="self-hosted,linux,sil,socketcan" \
#        RUNNER_ARCH="x64" \
#        CAN_TYPE="virtual" \
#        bash provision_runner.sh
#
# PARAMETERS:
#   GH_REPO_URL       - GitHub repository URL (required)
#   GH_RUNNER_TOKEN   - Runner registration token from GitHub (required)
#   GH_RUNNER_NAME    - Runner display name (default: sil-runner-01)
#   GH_RUNNER_LABELS  - Comma-separated labels (default: self-hosted,linux,sil)
#   RUNNER_ARCH       - Architecture: x64, arm64, or arm (default: x64)
#   CAN_TYPE          - SocketCAN type: virtual, physical, or none (default: none)
#   RUNNER_VER        - GitHub runner version (default: 2.329.0)
#
# IDEMPOTENCY:
#   Safe to run multiple times. All operations check for existing state before
#   making changes. Re-running will update configurations without breaking the
#   existing runner installation.
#
# ==============================================================================

# Exit immediately on error, treat unset variables as errors, fail on pipe errors
set -euo pipefail

# ==============================================================================
# CONFIGURATION: Load and validate environment variables
# ==============================================================================

# Required parameters (script exits if not set)
: "${GH_REPO_URL:?Set GH_REPO_URL to https://github.com/OWNER/REPO}"
: "${GH_RUNNER_TOKEN:?Set GH_RUNNER_TOKEN from GitHub Runners page}"

# Optional parameters with sensible defaults
GH_RUNNER_NAME="${GH_RUNNER_NAME:-sil-runner-01}"
GH_RUNNER_LABELS="${GH_RUNNER_LABELS:-self-hosted,linux,sil}"
RUNNER_ARCH="${RUNNER_ARCH:-x64}"
CAN_TYPE="${CAN_TYPE:-none}"
RUNNER_VER="${RUNNER_VER:-2.329.0}"

# Validate architecture matches GitHub Actions runner releases
case "$RUNNER_ARCH" in
  x64|arm64|arm) ;;
  *)
    echo "Invalid RUNNER_ARCH: $RUNNER_ARCH (must be x64, arm64, or arm)"
    exit 1
    ;;
esac

# Validate CAN interface type
case "$CAN_TYPE" in
  virtual|physical|none) ;;
  *)
    echo "Invalid CAN_TYPE: $CAN_TYPE (must be virtual, physical, or none)"
    exit 1
    ;;
esac

echo "[INFO] Provisioning GitHub runner: $GH_RUNNER_NAME"
echo "[INFO] Repository: $GH_REPO_URL"
echo "[INFO] Architecture: $RUNNER_ARCH | CAN Type: $CAN_TYPE | Runner Version: $RUNNER_VER"

# ==============================================================================
# STEP 1: Install base packages and security tools
# ==============================================================================
# SECURITY: Install essential security packages to harden the system
#   - ufw: Uncomplicated Firewall for network isolation
#   - fail2ban: Intrusion prevention (blocks brute-force SSH attacks)
#   - apparmor/apparmor-utils: Mandatory Access Control (MAC) system
#   - auditd: Audit daemon for tracking system calls and file access
#   - unattended-upgrades: Automatic security patch installation

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y curl tar jq git ca-certificates ufw fail2ban unattended-upgrades \
  apparmor apparmor-utils auditd

# ==============================================================================
# STEP 2: Install SocketCAN tooling (if CAN support is requested)
# ==============================================================================
# Install CAN bus utilities for Software-in-the-Loop (SIL) testing
# Supports both virtual CAN (vcan) and physical CAN adapters

if [ "$CAN_TYPE" != "none" ]; then
  # Install CAN utilities: ip (for CAN interface mgmt), candump, cansend, etc.
  apt-get install -y iproute2 can-utils || true

  # Install Python CAN libraries for SIL test scripts
  apt-get install -y python3 python3-pip python3-venv || true
  pip3 install --break-system-packages python-can cantools || true

  # Load SocketCAN kernel modules (required for CAN communication)
  if ! lsmod | grep -q "^can"; then
    modprobe can || echo "Warning: CAN kernel module not available"
  fi
  if ! lsmod | grep -q "^can_raw"; then
    modprobe can_raw || echo "Warning: CAN_RAW kernel module not available"
  fi
  if ! lsmod | grep -q "^vcan"; then
    modprobe vcan || echo "Warning: VCAN kernel module not available"
  fi

  # Persist CAN modules across reboots
  cat >/etc/modules-load.d/can.conf <<'EOF'
can
can_raw
vcan
can_dev
EOF

  # For physical CAN: install serial CAN adapter tools (slcand)
  if [ "$CAN_TYPE" = "physical" ]; then
    apt-get install -y can-utils slcan-utils || true
  fi
fi

# ==============================================================================
# STEP 3: Configure automatic security updates
# ==============================================================================
# SECURITY: Automatically apply security patches to prevent exploitation of
# known vulnerabilities. System will not auto-reboot to avoid runner downtime.

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

# ==============================================================================
# STEP 4: Create least-privileged runner user
# ==============================================================================
# SECURITY: Runner executes as 'github-runner' user with NO sudo privileges
# This prevents malicious workflows from escalating privileges or modifying
# system files. Even if a workflow is compromised, damage is contained.

if ! id -u github-runner >/dev/null 2>&1; then
  adduser --disabled-password --gecos "" github-runner
fi

# SECURITY: Explicitly remove any sudo access (defense in depth)
rm -f /etc/sudoers.d/github-runner

# SECURITY: Apply resource limits to prevent DoS attacks
#   nproc: limits process count (prevents fork bombs)
#   nofile: limits open file descriptors (prevents fd exhaustion)
#   cpu: limits CPU time per process (prevents CPU exhaustion)
cat >/etc/security/limits.d/github-runner.conf <<'EOF'
github-runner soft nproc 256
github-runner hard nproc 512
github-runner soft nofile 4096
github-runner hard nofile 8192
github-runner soft cpu 60
github-runner hard cpu 120
EOF

# Create runner installation directory with restricted permissions
install -d -o github-runner -g github-runner -m 0755 /opt/actions-runner

# ==============================================================================
# STEP 5: Configure restrictive firewall
# ==============================================================================
# SECURITY: Network isolation - deny all inbound except SSH
# This prevents external attacks and limits the attack surface.
# Runner only needs outbound access to GitHub API and package repos.

ufw --force reset >/dev/null 2>&1 || true
ufw default deny incoming
ufw default allow outgoing
# Allow SSH with rate limiting (prevents brute-force)
ufw limit OpenSSH 2>/dev/null || ufw limit ssh 2>/dev/null || ufw limit 22/tcp
echo "y" | ufw enable >/dev/null 2>&1 || true

# ==============================================================================
# STEP 6: Configure SSH intrusion prevention (fail2ban)
# ==============================================================================
# SECURITY: Protects SSH from brute-force attacks by banning IPs after
# 3 failed login attempts within 10 minutes. Ban lasts 1 hour.

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

# ==============================================================================
# STEP 7: Configure SocketCAN interface (if requested)
# ==============================================================================
# Creates a persistent CAN interface for SIL testing
# - virtual: vcan0 (software-only, no hardware needed)
# - physical: can0 (requires hardware CAN adapter)

if [ "$CAN_TYPE" = "virtual" ]; then
  # Virtual CAN interface for testing without hardware
  modprobe vcan 2>/dev/null || true
  cat >/etc/systemd/system/vcan0.service <<'EOF'
[Unit]
Description=Bring up vcan0 (virtual CAN)
After=network.target
[Service]
Type=oneshot
ExecStart=/bin/bash -c '/sbin/ip link add dev vcan0 type vcan 2>/dev/null || true'
ExecStart=/sbin/ip link set up vcan0
RemainAfterExit=yes
[Install]
WantedBy=multi-user.target
EOF
  systemctl daemon-reload
  systemctl enable vcan0.service
  systemctl restart vcan0.service || true

elif [ "$CAN_TYPE" = "physical" ]; then
  # Physical CAN interface (requires hardware adapter like MCP2515 or USB-CAN)
  cat >/etc/systemd/system/can0.service <<'EOF'
[Unit]
Description=Bring up can0 (physical CAN interface)
After=network.target
[Service]
Type=oneshot
# For native SocketCAN devices (e.g., MCP2515)
ExecStart=/sbin/ip link set can0 type can bitrate 500000
ExecStart=/sbin/ip link set can0 up
ExecStop=/sbin/ip link set can0 down
RemainAfterExit=yes
[Install]
WantedBy=multi-user.target
EOF
  systemctl daemon-reload
  systemctl enable can0.service
  systemctl restart can0.service || true
  
  # NOTE: For serial CAN adapters (slcan), run slcand before enabling service:
  # slcand -o -c -s6 /dev/ttyUSB0 can0
fi

# ==============================================================================
# STEP 8: Configure audit logging
# ==============================================================================
# SECURITY: Track all runner activity for forensic analysis and intrusion detection
# Logs all file modifications in runner directory and all processes executed by runner

systemctl enable auditd
systemctl restart auditd

# Create audit rules to monitor runner activity (idempotent)
if ! grep -q "runner_changes" /etc/audit/rules.d/runner.rules 2>/dev/null; then
cat >>/etc/audit/rules.d/runner.rules <<'EOF'
# Log all file writes/attribute changes in runner directory
-w /opt/actions-runner/ -p wa -k runner_changes
# Log all program executions by runner user (track what workflows run)
-a always,exit -F arch=b64 -S execve -F euid=github-runner -k runner_exec
EOF
fi

augenrules --load || true

# ==============================================================================
# STEP 9: Download and extract GitHub Actions runner binaries
# ==============================================================================
# Downloads official runner release from GitHub. Runs as 'github-runner' user
# to ensure all files are owned by the unprivileged account.

if [ ! -f /opt/actions-runner/bin/Runner.Listener ]; then
  echo "[INFO] Downloading GitHub Actions runner version $RUNNER_VER"
  ARCHIVE="actions-runner-linux-${RUNNER_ARCH}-${RUNNER_VER}.tar.gz"
  su - github-runner -c "cd /opt/actions-runner && \
    curl -fsSL -o '$ARCHIVE' \
      'https://github.com/actions/runner/releases/download/v${RUNNER_VER}/'$ARCHIVE && \
    tar xzf '$ARCHIVE'"
  echo "[INFO] Runner binaries extracted successfully"
fi

# Pre-create directories that runner needs (prevents permission errors during first run)
mkdir -p /opt/actions-runner/_diag /opt/actions-runner/_work
chown -R github-runner:github-runner /opt/actions-runner
chmod -R u+w /opt/actions-runner

# ==============================================================================
# STEP 10: Register runner with GitHub repository
# ==============================================================================
# Registers the runner with GitHub using a registration token. The .runner file
# indicates successful registration. This step is idempotent.
# SECURITY: Token is single-use and short-lived (keep it secret)

if [ ! -f /opt/actions-runner/.runner ]; then
  echo "[INFO] Registering runner with GitHub"
  cd /opt/actions-runner
  su github-runner -c "./config.sh --url '${GH_REPO_URL}' \
                --token '${GH_RUNNER_TOKEN}' \
                --name '${GH_RUNNER_NAME}' \
                --labels '${GH_RUNNER_LABELS}' \
                --work _work \
                --unattended"
  echo "[INFO] Runner registration completed"
fi

# ==============================================================================
# STEP 11: Install runner as a systemd service with strict resource limits
# ==============================================================================
# SECURITY: Systemd sandboxing and resource limits defend against malicious code
# - NoNewPrivileges: prevents setuid/setgid privilege escalation
# - PrivateTmp: isolates /tmp from other processes
# - ProtectSystem: makes system directories read-only
# - ProtectHome: blocks access to user home directories
# - LimitNPROC/TasksMax: prevents fork bombs
# - LimitFSIZE: prevents disk flooding attacks
# - CPUQuota/MemoryMax: prevents resource exhaustion
# - CapabilityBoundingSet: drops all capabilities (no raw sockets, etc.)
if ! systemctl is-active --quiet actions.runner.*; then
  /opt/actions-runner/svc.sh install github-runner || true
fi

# Create systemd drop-in directory and apply security limits
mkdir -p /etc/systemd/system/actions.runner.*.service.d/
cat >/etc/systemd/system/actions.runner.*.service.d/limits.conf <<'EOF'
[Service]
# Process isolation and sandboxing
NoNewPrivileges=yes
PrivateTmp=yes
ProtectSystem=strict
ProtectHome=yes
ReadWritePaths=/opt/actions-runner/_work /opt/actions-runner/_diag

# Resource limits to prevent DoS attacks
LimitCORE=0
LimitNOFILE=4096
LimitNPROC=512
LimitFSIZE=2147483648
CPUQuota=150%
MemoryMax=2G
TasksMax=256

# Execution limits
TimeoutStopSec=30

# Network restrictions (outbound only, plus CAN for SIL)
RestrictAddressFamilies=AF_INET AF_INET6 AF_UNIX AF_CAN

# Remove all capabilities (prevents raw socket access, etc.)
CapabilityBoundingSet=
AmbientCapabilities=
EOF

systemctl daemon-reload

# Ensure service is running
SERVICE_NAME=$(systemctl list-units --full --all --no-legend 'actions.runner.*' | awk '{print $1}' | head -n1)
if [ -n "$SERVICE_NAME" ]; then
  if ! systemctl is-active --quiet "$SERVICE_NAME"; then
    echo "[*] Starting runner service: $SERVICE_NAME"
    systemctl start "$SERVICE_NAME" || true
  else
    echo "[*] Runner service already active: $SERVICE_NAME"
  fi
fi

# ==============================================================================
# STEP 12: Apply AppArmor Mandatory Access Control profile
# ==============================================================================
# SECURITY: AppArmor provides an additional layer of defense by restricting
# what the runner process can access, even if it's compromised.
# - Allows: runner directory, work directory, network access, CAN interfaces
# - Denies: sensitive system files, other users' data, system directories

if [ ! -f /etc/apparmor.d/opt.actions-runner.Runner.Listener ]; then
cat >/etc/apparmor.d/opt.actions-runner.Runner.Listener <<'EOF'
#include <tunables/global>
/opt/actions-runner/bin/Runner.Listener {
  #include <abstractions/base>
  #include <abstractions/nameservice>
  
  # Allow runner to execute its own binaries
  /opt/actions-runner/ r,
  /opt/actions-runner/** r,
  /opt/actions-runner/bin/* ix,
  
  # Allow work and diagnostic directories (where jobs execute)
  /opt/actions-runner/_work/** rw,
  /opt/actions-runner/_diag/** rw,
  /opt/actions-runner/.credentials rw,
  /opt/actions-runner/.runner rw,
  /opt/actions-runner/.env rw,
  /opt/actions-runner/.path rw,
  
  # Network access for GitHub API communication
  network inet stream,
  network inet6 stream,
  
  # SocketCAN access for SIL testing
  network can raw,
  /sys/class/net/vcan0/** r,
  /sys/class/net/can0/** r,
  /dev/ttyUSB* rw,
  
  # Required system access for runner operation
  /proc/sys/kernel/random/uuid r,
  /sys/devices/virtual/net/vcan0/** r,
  
  # Explicitly deny access to sensitive files
  deny /etc/shadow r,
  deny /etc/sudoers* r,
  deny /root/** rw,
  deny /home/*/.ssh/** rw,
  
  # Prevent writes to critical system directories
  deny /boot/** w,
  deny /etc/** w,
  deny /sys/** w,
}
EOF
fi

# Load the AppArmor profile into the kernel
apparmor_parser -r /etc/apparmor.d/opt.actions-runner.Runner.Listener || true

# ==============================================================================
# STEP 13: Configure automatic cleanup of old work directories
# ==============================================================================
# SECURITY: Prevents disk space exhaustion from accumulated build artifacts
# Weekly cron job (Sundays at 03:15) removes work directories older than 7 days

if ! crontab -l 2>/dev/null | grep -q "_work"; then
  (crontab -l 2>/dev/null; echo '15 3 * * 0 find /opt/actions-runner/_work -mindepth 1 -maxdepth 1 -type d -mtime +7 -exec rm -rf {} \;') | crontab -
fi

# ==============================================================================
# PROVISIONING COMPLETE
# ==============================================================================
echo "[SUCCESS] Runner '$GH_RUNNER_NAME' provisioned with security hardening"
echo "[INFO] CAN Type: $CAN_TYPE"
echo "[INFO] Verify runner status in GitHub: Settings -> Actions -> Runners"
