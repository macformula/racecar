#!/bin/bash
# Simple script to run the Formula Electric Dashboard with simulator

set -e  # Exit on error

echo "🏎️  Formula Electric Dashboard Launcher"
echo "======================================"
echo ""

# Check if vcan0 exists
if ! ip link show vcan0 &> /dev/null; then
    echo "⚠️  vcan0 not found. Setting up SocketCAN..."
    echo "This requires sudo privileges."
    sudo src/platforms/linux/socketcan_setup.sh
    echo "✅ SocketCAN interface ready"
    echo ""
fi

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    echo "⚠️  Python virtual environment not found. Creating..."
    python3 -m venv venv
    source venv/bin/activate
    pip install python-can cantools
    echo "✅ Virtual environment created"
    echo ""
fi

# Build the dashboard
echo "🔨 Building dashboard..."
pio run -e linux
echo "✅ Build complete"
echo ""

# Kill any existing processes
pkill -f "fc_sim.py" 2>/dev/null || true
pkill -f ".pio/build/linux/program" 2>/dev/null || true
sleep 1

# Start the FrontController simulator in background
echo "🚀 Starting FrontController Simulator..."
source venv/bin/activate
python3 src/platforms/linux/fc_sim.py &
FC_PID=$!
echo "   PID: $FC_PID"

# Give simulator time to start
sleep 1

# Start the dashboard
echo "🚀 Starting Dashboard..."
echo ""
echo "Controls:"
echo "  Tab   - Scroll through options"
echo "  Space - Select/Confirm"
echo "  X     - Close window"
echo ""
echo "Press Ctrl+C to stop everything"
echo "======================================"
echo ""

# Run dashboard (this will block until closed)
SDL_VIDEODRIVER=x11 ./.pio/build/linux/program

# Cleanup when dashboard exits
echo ""
echo "🛑 Stopping FrontController Simulator..."
kill $FC_PID 2>/dev/null || true
echo "✅ Done!"

