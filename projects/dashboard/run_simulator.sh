#!/bin/bash
# Start the FrontController Simulator

echo "🎮 FrontController Simulator"
echo "======================================"

# Check if venv exists
if [ ! -d "venv" ]; then
    echo "❌ Error: Python virtual environment not found!"
    echo "Please run ./setup.sh first"
    exit 1
fi

# Activate venv and run
source venv/bin/activate
echo "Starting simulator..."
python3 src/platforms/linux/fc_sim.py

