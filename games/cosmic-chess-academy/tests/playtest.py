"""Input-only regression for the first three chess lessons."""

import hashlib
import json
from pathlib import Path

from pyboy import PyBoy

ROOT = Path(__file__).resolve().parents[1]
ROM = ROOT / "dist" / "cosmic-chess-academy.gbc"
OUT = ROOT / "tests" / "evidence"
OUT.mkdir(exist_ok=True)
GLYPHS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:/-+!.?<>"


def glyph(ch):
    return 0 if ch == " " else 4 + GLYPHS.index(ch)


def reads(p, x, y, s):
    return all(p.tilemap_background[x + i, y] - 256 == glyph(ch) for i, ch in enumerate(s))


def tap(p, button, settle=90):
    p.button_press(button)
    p.tick(3, render=True)
    p.button_release(button)
    p.tick(settle, render=True)


p = PyBoy(str(ROM), window="null", cgb=True)
checks = []
try:
    p.tick(120, render=True)
    assert reads(p, 3, 3, "COSMIC CHESS")
    p.screen.image.save(OUT / "title.png")
    checks.append("title boots")

    tap(p, "start", 130)
    assert reads(p, 0, 16, "ROOK RIDES A RAIL")
    assert (p.get_sprite(0).x, p.get_sprite(0).y) == (4, 100)
    p.screen.image.save(OUT / "rook.png")
    checks.append("rook lesson loads on 8x8 board")

    tap(p, "a")
    for _ in range(3):
        tap(p, "up")
    tap(p, "a")
    assert (p.get_sprite(0).x, p.get_sprite(0).y) == (4, 100)
    assert reads(p, 0, 16, "A SHIP BLOCKS PATH")
    checks.append("rook cannot cross friendly blocker")

    tap(p, "b")
    assert reads(p, 0, 16, "MINT: LEGAL MOVES")
    checks.append("B explains highlighted legal destinations")

    for _ in range(3):
        tap(p, "down")
    for _ in range(3):
        tap(p, "right")
    tap(p, "a")
    assert (p.get_sprite(0).x, p.get_sprite(0).y) == (52, 100)
    assert reads(p, 0, 16, "STAR SECURED!")
    checks.append("rook travels along rank to beacon")

    tap(p, "start", 130)
    assert reads(p, 0, 16, "BISHOP CUTS SPACE")
    assert (p.get_sprite(0).x, p.get_sprite(0).y) == (36, 84)
    tap(p, "a")
    for _ in range(3):
        tap(p, "right")
    for _ in range(3):
        tap(p, "up")
    tap(p, "a")
    assert reads(p, 0, 16, "STAR SECURED!")
    assert not p.get_sprite(2).on_screen
    p.screen.image.save(OUT / "bishop-capture.png")
    checks.append("bishop captures on an open diagonal")

    tap(p, "start", 130)
    assert reads(p, 0, 16, "KNIGHT CAN JUMP")
    tap(p, "a")
    tap(p, "right")
    tap(p, "up")
    tap(p, "up")
    tap(p, "a")
    assert reads(p, 0, 16, "STAR SECURED!")
    assert (p.get_sprite(0).x, p.get_sprite(0).y) == (68, 36)
    p.screen.image.save(OUT / "knight-warp.png")
    checks.append("knight jumps two plus one over pieces")

    tap(p, "start", 130)
    assert reads(p, 2, 3, "CADET GRADUATED!")
    p.screen.image.save(OUT / "graduation.png")
    checks.append("three missions lead to graduation")

    tap(p, "start", 130)
    assert reads(p, 0, 16, "ROOK RIDES A RAIL")
    checks.append("graduation replay resets to lesson one")
finally:
    p.stop()

result = {
    "rom": str(ROM.relative_to(ROOT)),
    "sha256": hashlib.sha256(ROM.read_bytes()).hexdigest(),
    "checks": checks,
    "status": "passed",
}
(OUT / "results.json").write_text(json.dumps(result, indent=2) + "\n")
print(json.dumps(result, indent=2))
