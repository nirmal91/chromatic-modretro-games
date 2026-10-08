"""Run the actual ROM with PyBoy; no emulated-memory writes or injected state."""
from collections import deque
from pathlib import Path
import re
import subprocess
import tempfile
import os

from pyboy import PyBoy

ROOT = Path(__file__).resolve().parent.parent
EVIDENCE = ROOT / "dist"
GBDK = Path(os.environ.get("GBDK_HOME", "/opt/gbdk"))

with tempfile.TemporaryDirectory() as temp:
    rom = Path(temp) / "pocket-snake.gbc"
    subprocess.run([str(GBDK / "bin/lcc"), "-debug", "-Wm-yc", "-Wm-ynPOCKET_SNAKE", "-o", str(rom), "src/main.c"], cwd=ROOT, check=True)
    symbols = dict((name, int(address, 16)) for name, address in re.findall(r"DEF Fmain\$(\w+)\$0_0\$0 0x([0-9A-Fa-f]+)", rom.with_suffix(".noi").read_text()))
    p = PyBoy(str(rom), window="null", sound_emulated=False, cgb=os.environ.get("TEST_DMG") != "1")
    p.set_emulation_speed(0)
    def value(name):
        return p.memory[symbols[name]]
    def score():
        return value("score") + 256 * p.memory[symbols["score"]+1]
    def body():
        return [(p.memory[symbols["xs"]+i], p.memory[symbols["ys"]+i]) for i in range(value("length"))]
    def press(button, frames=2):
        p.button_press(button); p.tick(frames); p.button_release(button); p.tick(2)
    def shot(name):
        prefix = "dmg-" if os.environ.get("TEST_DMG") == "1" else ""
        p.screen.image.save(str(EVIDENCE / (prefix + name + ".png")))
    p.tick(300); shot("title")
    press("a"); p.tick(25)
    assert value("state") == 1 and value("length") == 4
    shot("play")
    before = body()[0]
    p.button_press("left")
    for _ in range(20):
        p.tick(1)
        if body()[0] != before: break
    p.button_release("left")
    assert body()[0] == (before[0]+1, before[1]), "Reverse turn must be ignored"
    press("start"); p.tick(25)
    assert value("state") == 2
    paused = body(); p.tick(60)
    assert body() == paused, "Pause must stop movement"
    shot("paused")
    press("select"); p.tick(25)
    assert value("state") == 1 and score() == 0

    moves = [(1,0,"right"),(0,1,"down"),(-1,0,"left"),(0,-1,"up")]
    # Choose each bounded next input from the genuine current runtime state.
    for _ in range(150):
        if score() >= 50: break
        current = body(); target = (value("food_x"),value("food_y"))
        q = deque([(current[0], [])]); seen={current[0]}; blocked=set(current[1:])
        path = None
        while q:
            at, route = q.popleft()
            if at == target: path=route; break
            for direction,(dx,dy,button) in enumerate(moves):
                if not route and direction == (value("direction") ^ 2): continue
                nxt=(at[0]+dx,at[1]+dy)
                if 0<=nxt[0]<18 and 0<=nxt[1]<13 and nxt not in blocked and nxt not in seen:
                    seen.add(nxt);q.append((nxt,route+[button]))
        assert path, "Food must remain reachable during the test route"
        button=path[0]; p.button_press(button)
        before=current[0]
        for _ in range(30):
            p.tick(1)
            if body()[0] != before or value("state") != 1: break
        p.button_release(button)
        p.tick(4)
        assert value("state") == 1, "Safe planned input collided"
    assert score() >= 50 and value("length") == 9 and value("speed") == 11
    p.tick(2); shot("grown")
    p.button_press("right");p.tick(300);p.button_release("right")
    assert value("state") == 3, "Collision must end the game"
    shot("game-over")
    press("a");p.tick(25)
    assert value("state") == 1 and score() == 0 and value("length") == 4
    assert value("best") >= 50
    p.stop()
    print("PASS: title/start, reverse prevention, pause, pause restart, five fruit growth, speed increase, collision, retry, RAM best score")
