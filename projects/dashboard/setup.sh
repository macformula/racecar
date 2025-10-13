#!/bin/bash
# One-time setup for Formula Electric Dashboard

set -e

echo "🏎️  Formula Electric Dashboard Setup"
echo "======================================"
echo ""

# Check for PlatformIO
if ! command -v pio &> /dev/null; then
    echo "❌ Error: PlatformIO is not installed!"
    echo "Install it from: https://platformio.org/install/cli"
    exit 1
fi
echo "✅ PlatformIO found"

# Check for SDL2
if ! pkg-config --exists sdl2 2>/dev/null; then
    echo "⚠️  SDL2 not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y libsdl2-dev
    echo "✅ SDL2 installed"
else
    echo "✅ SDL2 found"
fi

# Set up Python virtual environment
if [ ! -d "venv" ]; then
    echo "📦 Creating Python virtual environment..."
    python3 -m venv venv
    source venv/bin/activate
    pip install --upgrade pip
    pip install python-can cantools
    echo "✅ Python environment ready"
else
    echo "✅ Python environment already exists"
fi

# Set up SocketCAN
echo ""
echo "🔌 Setting up SocketCAN interface (requires sudo)..."
sudo src/platforms/linux/socketcan_setup.sh
echo "✅ SocketCAN interface ready"

echo ""
echo "======================================"
echo "✅ Setup complete!"
echo ""
echo "To run the dashboard, use these commands:"
echo ""
echo "  Terminal 1:  ./run_simulator.sh"
echo "  Terminal 2:  ./run_dashboard.sh"
echo ""
echo "Or simply run: ./run.sh (runs both)"
echo "======================================"

