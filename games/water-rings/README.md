# Water Rings

An original pocket water toy for Game Boy and Game Boy Color, including ModRetro Chromatic. Pump the water jets to float eight rings above the two pegs, then release and let them sink onto the tips. Each peg holds four rings. All eight rings wins; your best time stays available until the console resets.

## Controls

- **A**: left water jet. Hold to raise rings, release to let them land.
- **B**: right water jet. Both buttons pump both jets.
- **Left / Right**: tilt the tank; steer airborne rings.
- **Up / Down**: gentle vertical tilt assist.
- **Start**: start, pause/resume, or play again after winning.
- **Select**: restart the tank at any time during a round.

Release a jet while a ring is above a peg. A ring only locks onto a peg while descending across its tip; floating up through it does not score. Captured rings stay attached. Water drag, buoyancy, gentle gravity, wall rebounds, two bubble streams, original pixel art, and two colored ring sets make this a calm tactile toy rather than a timed failure challenge. The timer stops during pause.

## Build

Install GBDK 4.5 or newer and run:

```sh
make GBDK_HOME=/path/to/gbdk
```

The cartridge is `dist/water-rings.gbc`. It supports monochrome Game Boy rendering and enhances the tank/rings with Game Boy Color palettes. `make art` regenerates the original 2bpp tiles from `tools/make_art.py`; source code and art remain editable without a game editor. There are no commercial ROMs or copied game assets.

## Verified gameplay

The exact built ROM was executed in headless PyBoy in Game Boy Color mode and forced monochrome Game Boy mode. The cartridge header permits both consoles; the Makefile defaults to `/opt/gbdk` and supports an explicit `GBDK_HOME`. Button-driven checks cover visible lifting from the left jet, pause freezing both physics and the timer, Select restart, four rings naturally landing on each peg, a stable eight-ring victory, and Start beginning a new round. Genuine emulator screenshots and the ROM hash are in `evidence/`. Run `python3 tools/verify.py` with PyBoy installed to repeat the checks. Forced monochrome boot, title/start screens, and jet movement were also verified; monochrome framebuffer screenshots are included. Browser playback, sound, and physical Chromatic play have not been verified here.
