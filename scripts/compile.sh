#!/usr/bin/env bash
set -euo pipefail

FQBN="esp32:esp32:esp32s3:CDCOnBoot=cdc"

echo "Compiling with FQBN: ${FQBN}"
arduino-cli compile --fqbn "${FQBN}" --clean .
