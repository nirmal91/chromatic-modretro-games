# Chromatic ModRetro Games

Original Game Boy Color games, built for ModRetro Chromatic and playable in a browser. Made by Nirmal Utwani with Codex.

**[Play the games](https://nirmal91.github.io/chromatic-modretro-games/)** · [Download ROMs](https://github.com/nirmal91/chromatic-modretro-games/releases)

[Download the four classics-inspired ROMs as a ZIP](https://nirmal91.github.io/chromatic-modretro-games/classic-roms.zip): Pocket Jumper, Pocket Blocks, Pocket Snake, and Water Rings. Unzip and choose a `.gbc` file for your compatible writable cartridge. Controls are included. These builds have been tested in emulators; physical Chromatic play is still unverified.

| Game | What it is | Play | ROM / source |
| --- | --- | --- | --- |
| **Pocket Jumper** | Leap across gaps, collect coins, bounce on critters, and reach the flag. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/pocket-jumper/) | [ROM](games/pocket-jumper/dist/pocket-jumper.gbc) · [source](games/pocket-jumper/) |
| **Pocket Blocks** | Stack seven shapes, clear rows, and keep up as the blocks fall faster. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/pocket-blocks/) | [ROM](games/pocket-blocks/dist/pocket-blocks.gbc) · [source](games/pocket-blocks/) |
| **Pocket Snake** | Grow through a pocket garden. Eat fruit, pick up speed, and avoid your own tail. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/pocket-snake/) | [ROM](games/pocket-snake/dist/pocket-snake.gbc) · [source](games/pocket-snake/) |
| **Water Rings** | Pump two water jets, float eight rings, then let them settle onto the pegs. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/water-rings/) | [ROM](games/water-rings/dist/water-rings.gbc) · [source](games/water-rings/) |
| **Meteor Mayhem** | A fast arcade shooter: dodge incoming rocks, blast drones, chain kills, dash through danger, and defeat the final boss. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/meteor-mayhem/) | [ROM](games/meteor-mayhem/dist/meteor-mayhem.gbc) · [source](games/meteor-mayhem/) |
| **Midnight Courier** | A short neon heist with four sectors, three cores, a cipher, and a timed getaway. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/midnight-courier/) | [ROM](games/midnight-courier/dist/midnight-courier.gbc) · [source](games/midnight-courier/) |
| **Cosmic Chess Academy (incomplete)** | An unfinished three-mission prototype for rook, bishop, and knight movement. It is not a complete chess game. | [Try prototype](https://nirmal91.github.io/chromatic-modretro-games/play/cosmic-chess-academy/) | [ROM](games/cosmic-chess-academy/dist/cosmic-chess-academy.gbc) · [source](games/cosmic-chess-academy/) |

All seven ROMs are original homebrew. The four new classics-inspired games are original code and artwork, rather than commercial Mario or Tetris ROMs. Each is a 32 KiB native GBDK cartridge with its own controls, build notes, and emulator tests. Cosmic Chess Academy and Meteor Mayhem are 32 KiB, GBC-only ROMs made in C with GBDK. Midnight Courier is a 128 KiB, GBC-only GB Studio project; its editable scenes and test suite remain together in its folder.

## Build and verify

From the repository root, run `python3 games/meteor-mayhem/tools/make_art.py`, then `cd games/meteor-mayhem && make`. The Makefile uses `GBDK_HOME` or the installed ModRetro managed toolchain. The result is `games/meteor-mayhem/dist/meteor-mayhem.gbc`. Cosmic Chess Academy builds the same way using [its Makefile](games/cosmic-chess-academy/Makefile).

Build any of the four new games with `make -C games/<slug> GBDK_HOME=/path/to/gbdk`. Their READMEs describe repeatable emulator checks and retained runtime screenshots.

Midnight Courier's build and regression instructions are in [its README](games/midnight-courier/README.md). The browser site lives in `docs/` for GitHub Pages; run `python3 tools/sync_site.py` after rebuilding a ROM.

## Embed the games on another site

`docs/` is a small self-contained static site. Copy the whole directory under a site's public path, such as `public/games/`, to serve the collection at `/games/index.html`. Its links and emulator assets use relative paths and explicit `index.html` targets, so the games also work under another prefix without framework rewrites. A host may add clean-URL redirects if desired.

After rebuilding a ROM, run `python3 tools/sync_site.py`. This updates each browser ROM and [the game manifest](docs/games-manifest.json), including exact ROM SHA-256 hashes. A host copying this site can validate the manifest before publishing, so its playable ROMs stay in sync with this repository. The original GitHub Pages site continues to use the same `docs/` source.

To load a ROM onto a Chromatic, use a compatible writable cartridge and the official ModRetro developer workflow. Writing replaces that cartridge's existing game data.

## Rights

Game code and original artwork: [MIT](LICENSE). The browser emulator and GBDK carry separate notices in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
