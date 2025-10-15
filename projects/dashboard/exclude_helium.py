"""
Exclude ARM Helium/NEON assembly files from build using SCons.

The STM32F469 (Cortex-M4) doesn't support Helium/NEON SIMD.
This script filters out those source files at the SCons level.
"""

Import("env")
import os

# Get the library source files
lib_dir = os.path.join(env.subst("$PROJECT_LIBDEPS_DIR"), env.subst("$PIOENV"), "lvgl")

# Filter function to exclude Helium and NEON files
def skip_helium_neon(node):
    filepath = node.get_path()
    # Exclude any file in helium/ or neon/ directories
    if '/helium/' in filepath or '/neon/' in filepath:
        if filepath.endswith(('.S', '.s', '.c')):
            print(f"[exclude] Excluding: {os.path.basename(filepath)}")
            return None
    return node

# Apply filter to source files
env.AddBuildMiddleware(skip_helium_neon, "*")

print("[exclude_helium] Build filter installed")
