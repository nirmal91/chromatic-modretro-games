"""Copy the shipping ROMs into the GitHub Pages browser player."""

import hashlib
import json
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GAMES = {
    "meteor-mayhem": "Meteor Mayhem",
    "midnight-courier": "Midnight Courier",
}

for slug, title in GAMES.items():
    source = ROOT / "games" / slug / "dist" / f"{slug}.gbc"
    target = ROOT / "docs" / "play" / slug / "rom" / f"{slug}.gbc"
    if not source.is_file():
        raise SystemExit(f"Missing shipping ROM: {source}")
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source, target)
    config = {
        "version": 1,
        "rom": f"rom/{slug}.gbc",
        "project": {"name": title, "author": "Nirmal Utwani"},
        "colorCorrection": "default",
        "customControls": {
            "up": ["ArrowUp", "w"],
            "down": ["ArrowDown", "s"],
            "left": ["ArrowLeft", "a"],
            "right": ["ArrowRight", "d"],
            "a": ["z", "j"],
            "b": ["x", "k"],
            "start": ["Enter"],
            "select": ["Shift"],
        },
    }
    (target.parent.parent / "gbstudio.json").write_text(json.dumps(config, indent=2) + "\n")
    digest = hashlib.sha256(source.read_bytes()).hexdigest()
    print(f"{title}: {source.stat().st_size} bytes, SHA-256 {digest}")
