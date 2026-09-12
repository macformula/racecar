Import("env")

import subprocess
from pathlib import Path

include_path = Path(env.subst("$PROJECT_INCLUDE_DIR")) / "generated"

def get_git_hash():
    # Executed command line command to retrieve a 7 character git hash
    git_hash = subprocess.check_output(
        ["git", "rev-parse", "--short=7", "HEAD"],
        cwd=include_path
    ).decode().strip()

    # Create the is_dirty variable to check if the repository has uncommitted changes
    dirty_output = subprocess.check_output(
        ["git", "status", "--porcelain"],
        cwd=include_path
    ).decode().strip()
    is_dirty = bool(dirty_output)

    git_hash_int = int(git_hash, 16) & 0x0FFFFFFF
    return git_hash_int, is_dirty

def generate_git_hash_file():
    # Create the output directory path if it doesn't exist
    include_path.mkdir(parents=True, exist_ok=True)
    header_path = include_path / "githash.hpp"

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