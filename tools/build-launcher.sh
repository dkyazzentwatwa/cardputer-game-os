#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="${ROOT}/dist/launcher"

mkdir -p "${OUT}"

echo "[launcher] compiling Cypher Game OS"
arduino-cli compile --profile adv --output-dir "${OUT}" "${ROOT}"

echo "[launcher] output: ${OUT}"
