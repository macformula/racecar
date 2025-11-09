#!/bin/bash
git ls-files -z 'projects/**/platformio.ini' | while IFS= read -r -d '' ini; do
    dir=$(dirname "$ini")
    echo "============================================================"
    echo "==== Performing Static Code Analysis on $dir"
    echo "============================================================"

    SUBDIRS=("$dir/src" "$dir/include")
    EXCLUDE_PREFIX="stm*"

    # Check for changes in $dir/src and $dir/include, excluding files within $dir/src/platforms/stm*
    if git diff --name-only --staged -- "${SUBDIRS[@]}" ":!$dir/src/platforms/$EXCLUDE_PREFIX" | grep -q .; then
        echo "Changes detected in $dir → running pio check"
        pio check --project-dir "$dir" --fail-on-defect=high || exit 1
    else
        echo "No changes in $dir → skipping"
    fi
done