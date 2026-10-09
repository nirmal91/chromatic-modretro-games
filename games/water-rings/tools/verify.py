"""Play the actual built cartridge with PyBoy; save genuine framebuffer evidence."""
from pathlib import Path
import hashlib
import json
from pyboy import PyBoy

root = Path(__file__).resolve().parent.parent
rom = root / "dist/water-rings.gbc"
evidence = root / "evidence"
evidence.mkdir(exist_ok=True)
boy = PyBoy(str(rom), window="null", sound_emulated=False)
boy.set_emulation_speed(0)

def step(buttons, frames):
    for key in ["a", "b", "start", "select", "left", "right", "up", "down"]:
        if key in buttons:
            boy.button_press(key)
        else:
            boy.button_release(key)
    boy.tick(frames)

def positions():
    return [(boy.memory[0xFE01+i*4]-4, boy.memory[0xFE00+i*4]-12) for i in range(8)]

def capture(name):
    boy.screen.image.save(evidence / (name + ".png"))

def landed():
    return sum(x in (55, 103) and y in (81, 89, 97, 105) for x, y in positions())

checks = []
try:
    step([], 100)
    capture("title")
    step(["start"], 2)
    step([], 60)
    capture("tank")
    before = positions()
    step(["a"], 30)
    raised = positions()
    assert any(y2 < y1 - 10 for (_, y1), (_, y2) in zip(before, raised)), "A must lift rings"
    capture("left-jet")
    step(["start"], 2)
    step([], 2)
    paused = positions()
    paused_frame = boy.screen.ndarray.copy()
    step([], 120)
    assert positions() == paused, "Pause must freeze ring physics"
    assert (boy.screen.ndarray == paused_frame).all(), "Pause must freeze timer and scene"
    capture("paused")
    checks.append("A jet visibly raises rings; Start pause freezes physics and timer")
    step(["select"], 2)
    step([], 2)
    assert landed() == 0, "Select must reset the round"
    step(["a"], 120)
    step([], 150)
    assert landed() == 4, "A/release should land four rings on the left peg"
    capture("four-landed")
    step(["b"], 150)
    step([], 150)
    assert landed() == 8, "B/release should land four remaining rings on the right peg"
    capture("win")
    win_positions = positions()
    step([], 120)
    assert positions() == win_positions, "Win must hold all eight rings"
    checks.append("Natural input lands four rings on each peg and enters stable win state")
    step(["start"], 2)
    step([], 5)
    assert landed() == 0, "Start after winning must begin a fresh round"
    capture("restart")
    checks.append("Select resets during play; Start restarts after winning")
    dmg = PyBoy(str(rom), window="null", sound_emulated=False, cgb=False)
    try:
        dmg.set_emulation_speed(0)
        dmg.tick(100)
        dmg.screen.image.save(evidence / "dmg-title.png")
        dmg.button_press("start")
        dmg.tick(2)
        dmg.button_release("start")
        dmg.tick(60)
        dmg.screen.image.save(evidence / "dmg-tank.png")
        resting = [dmg.memory[0xFE00+i*4] for i in range(8)]
        dmg.button_press("a")
        dmg.tick(30)
        assert any(dmg.memory[0xFE00+i*4] < resting[i]-10 for i in range(8)), "DMG jet must lift rings"
        dmg.screen.image.save(evidence / "dmg-jet.png")
    finally:
        dmg.stop(save=False)
    checks.append("Forced monochrome DMG boot/title/start and jet movement verified")
    result = {"rom": "dist/water-rings.gbc", "sha256": hashlib.sha256(rom.read_bytes()).hexdigest(),
              "method": "Built GBDK cartridge executed directly in headless PyBoy; only button input used",
              "checks": checks, "limits": "No physical device, browser emulator, audio or long-term save testing"}
    (evidence / "verification.json").write_text(json.dumps(result, indent=2)+"\n")
    print(json.dumps(result, indent=2))
finally:
    boy.stop(save=False)
