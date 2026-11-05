# Ashen Hollow (Game Boy Color)
Created and developed by ash&gold. GBDK-2020 C source, MBC1 + RAM + battery.

## Build
1. Download GBDK-2020 (4.2.0 or newer) from https://github.com/gbdk-2020/gbdk-2020/releases and unzip it so `bin/lcc` exists.
2. `GBDK_HOME=/path/to/gbdk ./build.sh`  (or `make GBDK_HOME=...`)
3. Output: `build/ashen_hollow.gbc`

No PC? Push this folder to a GitHub repo; the included Actions workflow builds the ROM
and attaches it as a downloadable artifact (Actions tab > latest run > ashen_hollow_gbc).

## Play in Delta
Import `ashen_hollow.gbc` in Delta. Saves go to the battery file (.sav) automatically.

## Controls
D-pad move/menus, A confirm, B back, START pause menu (Cards / Save / Stats).
Inn (red roof): 10G full heal + auto-save. Saves also happen after every battle win/capture.
Capture (Skill menu) costs 100G; collect all 6 cards for the ending and credits.
