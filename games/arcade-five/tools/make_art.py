"""Original 8x8 art shared by the five Chromatic arcade games."""
from pathlib import Path

background = [
    # Cargo crate, deck rail, rescue gate, airlock frame
    [".111111.","12222221","12333321","12322321","12322321","12333321","12222221",".111111."],
    ["11111111","22222222","........","........","........","........","22222222","11111111"],
    ["2......2","22....22","222..222","22233222","22233222","222..222","22....22","2......2"],
    [".111111.","12222221","12....21","12....21","12....21","12....21","12222221",".111111."],
]
sprites = [
    # Player pilot, pursuer, storm wing, storm rival, parcel, customer, rescue drone, hostile, civilian, reticle
    ["...22...","..2332..",".233332.","23333332","22333322","..2..2..",".2....2.","........"],
    [".333333.","32222223","32322323","32222223",".322223.",".3.33.3.","3......3","........"],
    ["........","2......2","22.33.22",".233332.","..2332..","...22...","..2..2..",".2....2."],
    ["........","3......3","33.22.33",".322223.","..3223..","...33...","..3..3..",".3....3."],
    ["..1111..",".122221.","12233221","12333321","12333321","12233221",".122221.","..1111.."],
    ["..2222..",".233332.","23322332","23333332",".233332.","..2..2..",".2....2.","........"],
    ["...33...","..3223..",".322223.","32222223",".322223.","..3..3..",".3....3.","........"],
    [".333333.","32222223","32333323","32322323","32333323","32222223",".3.33.3.","3......3"],
    ["...22...","..2332..","..2332..","...22...","..2332..",".23..32.",".2....2.","........"],
    ["3......3",".3....3.","..3..3..","...33...","...33...","..3..3..",".3....3.","3......3"],
]

def encode(tiles):
    values=[]
    for tile in tiles:
        for row in tile:
            assert len(row)==8
            lo=hi=0
            for ch in row:
                n=0 if ch=='.' else int(ch)
                lo=(lo<<1)|(n&1)
                hi=(hi<<1)|((n>>1)&1)
            values.extend((lo,hi))
    return ','.join(f'0x{n:02x}' for n in values)

path=Path(__file__).resolve().parents[1]/'src'/'art_extra.h'
path.write_text('/* Generated from original patterns in tools/make_art.py. */\n#ifndef ARCADE_EXTRA_ART_H\n#define ARCADE_EXTRA_ART_H\n'
    f'#define EXTRA_BG_COUNT {len(background)}\n#define EXTRA_SPRITE_COUNT {len(sprites)}\n'
    f'static const uint8_t extra_bg_tiles[] = {{{encode(background)}}};\n'
    f'static const uint8_t extra_sprite_tiles[] = {{{encode(sprites)}}};\n#endif\n')
