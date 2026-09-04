#!/usr/bin/env bash
set -euo pipefail

FQBN="esp32:esp32:esp32s3:CDCOnBoot=cdc"

PORT=$(ls /dev/tty.usbmodem* 2>/dev/null | head -n 1 || true)

if [ -z "${PORT}" ]; then
  echo "No /dev/tty.usbmodem* device found. Is the board plugged in?"
  echo "Tip: unplug and replug the board, then try again."
  exit 1
fi

echo "Using port: ${PORT}"
arduino-cli upload -p "${PORT}" --fqbn "${FQBN}" .
