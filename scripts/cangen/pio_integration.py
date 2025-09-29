#!/usr/bin/env python3
"""
PlatformIO integration script for cangen.
This script runs as a prebuild step when cangen is used as a library dependency.
"""

import subprocess

import SCons.Action

Import("env")
python_exe = env.subst("$PYTHONEXE")

try:
    import cangen.__main__
except ImportError:
    subprocess.run([python_exe, "-m", "pip", "install", "-e", "."])


proj_dir = env.subst("$PROJECT_DIR")

cmd = [python_exe, "-m", "cangen", proj_dir]
action = SCons.Action.CommandAction(cmd)
result = env.Execute(action)

if result == 0:
    print("[cangen] Success!")
else:
    print(f"[cangen] ERROR: {result} processing command '{cmd}'")
    exit(1)
