# Meteor Mayhem

A compact arcade shooter for the Game Boy Color. The player ship can move in four directions, fire a rapid laser, and dash through danger. Kill chains multiply the score, repair pickups reward risk, three increasingly fast waves lead to a final boss, and a local best score makes replay worthwhile.

| Action | Chromatic / Game Boy | Browser |
| --- | --- | --- |
| Fly | D-pad | Arrow keys / WASD |
| Fire | A | Z / J |
| Dash | B | X / K |
| Start or retry | Start | Enter |

Hold A to fire. B gives a short invulnerable burst, then recharges. Defeat 30 enemies to summon the boss; defeat it to clear the sky. You have three hit points. A repair pickup appears after certain kill milestones if you are injured. A lost run can be restarted with Start.

## Build

Run `python3 tools/make_art.py` and `make` from this folder. Requires GBDK 4.5 or compatible. The Makefile defaults to the managed ModRetro toolchain on macOS; set `GBDK_HOME` for another installation. The generated `src/art.h` is committed so a standard build needs only GBDK. The final ROM is `dist/meteor-mayhem.gbc`.

The pixel glyphs and sprites are generated from original source patterns in `tools/make_art.py`. The game uses 20 hardware sprites at most and a fixed 20×18 background tilemap. It has no save memory; the best score lasts until power-off or emulator reset.

The ROM is GBC-only, 32 KiB, and valid for real compatible hardware. It has been booted and played with real inputs in PyBoy. Hardware play of this new game has not yet been verified.
