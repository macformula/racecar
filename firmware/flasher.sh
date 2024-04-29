#!/bin/bash

VERBOSE=false

# Function to display usage instructions
usage() {
    echo "Usage: $0 [-v]"
    echo "Options:"
    echo "  -v, --verbose  Print verbose output (show executed commands)"
    exit 1
}

# Function to build the project
build_project() {
    echo "Building the project..."
    if [ "$VERBOSE" = true ]; then
        set -x
    fi
    make PROJECT=DemoCanBar PLATFORM=stm32f767 &&
    rsync -az build/DemoCanBar/stm32f767/DemoCanBar.bin macfe:~/local/bin
    if [ "$VERBOSE" = true ]; then
        set +x
    fi
}

# Function to flash the binary on the remote machine
flash_binary() {
    echo "Flashing binary on the remote machine..."
    if [ "$VERBOSE" = true ]; then
        set -x
    fi
    ssh macfe << EOF
    cd ~/local/bin &&
    ./flasher --ecu=FrontController --binary=./DemoCanBar.bin
EOF
    if [ "$VERBOSE" = true ]; then
        set +x
    fi
}

# Main function to execute the build, sync, and flash steps
main() {
    while [[ $# -gt 0 ]]; do
        key="$1"
        case $key in
            -v|--verbose)
            VERBOSE=true
            shift
            ;;
            *)
            usage
            ;;
        esac
    done

    build_project || { echo "Build failed! Exiting..."; exit 1; }
    flash_binary || { echo "Flashing failed! Exiting..."; exit 1; }

    echo "Flash process completed successfully!"
}

# Execute the main function
main "$@"
