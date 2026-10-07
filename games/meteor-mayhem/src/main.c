#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "art.h"

#define ENEMIES 7
#define SHOTS 4
#define FOE_SHOTS 3
#define OFF 0
#define TITLE 0
#define PLAY 1
#define LOSE 2
#define WIN 3

typedef struct { uint8_t live, x, y, type, hp, phase; } Foe;
typedef struct { uint8_t live, x, y; } Shot;

static const palette_color_t bg_palette[] = {
    RGB8(6, 9, 22), RGB8(159, 226, 225), RGB8(95, 190, 205), RGB8(235, 244, 208)
};
static const palette_color_t player_palette[] = {
    RGB8(6, 9, 22), RGB8(52, 212, 194), RGB8(239, 243, 121), RGB8(255, 255, 247)
};
static const palette_color_t hazard_palette[] = {
    RGB8(6, 9, 22), RGB8(173, 50, 103), RGB8(255, 113, 110), RGB8(255, 216, 146)
};
static const palette_color_t boss_palette[] = {
    RGB8(6, 9, 22), RGB8(105, 65, 178), RGB8(225, 79, 205), RGB8(255, 217, 119)
};

static Foe foes[ENEMIES];
static Shot shots[SHOTS];
static Shot foe_shots[FOE_SHOTS];
static uint8_t frame, rng = 77, state = TITLE, old_keys;
static uint8_t px, py, hp, dash, invincible, fire_timer, kills, wave, boss_hp, boss_x, boss_dir;
static uint8_t combo, combo_timer, pickup_live, pickup_x, pickup_y;
static uint16_t score, best;

static uint8_t random8(void) {
    rng ^= rng << 3;
    rng ^= rng >> 5;
    rng ^= rng << 1;
    rng += DIV_REG;
    return rng;
}

static uint8_t near(uint8_t ax, uint8_t ay, uint8_t bx, uint8_t by, uint8_t dx, uint8_t dy) {
    return ((ax > bx ? ax - bx : bx - ax) < dx && (ay > by ? ay - by : by - ay) < dy);
}

static uint8_t glyph(char c) {
    uint8_t i;
    if (c == ' ') return 0;
    for (i = 0; glyph_chars[i]; ++i) if (glyph_chars[i] == c) return i + 4;
    return 0;
}

static void text(uint8_t x, uint8_t y, const char *s) {
    uint8_t i = 0, tile;
    while (s[i] && x + i < 20) {
        tile = glyph(s[i]);
        set_bkg_tiles(x + i, y, 1, 1, &tile);
        ++i;
    }
}

static void clear_screen(void) {
    uint8_t row[20], y, x;
    for (x = 0; x < 20; ++x) row[x] = 0;
    for (y = 0; y < 18; ++y) set_bkg_tiles(0, y, 20, 1, row);
}

static void number(uint8_t x, uint8_t y, uint16_t n, uint8_t width) {
    uint8_t buf[5], i;
    if (n > 9999) n = 9999;
    for (i = 0; i < width; ++i) { buf[width - 1 - i] = glyph('0' + (n % 10)); n /= 10; }
    set_bkg_tiles(x, y, width, 1, buf);
}

static void hide_all(void) {
    uint8_t i;
    for (i = 0; i < 20; ++i) hide_sprite(i);
}

static void sound(uint8_t pitch, uint8_t volume) {
    NR21_REG = 0x40;
    NR22_REG = volume;
    NR23_REG = pitch;
    NR24_REG = 0x87;
}

static void title(void) {
    uint8_t stars[20], x, y;
    hide_all(); clear_screen();
    for (y = 0; y < 18; ++y) {
        for (x = 0; x < 20; ++x) stars[x] = ((x * 11 + y * 7) % 43 == 0) ? 1 : 0;
        set_bkg_tiles(0, y, 20, 1, stars);
    }
    text(3, 3, "METEOR MAYHEM");
    text(2, 5, "ONE SHIP. NO BRAKES.");
    text(3, 8, "D PAD  FLY");
    text(3, 9, "A      BLAST");
    text(3, 10, "B      DASH");
    text(2, 13, "3 WAVES + A BOSS");
    text(3, 15, "START TO LAUNCH");
    move_sprite(0, 40, 111);
    move_sprite(1, 112, 110);
    move_sprite(2, 128, 118);
    state = TITLE;
}

static void hud(void) {
    text(0, 0, "SCORE"); number(6, 0, score, 4);
    text(12, 0, "HP"); number(15, 0, hp, 1);
    text(0, 17, "WAVE"); number(5, 17, wave, 1);
    text(9, 17, "DASH");
    if (dash == 0) text(14, 17, "READY"); else text(14, 17, "WAIT ");
}

static void arena(void) {
    uint8_t x, y, map[20];
    clear_screen();
    for (x = 0; x < 20; ++x) map[x] = 3;
    set_bkg_tiles(0, 1, 20, 1, map);
    set_bkg_tiles(0, 16, 20, 1, map);
    for (y = 2; y < 16; ++y) {
        for (x = 0; x < 20; ++x) map[x] = ((x * 13 + y * 17) % 29 == 0) ? 1 : 0;
        set_bkg_tiles(0, y, 20, 1, map);
    }
    hud();
}

static void start_game(void) {
    uint8_t i;
    px = 80; py = 112; hp = 3; dash = 0; invincible = 0; fire_timer = 0;
    kills = 0; wave = 1; score = 0; combo = 0; combo_timer = 0;
    boss_hp = 0; boss_x = 72; boss_dir = 1; pickup_live = 0;
    for (i = 0; i < ENEMIES; ++i) foes[i].live = 0;
    for (i = 0; i < SHOTS; ++i) shots[i].live = 0;
    for (i = 0; i < FOE_SHOTS; ++i) foe_shots[i].live = 0;
    frame = 0; arena(); state = PLAY;
    move_sprite(0, px + 8, py + 16);
    sound(0xc0, 0xf2);
}

static void end_game(uint8_t won) {
    hide_all(); clear_screen();
    if (score > best) best = score;
    if (won) {
        text(4, 4, "SKY CLEARED!");
        text(3, 6, "THE CORE IS YOURS");
    } else {
        text(4, 4, "SHIP DOWN!");
        text(2, 6, "THE STARS FIGHT ON");
    }
    text(4, 9, "SCORE"); number(10, 9, score, 4);
    text(4, 10, "BEST"); number(10, 10, best, 4);
    text(2, 14, "START TO RETRY");
    state = won ? WIN : LOSE;
    sound(won ? 0xf0 : 0x38, 0xf5);
}

static void damage(void) {
    if (invincible) return;
    invincible = 50; combo = 0; combo_timer = 0;
    if (hp) --hp;
    sound(0x25, 0xf3);
    if (!hp) end_game(0);
    else hud();
}

static void score_kill(void) {
    if (combo < 5) ++combo;
    combo_timer = 100;
    score += 10 * combo;
    ++kills;
    sound(0xc0 + (combo << 3), 0x81);
    if ((kills % 7) == 0 && hp < 3 && !pickup_live) {
        pickup_live = 1; pickup_x = 24 + (random8() % 112); pickup_y = 28;
    }
    if (kills == 8 || kills == 19) { ++wave; arena(); }
    if (kills == 30) { wave = 4; boss_hp = 14; arena(); }
    hud();
}

static void spawn_foe(void) {
    uint8_t i;
    if (boss_hp) return;
    for (i = 0; i < ENEMIES; ++i) if (!foes[i].live) {
        foes[i].live = 1;
        foes[i].x = 12 + (random8() % 136);
        foes[i].y = 18;
        foes[i].type = (random8() % 4 == 0) ? 1 : 0;
        foes[i].hp = foes[i].type ? 1 : 2;
        foes[i].phase = random8();
        return;
    }
}

static void fire(void) {
    uint8_t i;
    for (i = 0; i < SHOTS; ++i) if (!shots[i].live) {
        shots[i].live = 1; shots[i].x = px; shots[i].y = py - 5;
        fire_timer = 9;
        sound(0xe0, 0x52);
        return;
    }
}

static void foe_fire(uint8_t x, uint8_t y) {
    uint8_t i;
    for (i = 0; i < FOE_SHOTS; ++i) if (!foe_shots[i].live) {
        foe_shots[i].live = 1; foe_shots[i].x = x; foe_shots[i].y = y;
        return;
    }
}

static void move_player(uint8_t keys) {
    uint8_t speed = 2;
    if ((keys & J_B) && !(old_keys & J_B) && !dash) {
        dash = 80; invincible = 14; speed = 7; sound(0x45, 0xa1);
    } else if (dash > 68) speed = 5;
    if ((keys & J_LEFT) && px > 10 + speed) px -= speed;
    if ((keys & J_RIGHT) && px < 150 - speed) px += speed;
    if ((keys & J_UP) && py > 23 + speed) py -= speed;
    if ((keys & J_DOWN) && py < 121 - speed) py += speed;
    if (dash) --dash;
    if (invincible) --invincible;
    if (fire_timer) --fire_timer;
    if ((keys & J_A) && !fire_timer) fire();
    if (invincible && ((frame & 3) < 2)) hide_sprite(0);
    else move_sprite(0, px + 8, py + 16);
}

static void tick_shots(void) {
    uint8_t i, j;
    for (i = 0; i < SHOTS; ++i) {
        if (!shots[i].live) { hide_sprite(8 + i); continue; }
        if (shots[i].y < 19) { shots[i].live = 0; hide_sprite(8 + i); continue; }
        shots[i].y -= 4;
        if (boss_hp && near(shots[i].x, shots[i].y, boss_x + 8, 37, 13, 12)) {
            shots[i].live = 0;
            if (--boss_hp == 0) { score += 500; end_game(1); return; }
            sound(0x78, 0x91);
        }
        for (j = 0; j < ENEMIES && shots[i].live; ++j) {
            if (foes[j].live && near(shots[i].x, shots[i].y, foes[j].x, foes[j].y, 7, 7)) {
                shots[i].live = 0;
                if (--foes[j].hp == 0) { foes[j].live = 0; score_kill(); }
                else sound(0x80, 0x71);
            }
        }
        if (shots[i].live) move_sprite(8 + i, shots[i].x + 8, shots[i].y + 16);
        else hide_sprite(8 + i);
    }
}

static void tick_foes(void) {
    uint8_t i;
    for (i = 0; i < ENEMIES; ++i) {
        if (!foes[i].live) { hide_sprite(1 + i); continue; }
        if (foes[i].type && (frame & 7) == 0) {
            if (foes[i].phase & 1) foes[i].x += 2; else foes[i].x -= 2;
            if (foes[i].x < 14 || foes[i].x > 146) foes[i].phase ^= 1;
        }
        if ((frame & 1) == 0 || wave > 2) ++foes[i].y;
        if (foes[i].type && ((frame + foes[i].phase) % 90 == 0)) foe_fire(foes[i].x, foes[i].y);
        if (foes[i].y > 122) { foes[i].live = 0; hide_sprite(1 + i); continue; }
        if (near(px, py, foes[i].x, foes[i].y, 8, 8)) damage();
        if (state != PLAY) return;
        move_sprite(1 + i, foes[i].x + 8, foes[i].y + 16);
    }
    for (i = 0; i < FOE_SHOTS; ++i) {
        if (!foe_shots[i].live) { hide_sprite(12 + i); continue; }
        foe_shots[i].y += 2;
        if (foe_shots[i].y > 124) { foe_shots[i].live = 0; hide_sprite(12 + i); continue; }
        if (near(px, py, foe_shots[i].x, foe_shots[i].y, 6, 6)) {
            foe_shots[i].live = 0; hide_sprite(12 + i); damage();
        } else move_sprite(12 + i, foe_shots[i].x + 8, foe_shots[i].y + 16);
        if (state != PLAY) return;
    }
}

static void tick_boss(void) {
    uint8_t i;
    if (!boss_hp) { for (i = 16; i < 20; ++i) hide_sprite(i); return; }
    if ((frame & 1) == 0) {
        if (boss_dir) ++boss_x; else --boss_x;
        if (boss_x > 134) boss_dir = 0;
        if (boss_x < 12) boss_dir = 1;
    }
    if ((frame % 40) == 0) foe_fire(boss_x + 8, 46);
    if (near(px, py, boss_x + 8, 37, 13, 13)) damage();
    if (state != PLAY) return;
    move_sprite(16, boss_x + 8, 45);
    move_sprite(17, boss_x + 16, 45);
    move_sprite(18, boss_x + 8, 53);
    move_sprite(19, boss_x + 16, 53);
}

static void tick_pickup(void) {
    if (!pickup_live) { hide_sprite(15); return; }
    if ((frame & 1) == 0) ++pickup_y;
    if (pickup_y > 124) { pickup_live = 0; hide_sprite(15); return; }
    if (near(px, py, pickup_x, pickup_y, 8, 8)) {
        pickup_live = 0; if (hp < 3) ++hp; score += 25;
        sound(0xf5, 0xa2); hud(); hide_sprite(15); return;
    }
    move_sprite(15, pickup_x + 8, pickup_y + 16);
}

void main(void) {
    uint8_t i, keys;
    DISPLAY_OFF;
    set_bkg_data(0, BG_TILE_COUNT, bg_tiles);
    set_sprite_data(0, SPRITE_TILE_COUNT, sprite_tiles);
    set_bkg_palette(0, 1, bg_palette);
    set_sprite_palette(0, 1, player_palette);
    set_sprite_palette(1, 1, hazard_palette);
    set_sprite_palette(2, 1, boss_palette);
    set_sprite_prop(0, 0);
    for (i = 1; i <= 7; ++i) set_sprite_prop(i, 1);
    for (i = 8; i <= 11; ++i) set_sprite_prop(i, 0);
    for (i = 12; i <= 14; ++i) set_sprite_prop(i, 1);
    set_sprite_prop(15, 0);
    for (i = 16; i < 20; ++i) set_sprite_prop(i, 2);
    set_sprite_tile(0, 0);
    for (i = 1; i <= 7; ++i) set_sprite_tile(i, 1);
    for (i = 8; i <= 11; ++i) set_sprite_tile(i, 3);
    for (i = 12; i <= 14; ++i) set_sprite_tile(i, 4);
    set_sprite_tile(15, 5);
    for (i = 16; i < 20; ++i) set_sprite_tile(i, i - 10);
    NR52_REG = 0x80; NR50_REG = 0x77; NR51_REG = 0xff;
    SHOW_BKG; SHOW_SPRITES; SPRITES_8x8;
    title(); DISPLAY_ON;
    while (1) {
        wait_vbl_done();
        keys = joypad(); ++frame;
        if (state != PLAY) {
            if ((keys & J_START) && !(old_keys & J_START)) start_game();
            old_keys = keys; continue;
        }
        move_player(keys);
        if ((frame % (wave == 1 ? 34 : wave == 2 ? 26 : 19)) == 0) spawn_foe();
        tick_shots();
        if (state == PLAY) tick_foes();
        if (state == PLAY) tick_boss();
        if (state == PLAY) tick_pickup();
        if (combo_timer && --combo_timer == 0) combo = 0;
        if ((frame & 15) == 0 && state == PLAY) hud();
        old_keys = keys;
    }
}
