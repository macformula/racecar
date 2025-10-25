#!/bin/bash
# Build and run the Dashboard

echo "Formula Electric Dashboard"
echo "======================================"
echo ""

# Always rebuild to pick up any code changes
echo "Building dashboard..."
pio run -e linux

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build complete"
echo ""
echo "Starting Dashboard..."
echo ""
echo "Controls:"
echo "  Tab   - Scroll through options"
echo "  Space - Select/Confirm"
echo "  X or Ctrl+C     - Close window"
echo ""
echo "======================================"
echo ""

SDL_VIDEODRIVER=x11 ./.pio/build/linux/program
