import subprocess
from pathlib import Path

# Repository root is two levels up from this script
repo_root = Path.cwd().resolve().parents[1]

def get_git_hash():
    try:
        # Executed command line command to retrieve a 7 character git hash
        git_hash = subprocess.check_output(
            ["git", "rev-parse", "--short=7", "HEAD"],
            cwd=repo_root
        ).decode().strip()

        # Create the is_dirty variable to check if the repository has uncommitted changes
        dirty_output = subprocess.check_output(
            ["git", "status", "--porcelain"],
            cwd=repo_root
        ).decode().strip()
        is_dirty = bool(dirty_output)

    except subprocess.CalledProcessError:
        git_hash = "0000000"
        is_dirty = False

    git_hash_int = int(git_hash, 16) & 0x0FFFFFFF
    return git_hash_int, is_dirty

def generate_git_hash_file():
    # Create the output directory path if it doesn't exist
    include_dir = repo_root / "include" / "generated"
    include_dir.mkdir(parents=True, exist_ok=True)
    header_path = include_dir / "githash.hpp"

    git_hash, is_dirty = get_git_hash()

    header_content = f"""#pragma once

#include <cstdint>

namespace macfe::generated {{

const uint32_t GIT_HASH = 0x{git_hash:08x};
const bool GIT_DIRTY = {'true' if is_dirty else 'false'};

}} // namespace macfe::generated
"""

    header_path.write_text(header_content)

generate_git_hash_file()