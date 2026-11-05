#!/bin/sh
# Usage: GBDK_HOME=/path/to/gbdk ./build.sh
set -e
GBDK_HOME=${GBDK_HOME:-$HOME/gbdk}
mkdir -p build
"$GBDK_HOME/bin/lcc" -Wm-yc -Wm-yt0x03 -Wm-yo2 -Wm-ya1 -Wm-yn"ASHENHOLLOW" -o build/ashen_hollow.gbc src/main.c
echo "Built build/ashen_hollow.gbc"
