# Five original Chromatic arcade games

These are five separate, 32 KiB, Game Boy Color-only ROMs with one shared GBDK codebase. Each has a different three-round game loop, original pixel art, a score, health, restart, and its own browser page. They take broad arcade-mechanic inspiration from classic games; they do not use those games' names, code, graphics, or sound.

| Game | Controls | Goal |
| --- | --- | --- |
| Cargo Crush | D-pad push, A pulse, hold B to brake | Crush or pulse every pursuer through three depot rounds. |
| Storm Riders | Left/right steer, A flap, B dash | Win aerial collisions by diving from above. |
| Parcel Panic | Up/down lane, A send, B catch | Deliver parcels in four lanes and catch the returns. |
| Lost Drones | D-pad move, A dash, B decoy | Pick up three drones and escort them to the gate. |
| Airlock Alert | Left/right door, A fire, B scan | Shoot raiders, spare friendlies, survive three shifts. |

`make -C games/arcade-five` builds the five ROMs with the ModRetro managed GBDK compiler (override `GBDK_HOME` for another GBDK install). `python3 games/arcade-five/tools/make_art.py` regenerates original tile data from editable pixel patterns. Run `python3 games/arcade-five/tests/playtest.py` with PyBoy installed to boot every exact ROM and exercise real inputs; images go to `tests/evidence/`. Run `python3 tools/sync_site.py` from the repository root to copy rebuilt ROMs and SHA-256 hashes into the browser site.

The ROMs were emulator tested; physical cartridge play of these new games has not yet been verified. A compatible writable cartridge is needed to play them on a Chromatic.
