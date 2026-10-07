"""Real-input PyBoy smoke and progression checks for the shipping ROM."""

import hashlib
import json
from pathlib import Path

from pyboy import PyBoy

ROOT = Path(__file__).resolve().parents[1]
ROM = ROOT / "dist" / "meteor-mayhem.gbc"
OUT = ROOT / "tests" / "evidence"
OUT.mkdir(exist_ok=True)


def tile(p, x, y):
    return p.tilemap_background[x, y] - 256


def glyph(ch):
    chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:/-+!.?<>"
    return 0 if ch == " " else 4 + chars.index(ch)


def read_text(p, x, y, text):
    return all(tile(p, x + i, y) == glyph(ch) for i, ch in enumerate(text))


def hold(p, button, frames):
    p.button_press(button)
    p.tick(frames, render=True)
    p.button_release(button)


p = PyBoy(str(ROM), window="null", cgb=True)
checks = []
try:
    p.tick(120, render=True)
    assert read_text(p, 3, 3, "METEOR MAYHEM")
    checks.append("title rendered")
    p.screen.image.save(OUT / "title.png")

    hold(p, "start", 5)
    # The native game draws all 18 tile rows before its first gameplay frame.
    for _ in range(180):
        p.tick(1, render=True)
        if read_text(p, 0, 0, "SCORE") and read_text(p, 0, 17, "WAVE"):
            break
    assert read_text(p, 0, 0, "SCORE")
    assert read_text(p, 0, 17, "WAVE")
    checks.append("Start enters gameplay with HUD")

    x0 = p.get_sprite(0).x
    hold(p, "right", 12)
    x1 = p.get_sprite(0).x
    assert x1 >= x0 + 20, (x0, x1)
    checks.append("D-pad moves ship")

    p.button_press("right")
    hold(p, "b", 2)
    p.tick(8, render=True)
    p.button_release("right")
    x2 = p.get_sprite(0).x
    assert x2 >= x1 + 37, (x1, x2)
    assert read_text(p, 9, 17, "DASH") and read_text(p, 14, 17, "WAIT")
    checks.append("B dash moves faster and enters cooldown")

    hold(p, "a", 12)
    assert any(p.get_sprite(i).on_screen for i in range(8, 12))
    checks.append("A fires genuine laser sprites")
    p.screen.image.save(OUT / "gameplay.png")

    # Chase naturally spawned enemies while holding A. No RAM writes or scene skips.
    p.button_press("a")
    direction = None
    last_x = p.get_sprite(0).x
    reached_boss = False
    for frame in range(2400):
        ship = p.get_sprite(0)
        if ship.on_screen:
            last_x = ship.x
        targets = [p.get_sprite(i) for i in range(1, 8)]
        targets = [e for e in targets if e.on_screen and 20 < e.y < 100]
        target_x = max(targets, key=lambda e: e.y).x if targets else 80
        next_direction = "right" if target_x > last_x + 3 else "left" if target_x < last_x - 3 else None
        if next_direction != direction:
            if direction:
                p.button_release(direction)
            if next_direction:
                p.button_press(next_direction)
            direction = next_direction
        p.tick(1, render=(frame % 60 == 0))
        if tile(p, 5, 17) == glyph("4"):
            reached_boss = True
            p.tick(1, render=True)
            p.screen.image.save(OUT / "boss-wave.png")
            break
    assert reached_boss, "Could not reach boss wave through ordinary input"
    assert any(tile(p, 6 + i, 0) != glyph("0") for i in range(4))
    checks.append("Kills award score and three waves lead to boss")

    # Replay a clean run for the finish, using only controller input and visible sprites.
    p.stop()
    p = PyBoy(str(ROM), window="null", cgb=True)
    p.tick(120)
    hold(p, "start", 5)
    p.tick(70)
    p.button_press("a")
    direction = None
    last_x = 80
    old_boss_x = 72
    boss_phase = False
    won = False
    for frame in range(2500):
        ship = p.get_sprite(0)
        if ship.on_screen:
            last_x = ship.x
        boss = p.get_sprite(16)
        if boss.on_screen:
            boss_phase = True
        if boss_phase and boss.on_screen:
            dx = boss.x - old_boss_x
            old_boss_x = boss.x
            target_x = boss.x + 8 + (9 if dx >= 0 else -9)
        else:
            targets = [p.get_sprite(i) for i in range(1, 8)]
            targets = [e for e in targets if e.on_screen and 20 < e.y < 100]
            target_x = max(targets, key=lambda e: e.y).x if targets else 80
        next_direction = "right" if target_x > last_x + 3 else "left" if target_x < last_x - 3 else None
        if next_direction != direction:
            if direction:
                p.button_release(direction)
            if next_direction:
                p.button_press(next_direction)
            direction = next_direction
        if boss_phase and frame % 80 == 0:
            p.button_press("b")
        if boss_phase and frame % 80 == 2:
            p.button_release("b")
        p.tick(1, render=(frame % 60 == 0))
        if read_text(p, 4, 4, "SKY CLEARED!"):
            won = True
            p.tick(1, render=True)
            p.screen.image.save(OUT / "victory.png")
            break
        assert not read_text(p, 4, 4, "SHIP DOWN!"), "Bot lost before defeating boss"
    assert won, "Boss did not fall to real player inputs"
    checks.append("Final boss defeated and victory shown")
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
