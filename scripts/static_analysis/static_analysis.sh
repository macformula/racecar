#!/bin/bash
git ls-files -z 'projects/**/platformio.ini' | while IFS= read -r -d '' ini; do
    dir=$(dirname "$ini")
    echo "============================================================"
    echo "==== Performing Static Code Analysis on $dir"
    echo "============================================================"

    if ! git diff --quiet --staged --exit-code "$dir"; then
        echo "Changes detected in $dir → running pio check"
        pio check --project-dir "$dir" --fail-on-defect=high || exit 1
    else
        echo "No changes in $dir → skipping"
    fi
done