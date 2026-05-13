#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_DIST="${ROOT}/dist/apps"
SD_ROOT="${ROOT}/dist/sd-card"
SD_APPS="${SD_ROOT}/cardputer-game-os/apps"
SD_SAVES="${SD_ROOT}/cardputer-game-os/saves"
MANIFEST="${APP_DIST}/games.json"

rm -rf "${SD_ROOT}"
mkdir -p "${SD_APPS}" "${SD_SAVES}"

if [[ -f "${MANIFEST}" ]]; then
  cp -f "${MANIFEST}" "${SD_APPS}/games.json"
else
  echo "[sd] no built manifest found; using config/games.json"
  cp -f "${ROOT}/config/games.json" "${SD_APPS}/games.json"
fi

if compgen -G "${APP_DIST}/*.bin" >/dev/null; then
  cp -f "${APP_DIST}"/*.bin "${SD_APPS}/"
fi

echo "[sd] prepared ${SD_ROOT}"
echo "[sd] copy the contents of ${SD_ROOT} to the root of a FAT32 SD card"
