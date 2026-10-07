"""Generate original 8x8 chess and mission sprites as Game Boy 2bpp tiles."""

from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SPRITES = [
    # Rook: battlement, tower, foundation.
    [".1.11.1.", ".111111.", ".122221.", ".122221.", ".122221.", ".122221.", "11111111", ".333333."],
    # Bishop: pointed helm, diagonal stripe.
    ["...11...", "..1221..", "..1221..", ".123321.", ".122221.", ".122221.", "11111111", ".333333."],
    # Knight: a horse-like space jumper.
    ["...111..", "..12221.", "..122221", ".1232221", ".122221.", ".12221..", "11111111", ".333333."],
    # Pawn: little scout pod.
    ["...11...", "..1221..", "..1221..", "...11...", "..1221..", ".122221.", "11111111", "........"],
    # Queen: crown and fusion ship.
    [".1.11.1.", ".121121.", ".122221.", ".123321.", ".122221.", ".122221.", "11111111", ".333333."],
    # King: cross-like antenna and hull.
    ["...11...", ".111111.", "...11...", ".122221.", ".122221.", ".122221.", "11111111", ".333333."],
    # Beacon: destination star.
    ["...1....", "..121...", ".12221..", "12232221", ".12221..", "..121...", "...1....", "........"],
]


def encode(rows):
    data = []
    for row in rows:
        values = [0 if c == "." else int(c) for c in row]
        data += [
            sum((v & 1) << (7 - i) for i, v in enumerate(values)),
            sum(((v >> 1) & 1) << (7 - i) for i, v in enumerate(values)),
        ]
    return data


data = sum((encode(rows) for rows in SPRITES), [])
(ROOT / "src").mkdir(exist_ok=True)
(ROOT / "src" / "pieces.h").write_text(
    "/* Generated from tools/make_pieces.py. */\n"
    "#ifndef COSMIC_PIECES_H\n#define COSMIC_PIECES_H\n"
    "#include <stdint.h>\n"
    f"#define PIECE_TILE_COUNT {len(SPRITES)}\n"
    "static const uint8_t piece_tiles[] = {" + ",".join(f"0x{x:02x}" for x in data) + "};\n"
    "#endif\n"
)
