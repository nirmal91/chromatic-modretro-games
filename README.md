# Chromatic ModRetro Games

Original Game Boy Color games, built for ModRetro Chromatic and playable in a browser. Made by Nirmal Utwani with Codex.

**[Play the games](https://nirmal91.github.io/chromatic-modretro-games/)** · [Download ROMs](https://github.com/nirmal91/chromatic-modretro-games/releases)

| Game | What it is | Play | ROM / source |
| --- | --- | --- | --- |
| **Meteor Mayhem** | A fast arcade shooter: dodge incoming rocks, blast drones, chain kills, dash through danger, and defeat the final boss. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/meteor-mayhem/) | [ROM](games/meteor-mayhem/dist/meteor-mayhem.gbc) · [source](games/meteor-mayhem/) |
| **Midnight Courier** | A short neon heist with four sectors, three cores, a cipher, and a timed getaway. | [Play now](https://nirmal91.github.io/chromatic-modretro-games/play/midnight-courier/) | [ROM](games/midnight-courier/dist/midnight-courier.gbc) · [source](games/midnight-courier/) |

Both ROMs are original homebrew. Meteor Mayhem is a 32 KiB, GBC-only ROM made in C with GBDK. Midnight Courier is a 128 KiB, GBC-only GB Studio project; its editable scenes and test suite remain together in its folder.

## Build and verify

From the repository root, run `python3 games/meteor-mayhem/tools/make_art.py`, then `cd games/meteor-mayhem && make`. The Makefile uses `GBDK_HOME` or the installed ModRetro managed toolchain. The result is `games/meteor-mayhem/dist/meteor-mayhem.gbc`.

Midnight Courier's build and regression instructions are in [its README](games/midnight-courier/README.md). The browser site lives in `docs/` for GitHub Pages; run `python3 tools/sync_site.py` after rebuilding either ROM.

To load a ROM onto a Chromatic, use a compatible writable cartridge and the official ModRetro developer workflow. Writing replaces that cartridge's existing game data.

## Rights

Game code and original artwork: [MIT](LICENSE). The browser emulator and GBDK carry separate notices in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
