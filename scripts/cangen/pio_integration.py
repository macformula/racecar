#!/usr/bin/env python3
"""
PlatformIO integration script for cangen.
This script runs as a prebuild step when cangen is used as a library dependency.
"""

Import("env")

import sys
from pathlib import Path
import subprocess

python_exe = env.subst("$PYTHONEXE")

try:
    import cangen.can_generator as cangen
except ImportError:
    print("[cangen] Installing dependencies.")
    subprocess.run([python_exe, "-m", "pip", "install", "-e", "."], check=True)

print("[cangen] Running code generation...")

# Get the current project directory (where platformio.ini is located)
project_dir = Path(env.subst("$PROJECT_DIR"))

# Check if config.yaml exists (required for cangen)
config_file = project_dir / "config.yaml"
if not config_file.exists():
    print(f"[cangen] Warning: No config.yaml found in {project_dir}")
    print("[cangen] Skipping code generation")
    sys.exit(2)

cangen.generate_can_for_project(str(project_dir))
