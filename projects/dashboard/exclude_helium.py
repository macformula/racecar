"""
Disable ARM SIMD optimizations for x86 Linux builds.

LVGL 9.3 includes ARM-specific Helium and NEON SIMD assembly files that
cannot be compiled on x86 architectures. This pre-build script renames
these files (.S, .s, .c) to .disabled in the LVGL draw/sw/blend directory
to prevent compilation errors while building on Linux.

This runs automatically before each build via platformio.ini extra_scripts.
"""


Import("env")
import os
import shutil

# Rename ARM SIMD files (Helium and NEON) so they won't be compiled on x86 Linux
project_dir = env.subst("$PROJECT_DIR")
blend_dir = os.path.join(project_dir, ".pio/libdeps/linux/lvgl/src/draw/sw/blend")

# Disable ARM-specific SIMD directories
for simd_type in ["helium", "neon"]:
    simd_dir = os.path.join(blend_dir, simd_type)
    if os.path.exists(simd_dir):
        for root, dirs, files in os.walk(simd_dir):
            for file in files:
                if file.endswith(('.S', '.s', '.c')):
                    src_file = os.path.join(root, file)
                    dst_file = src_file + ".disabled"
                    if not os.path.exists(dst_file):
                        shutil.move(src_file, dst_file)
                        print(f"Disabled ARM {simd_type.upper()} file: {file}")

