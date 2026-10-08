# Pocket Blocks

An original falling-block puzzle cartridge for Game Boy, Game Boy Color, and ModRetro Chromatic. Stack seven four-cell shapes in a 10 × 16 well, complete rows, and keep the skyline clear. All graphics and code are original; no commercial ROMs or assets are included.

- Left / Right: move (hold for repeat).
- Down: soft drop.
- A / B: clockwise / counterclockwise rotation, with horizontal wall kicks.
- Up: hard drop.
- Start: begin, pause/resume, or restart after game over.

The outlined landing guide shows where the piece will fall. The next shape appears beside the well. A shuffled seven-shape bag keeps the supply balanced. Clears award 100 / 300 / 500 / 800 points × level; soft and hard drops earn 1 and 2 points per row. Every ten cleared rows increases the level and falling speed. Runs and scores reset on restart; there is no battery save.

## Build

Install GBDK 4.5.0 or newer, then run:

```sh
make GBDK_HOME=/path/to/gbdk
```

The playable cartridge is `dist/pocket-blocks.gbc`. `tools/make_art.py` regenerates the original tile and font data with the Python standard library. This is an editable native GBDK C project; it does not require GB Studio.

## Verification

Actual cartridge tested with PyBoy using:

```sh
PYTHONPATH=/path/to/pyboy/site-packages python3 tools/test_game.py
```

The test boots the built ROM and checks Start, movement, rotation, pause, hard drop and restart through button input. It also prepares explicit emulator RAM board fixtures to exercise a four-line clear, level progression, and blocked-spawn game over. Results and actual emulator screenshots live in `dist/test-results.json`, `title.png`, `play.png`, `line-clear.png`, and `game-over.png`. Both CGB and original monochrome Game Boy (DMG) modes pass the same suite. Add `POCKET_BLOCKS_DMG=1` to run the DMG suite; its screenshots and results use a `-dmg` suffix. Physical Chromatic play and flashing have not been tested.
