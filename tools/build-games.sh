#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DIST="${ROOT}/dist/apps"
BUILD_ROOT="${ROOT}/build/games"
CORE_LIB="${ROOT}/libraries/CardputerGameCore"
CARDPUTER_FQBN="m5stack:esp32:m5stack_cardputer:FlashSize=8M,PartitionScheme=default_8MB,CDCOnBoot=cdc,USBMode=hwcdc"

rm -rf "${DIST}"
mkdir -p "${DIST}" "${BUILD_ROOT}"

copy_game_bin() {
  local build_dir="$1"
  local dest="$2"
  local found=""
  found="$(find "${build_dir}" -maxdepth 1 -type f -name "*.ino.bin" | head -n 1 || true)"
  if [[ -z "${found}" ]]; then
    found="$(find "${build_dir}" -maxdepth 1 -type f -name "*.bin" ! -name "*.merged.bin" ! -name "*.bootloader.bin" ! -name "*.partitions.bin" | head -n 1 || true)"
  fi
  if [[ -z "${found}" ]]; then
    echo "[games] no sketch .bin found in ${build_dir}"
    return 1
  fi
  cp -f "${found}" "${DIST}/${dest}"
  echo "[games] packaged ${dest}"
}

GAMES_DATA=$(cat <<'GAMES'
star-trader-pocket-frontier|star_trader_pocket_frontier|Star Trader|Deep ship-console trading sim with markets, routes, cargo, crew, upgrades, and route events.
dungeon-courier|dungeon_courier|Dungeon Courier|Deep contract delivery RPG with town boards, package integrity, supplies, route choices, and perks.
tiny-wasteland|tiny_wasteland|Tiny Wasteland|Deep survival road drama with regions, camp care, scavenging risk, party state, trade, and vehicle pressure.
pocket-kingdom-manager|pocket_kingdom_manager|Pocket Kingdom|Deep seasonal kingdom ledger with workers, buildings, laws, advisors, crises, and yearly reports.
monster-ranch-trail|monster_ranch_trail|Monster Ranch|Deep weekly ranch sim with creature care, training, expeditions, facilities, tournaments, and ranch events.
guildmaster-pocket|guildmaster_pocket|Guildmaster|Deep quest desk sim with generated contracts, class parties, staged results, hall upgrades, and rivals.
haunted-radio-operator|haunted_radio_operator|Haunted Radio|Deep fictional station mystery with tuning, fake signals, decode tools, cases, logs, and power routing.
cyberdeck-hacker-rpg|cyberdeck_hacker_rpg|Cyberdeck RPG|Deep offline fake node-crawl with job board, simulated nodes, tools, recovered files, market, and reps.
pocket-detective-agency|pocket_detective_agency|Pocket Detective|Deep notebook mystery game with cases, maps, interviews, clue book, clue comparison, and accusations.
cryptid-park-ranger|cryptid_park_ranger|Cryptid Ranger|Deep ethical field research game with zones, gear, patrols, evidence tracking, guide entries, and reports.
cyber-ranger|cyber_ranger|Cyber Ranger|Deep fictional cyber-wilderness investigation with park zones, fake anomaly boards, deck commands, guide, cases, and gear.
star-trail-rancher|star_trail_rancher|Star Trail Ranch|Deep space creature transport sim with contracts, habitats, route decay, market supplies, and discovery logs.
wasteland-guildmaster|wasteland_guildmaster|Waste Guild|Deep settlement command sim with base pressure, expeditions, team roles, staged results, facilities, and threats.
signal-rat-cyberdeck-rpg|signal_rat_cyberdeck_rpg|Signal Rat|Deep flagship fictional cyberdeck RPG with safehouse, faction jobs, simulated nodes, recovered files, and deck upgrades.
poke-trail|PokeTrail|Poke-Trail|Original creature trail RPG with travel, battles, catching, audio, and region progression.|/Users/cypher/Documents/GitHub/poke-trail/PokeTrail|/poke-trail/save.sav
GAMES
)

while IFS='|' read -r slug sketch title notes source_path save_path; do
  [[ -z "${slug}" ]] && continue
  src="${source_path:-${ROOT}/games/${sketch}}"
  out="${BUILD_ROOT}/${sketch}"
  rm -rf "${out}"
  mkdir -p "${out}"
  echo "[games] building ${slug}"
  arduino-cli compile --fqbn "${CARDPUTER_FQBN}" --library "${CORE_LIB}" --output-dir "${out}" "${src}"
  copy_game_bin "${out}" "${slug}.bin"
done <<< "${GAMES_DATA}"

{
  echo '{'
  echo '  "games": ['
  first=1
  while IFS='|' read -r slug sketch title notes source_path save_path; do
    [[ -z "${slug}" ]] && continue
    source_path="${source_path:-${ROOT}/games/${sketch}}"
    save_path="${save_path:-/cardputer-game-os/saves/${slug}/save.sav}"
    if [[ ${first} -eq 0 ]]; then echo ','; fi
    first=0
    cat <<JSON
    {
      "name": "${title}",
      "slug": "${slug}",
      "binary": "${slug}.bin",
      "source_path": "${source_path}",
      "version": "$([[ "${slug}" == "poke-trail" ]] && echo "local-v1" || echo "deep-v2")",
      "status": "ready",
      "notes": "${notes}",
      "save_path": "${save_path}"
    }
JSON
  done <<< "${GAMES_DATA}"
  echo
  echo '  ]'
  echo '}'
} > "${DIST}/games.json"

echo "[games] manifest: ${DIST}/games.json"
