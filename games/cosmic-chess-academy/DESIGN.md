# Cosmic Chess Academy — playable concept

**Pitch:** A tiny rookie pilot repairs a lost starship by solving one real chess idea per room. Every beam, jump, and capture obeys chess movement. The space setting adds stakes and personality, not secret exceptions to the rules.

The first loop is deliberately short: inspect a piece, show the squares it can legally reach, make a move, and receive a sentence explaining *why* it worked or failed. A friendly ship computer, DOT, gives hints without taking the controller away. A solved room lights a new star on the map. Retry is immediate.

## Learning ladder

1. **Rook Rails:** the rook flies along a rank or file; a friendly satellite blocks its beam. Move to the clear beacon.
2. **Bishop Nebula:** a bishop crosses diagonals. Capture the hostile drone at the end of an open diagonal.
3. **Knight Warp:** a knight jumps in an L, even across obstacles. Land on the escape beacon.
4. **Pawn Patrol:** forward movement and diagonal captures are different. Take a diagonal threat.
5. **Queen Comet:** combine rook and bishop movement to find a long-range capture.
6. **King's Shield:** move the king to a safe square, introducing attacked squares.
7. **Fork in the Stars:** use the knight to attack two valuable targets at once.
8. **Orbital Mate:** finish a real checkmate in one, then explain why the king cannot escape.

Lessons 1–3 form the initial playable slice. The later lessons are a curriculum proposal for us to tune together; their chess validation and puzzle positions should be tested before release.

## Controls and feedback

- D-pad: move the board cursor.
- A: select a friendly piece, then choose its destination.
- B: show or hide legal destinations. Press A on the selected piece to cancel a move.
- Start: advance after solving a room, or retry the current unsolved room.

The board is always an 8×8 board, with white moving upward. A cyan cursor marks the current square, mint marks legal destinations, and gold marks the mission target. Legal captures remove the opposing piece. Illegal selections leave the board unchanged and briefly explain the rule. Early lessons have no clock or life penalty.

## What makes it a game

The ship computer reacts to mistakes with short, warm quips. Rooms begin with a concrete mission (“align the rook's rail to power the gate”) and end with a visible star earned. Later rooms combine rules under light pressure. Optional score comes from solving with fewer hints, never from rushing a new learner.

The aim is to teach transferable chess habits: *What squares can this piece reach? What blocks the path? Is the destination safe?* The game should never reward a move that would be illegal in standard chess.

## Open decisions for co-design

- Keep story missions as the main mode, or make them a wrapper around fast arcade challenges?
- Should hints reveal every legal move immediately, or wait for one attempt?
- Should the player control a persistent crew of pieces, or focus on one piece per lesson?
- What tone feels right for DOT: cheeky flight instructor, calm coach, or oddball robot?
