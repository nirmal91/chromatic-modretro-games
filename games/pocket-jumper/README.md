# Pocket Jumper

An original Game Boy Color homebrew platformer inspired by classic side-scrolling games. A tiny red explorer crosses four stretches of floating ledges, gaps, purple critters, and twenty coins to reach the flag. Original characters and tile art; no Nintendo code or assets.

- **D-pad:** move left/right.
- **A:** jump; hold for a high jump, release for a shorter hop.
- **B:** hold to run, especially over wide gaps.
- **Start:** start, pause, resume, or restart after the ending.

Land on critters to bounce and defeat them. Touching them costs a life; falling into a gap also costs a life. Three lives, brief protection after respawning, and checkpoints every stretch keep the adventure approachable. Reach the flag for a coin-count ending. There is no battery save.

## Build

Install GBDK 2020 and run `make GBDK_HOME=/path/to/gbdk`. The cartridge is `dist/pocket-jumper.gbc`, playable on Game Boy Color, ModRetro Chromatic, or compatible emulators. Art can be edited in `tools/make_art.py`; `make` regenerates `src/art.h`.

## Verification

Run `python3 tests/playtest.py` with PyBoy installed after building. The input-driven emulator check starts the cartridge, walks, jumps, pauses/resumes, scrolls through the full level and reaches the flag, collects a coin, deliberately falls into a pit three times, and restarts from game over. It observes linker symbols without writing game memory. Screenshots are saved under `dist/`. The ROM was built with GBDK 4.5.0 and passed this check; physical cartridge transfer has not been tested.
