# Cosmic Chess Academy (work in progress)

A playable teaching prototype for Game Boy Color. DOT, a ship computer, guides three short chess movement missions on an 8×8 board: rook, bishop, and knight. Moves use standard chess movement patterns, including a rook's blocked path, a bishop's diagonal capture, and a knight's jump. The first three lessons deliberately omit check, castling, promotion, and opponent turns; those are planned for later missions after we shape the teaching style together.

Use the D-pad to move the cursor, A to select and move the friendly piece, B to highlight legal destinations, and Start to advance after a solved mission. A wrong move leaves the board unchanged and explains the rule. Start during an unsolved room restarts it.

Run `python3 tools/make_pieces.py` and `make` in this folder to rebuild `dist/cosmic-chess-academy.gbc` with GBDK. The Makefile uses the managed ModRetro toolchain by default on this Mac; `GBDK_HOME` can point to another installation. The 5×7 font is shared with Meteor Mayhem in this collection, while all chess-piece sprites are original to this prototype.

This is a three-mission preview for co-design, listed on the collection's games page with a browser player and downloadable ROM. The longer curriculum and open questions are in [DESIGN.md](DESIGN.md). It has not been installed on a physical cartridge.
