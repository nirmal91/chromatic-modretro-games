# Pocket Snake

An original garden-themed Snake cartridge for Game Boy, Game Boy Color, and ModRetro Chromatic. Eat fruit, grow your snake, and avoid the walls and your own body. Every five fruit increases the pace, from a relaxed start to a fast finish. Clear all 234 garden cells to win. The best score lasts until the cartridge is reset.

## Controls

- D-pad: steer (reverse turns are prevented).
- A or Start: begin a game or retry after a collision.
- Start during play: pause / resume.
- Select while paused: restart.

## Build

Install GBDK-2020, then run `make GBDK_HOME=/absolute/path/to/gbdk` in this directory. The playable cartridge is `dist/pocket-snake.gbc`; it also supports the original monochrome Game Boy. Tile artwork is authored in `tools/make_art.py` and regenerated automatically by Make.

All code and graphics are original homebrew. No commercial ROM, Nintendo artwork, or game assets are included.

## Runtime verification

With PyBoy installed, run `GBDK_HOME=/absolute/path/to/gbdk python3 tools/playtest.py`. Add `TEST_DMG=1` to run the full suite in monochrome Game Boy mode. The test builds the same source into a temporary ROM and uses real directional/button input to check start, reversal prevention, pause, restart, fruit collection, growth, increasing speed, collision, retry, and the session best score. It retains actual emulator screenshots in `dist/`. It does not alter emulated game state. Audio and physical-device behavior still need a hardware check.
