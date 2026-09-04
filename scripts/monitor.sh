#!/usr/bin/env bash
set -euo pipefail

PORT=$(ls /dev/tty.usbmodem* 2>/dev/null | head -n 1 || true)

if [ -z "${PORT}" ]; then
  echo "No /dev/tty.usbmodem* device found. Is the board plugged in?"
  echo "Tip: unplug and replug the board, then try again."
  exit 1
fi

echo "Monitoring port: ${PORT}"
arduino-cli monitor -p "${PORT}" -c baudrate=115200
