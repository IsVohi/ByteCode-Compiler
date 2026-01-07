#!/bin/bash
# run.sh - Launch ByteCode Compiler in a new terminal window

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
COMPILER="$SCRIPT_DIR/build/compiler"

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo "Error: Compiler not found. Please build first:"
    echo "  mkdir build && cd build && cmake .. && make"
    exit 1
fi

# If a file argument is provided, run in batch mode
if [ -n "$1" ]; then
    osascript -e "tell application \"Terminal\" to do script \"cd '$SCRIPT_DIR' && ./build/compiler '$1'; echo ''; echo 'Press Enter to close...'; read\""
else
    # No file - open REPL in new terminal
    osascript -e "tell application \"Terminal\" to do script \"cd '$SCRIPT_DIR' && ./build/compiler\""
fi

echo "Opened compiler in new Terminal window"
