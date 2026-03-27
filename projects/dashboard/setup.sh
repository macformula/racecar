#!/bin/bash
# One-time setup for Formula Electric Dashboard

set -e

echo "Formula Electric Dashboard Setup"
echo "======================================"
echo ""

# Initialize git submodules (for lv_drivers)
echo "Initializing git submodules..."
git submodule update --init --recursive
echo "Submodules initialized"

# Check for Python3
if ! command -v python3 &> /dev/null; then
    echo "Error: Python3 is not installed!"
    echo "Install it with: sudo apt-get install python3 python3-pip python3-venv"
    exit 1
fi
echo "Python3 found"

# Check for pip
if ! command -v pip3 &> /dev/null; then
    echo "Error: pip3 is not installed!"
    echo "Install it with: sudo apt-get install python3-pip"
    exit 1
fi
echo "pip3 found"

# Check for build tools
if ! dpkg -l | grep -q build-essential; then
    echo " Build tools not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y build-essential
    echo "Build tools installed"
else
    echo "Build tools found"
fi

# Check for PlatformIO
if ! command -v pio &> /dev/null; then
    echo "Error: PlatformIO is not installed!"
    echo "Install it from: https://platformio.org/install/cli"
    exit 1
fi
echo "PlatformIO found"

# Check for X11 development headers
if ! pkg-config --exists x11 2>/dev/null; then
    echo "X11 development headers not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y libx11-dev
    echo "X11 development headers installed"
else
    echo "X11 development headers found"
fi

# Check for CAN utilities
if ! command -v cansend &> /dev/null; then
    echo "CAN utilities not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y can-utils
    echo "CAN utilities installed"
else
    echo "CAN utilities found"
fi

# Check for SDL2
if ! pkg-config --exists sdl2 2>/dev/null; then
    echo "SDL2 not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y libsdl2-dev
    echo "SDL2 installed"
else
    echo "SDL2 found"
fi

# Set up Python virtual environment
if [ ! -d "venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv venv
    source venv/bin/activate
    pip install --upgrade pip
    pip install -r src/platforms/linux/requirements.txt
    echo "Python environment ready"
else
    echo "Python environment already exists"
fi

# Set up SocketCAN
echo ""
echo "Setting up SocketCAN interface (requires sudo)..."
sudo src/platforms/linux/socketcan_setup.sh
echo "SocketCAN interface ready"

echo ""
echo "======================================"
echo "Setup complete!"
echo ""
echo "To run the dashboard, use these commands:"
echo ""
echo "  Terminal 1:  ./run_simulator.sh"
echo "  Terminal 2:  ./run_dashboard.sh"
echo ""
echo "Or simply run: ./run.sh (runs both)"
echo "======================================"

