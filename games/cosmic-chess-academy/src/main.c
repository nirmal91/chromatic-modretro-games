#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "../../meteor-mayhem/src/art.h" /* Shared original collection font. */
#include "pieces.h"

#define ROOK 0
#define BISHOP 1
#define KNIGHT 2
#define PAWN 3
#define QUEEN 4
#define KING 5
#define BEACON 6

#define MODE_TITLE 0
#define MODE_LESSON 1
#define MODE_VICTORY 2

typedef struct {
    uint8_t piece, x, y, friend_piece, friend_x, friend_y;
    uint8_t enemy_piece, enemy_x, enemy_y, enemy_exists;
    uint8_t goal_x, goal_y, capture_goal;
    const char *intro1, *intro2, *rule;
} Lesson;

static const Lesson lessons[3] = {
    {ROOK, 0, 6, PAWN, 0, 4, PAWN, 7, 0, 0, 3, 6, 0,
     "ROOK RIDES A RAIL", "FLY STRAIGHT TO D2", "ROOK: STRAIGHT LINE"},
    {BISHOP, 2, 5, PAWN, 1, 4, PAWN, 5, 2, 1, 5, 2, 1,
     "BISHOP CUTS SPACE", "CAPTURE DRONE F6", "BISHOP: DIAGONAL"},
    {KNIGHT, 3, 4, PAWN, 4, 4, PAWN, 3, 3, 0, 4, 2, 0,
     "KNIGHT CAN JUMP", "WARP TO STAR E6", "KNIGHT: TWO + ONE"},
};

static const palette_color_t board_colors[32] = {
    /* 0 dark square */ RGB8(5, 9, 28), RGB8(35, 44, 87), RGB8(142, 174, 213), RGB8(244, 247, 245),
    /* 1 light square */ RGB8(5, 9, 28), RGB8(70, 86, 143), RGB8(187, 213, 219), RGB8(244, 247, 245),
    /* 2 unused */ RGB8(5, 9, 28), RGB8(29, 40, 72), RGB8(142, 174, 213), RGB8(244, 247, 245),
    /* 3 legal */ RGB8(5, 9, 28), RGB8(42, 146, 140), RGB8(108, 242, 206), RGB8(244, 247, 245),
    /* 4 cursor */ RGB8(5, 9, 28), RGB8(78, 186, 224), RGB8(162, 244, 248), RGB8(244, 247, 245),
    /* 5 goal */ RGB8(5, 9, 28), RGB8(197, 146, 55), RGB8(255, 216, 102), RGB8(244, 247, 245),
    /* 6 selected */ RGB8(5, 9, 28), RGB8(145, 83, 211), RGB8(223, 155, 248), RGB8(244, 247, 245),
    /* 7 text */ RGB8(5, 9, 28), RGB8(230, 241, 238), RGB8(115, 227, 210), RGB8(255, 255, 255),
};
static const palette_color_t ship_colors[12] = {
    RGB8(5, 9, 28), RGB8(40, 193, 196), RGB8(219, 247, 231), RGB8(255, 224, 117),
    RGB8(5, 9, 28), RGB8(177, 55, 122), RGB8(255, 119, 142), RGB8(255, 220, 175),
    RGB8(5, 9, 28), RGB8(203, 140, 53), RGB8(255, 224, 112), RGB8(255, 248, 214),
};

static uint8_t game_mode = MODE_TITLE, lesson_index, cx, cy, px, py;
static uint8_t friend_x, friend_y, enemy_x, enemy_y, enemy_exists;
static uint8_t selected, hints, solved, old_keys;

static uint8_t absdiff(uint8_t a, uint8_t b) { return a > b ? a - b : b - a; }

static uint8_t glyph(char c) {
    uint8_t i;
    if (c == ' ') return 0;
    for (i = 0; glyph_chars[i]; ++i) if (glyph_chars[i] == c) return i + 4;
    return 0;
}

static void text(uint8_t x, uint8_t y, const char *s) {
    uint8_t tile;
    while (*s && x < 20) { tile = glyph(*s++); set_bkg_tiles(x++, y, 1, 1, &tile); }
}

static void blank(void) {
    uint8_t row[20], x, y;
    for (x = 0; x < 20; ++x) row[x] = 0;
    for (y = 0; y < 18; ++y) set_bkg_tiles(0, y, 20, 1, row);
    for (x = 0; x < 20; ++x) row[x] = 7;
    VBK_REG = 1;
    for (y = 0; y < 18; ++y) set_bkg_tiles(0, y, 20, 1, row);
    VBK_REG = 0;
}

static void message(const char *a, const char *b) {
    text(0, 16, "                    ");
    text(0, 17, "                    ");
    text(0, 16, a);
    text(0, 17, b);
}

static void title(void) {
    uint8_t x, y, row[20];
    game_mode = MODE_TITLE; blank();
    for (y = 0; y < 18; ++y) {
        for (x = 0; x < 20; ++x) row[x] = ((x * 19 + y * 13) % 41 == 0) ? 1 : 0;
        set_bkg_tiles(0, y, 20, 1, row);
    }
    text(3, 3, "COSMIC CHESS");
    text(5, 5, "ACADEMY");
    text(2, 7, "DOT: READY CADET?");
    text(2, 9, "ROOK / BISHOP /");
    text(2, 10, "KNIGHT MISSIONS");
    text(2, 12, "D PAD: CURSOR");
    text(2, 13, "A: MOVE  B: HINT");
    text(2, 16, "START TO LAUNCH");
    hide_sprite(0); hide_sprite(1); hide_sprite(2); hide_sprite(3);
}

static uint8_t has_friend(uint8_t x, uint8_t y) { return x == friend_x && y == friend_y; }
static uint8_t has_enemy(uint8_t x, uint8_t y) { return enemy_exists && x == enemy_x && y == enemy_y; }

static uint8_t path_clear(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    int8_t sx = x2 > x1 ? 1 : x2 < x1 ? -1 : 0;
    int8_t sy = y2 > y1 ? 1 : y2 < y1 ? -1 : 0;
    int8_t x = (int8_t)x1 + sx, y = (int8_t)y1 + sy;
    int8_t end_x = (int8_t)x2, end_y = (int8_t)y2;
    while (x != end_x || y != end_y) {
        if (has_friend(x, y) || has_enemy(x, y)) return 0;
        x += sx; y += sy;
    }
    return 1;
}

static uint8_t legal(uint8_t x, uint8_t y) {
    uint8_t dx = absdiff(px, x), dy = absdiff(py, y);
    uint8_t type = lessons[lesson_index].piece, capture = has_enemy(x, y);
    if ((x == px && y == py) || has_friend(x, y)) return 0;
    if (type == KNIGHT) return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
    if (type == PAWN) return (x == px && y + 1 == py && !capture) ||
                              (dx == 1 && y + 1 == py && capture);
    if (type == KING) return dx <= 1 && dy <= 1;
    if ((type == ROOK || type == QUEEN) && ((dx == 0) != (dy == 0)))
        return path_clear(px, py, x, y);
    if ((type == BISHOP || type == QUEEN) && dx == dy)
        return path_clear(px, py, x, y);
    return 0;
}

static void square_palette(uint8_t x, uint8_t y, uint8_t pal) {
    uint8_t attrs[4] = {pal, pal, pal, pal};
    VBK_REG = 1;
    set_bkg_tiles(x * 2, y * 2, 2, 2, attrs);
    VBK_REG = 0;
}

static void board(void) {
    uint8_t x, y, fill[4] = {2, 2, 2, 2}, pal;
    for (y = 0; y < 8; ++y) for (x = 0; x < 8; ++x) {
        set_bkg_tiles(x * 2, y * 2, 2, 2, fill);
        pal = (x + y) & 1;
        if (x == lessons[lesson_index].goal_x && y == lessons[lesson_index].goal_y) pal = 5;
        if (hints && legal(x, y)) pal = 3;
        if (selected && x == px && y == py) pal = 6;
        if (x == cx && y == cy) pal = 4;
        square_palette(x, y, pal);
    }
}

static void sprites(void) {
    const Lesson *l = &lessons[lesson_index];
    set_sprite_tile(0, l->piece);
    move_sprite(0, 16 * px + 12, 16 * py + 20);
    set_sprite_tile(1, l->friend_piece);
    move_sprite(1, 16 * friend_x + 12, 16 * friend_y + 20);
    if (enemy_exists) {
        set_sprite_tile(2, l->enemy_piece);
        move_sprite(2, 16 * enemy_x + 12, 16 * enemy_y + 20);
    } else hide_sprite(2);
    if (l->capture_goal && enemy_exists) hide_sprite(3);
    else { set_sprite_tile(3, BEACON); move_sprite(3, 16 * l->goal_x + 12, 16 * l->goal_y + 20); }
}

static void load_lesson(void) {
    const Lesson *l = &lessons[lesson_index];
    game_mode = MODE_LESSON; selected = 0; hints = 0; solved = 0;
    px = cx = l->x; py = cy = l->y;
    friend_x = l->friend_x; friend_y = l->friend_y;
    enemy_x = l->enemy_x; enemy_y = l->enemy_y; enemy_exists = l->enemy_exists;
    blank();
    text(16, 1, "DOT");
    text(16, 3, "ROOM");
    text(16, 4, lesson_index == 0 ? "1/3" : lesson_index == 1 ? "2/3" : "3/3");
    text(16, 7, "A GO");
    text(16, 9, "B ?");
    text(16, 12, "STAR");
    message(l->intro1, l->intro2);
    board(); sprites();
}

static void victory(void) {
    blank(); game_mode = MODE_VICTORY;
    hide_sprite(0); hide_sprite(1); hide_sprite(2); hide_sprite(3);
    text(2, 3, "CADET GRADUATED!");
    text(2, 6, "3 STARS CHARTED");
    text(2, 9, "DOT: NICE FLYING.");
    text(2, 12, "NEXT: REAL TACTICS");
    text(2, 15, "START TO REPLAY");
}

static void click_a(void) {
    const Lesson *l = &lessons[lesson_index];
    if (solved) return;
    if (!selected) {
        if (cx == px && cy == py) {
            selected = 1;
            message("SHIP SELECTED", "CHOOSE A SQUARE");
        } else message("SELECT YOUR PIECE", "THEN CHOOSE LANDING");
    } else if (cx == px && cy == py) {
        selected = 0;
        message("SHIP DESELECTED", "A: SELECT  B: HINT");
    } else if (!legal(cx, cy)) {
        if (has_friend(cx, cy)) message("FRIENDLY SHIP THERE", "CHOOSE ANOTHER TILE");
        else if (l->piece == ROOK && (cx == px || cy == py) && !path_clear(px, py, cx, cy))
            message("A SHIP BLOCKS PATH", "TRY AN OPEN RAIL");
        else if (l->piece == BISHOP && absdiff(cx, px) == absdiff(cy, py) &&
                 !path_clear(px, py, cx, cy))
            message("A SHIP BLOCKS PATH", "TRY AN OPEN RAY");
        else message(l->rule, "TRY A MINT SQUARE");
    } else {
        px = cx; py = cy; selected = 0;
        if (has_enemy(px, py)) { enemy_exists = 0; hide_sprite(2); }
        if (px == l->goal_x && py == l->goal_y && (!l->capture_goal || !enemy_exists)) {
            solved = 1;
            message("STAR SECURED!", "START: NEXT MISSION");
        } else message("LEGAL MOVE!", "FIND THE GOLD STAR");
        sprites();
    }
    board();
}

static void click_b(void) {
    if (solved) return;
    hints = !hints;
    if (hints) message("MINT: LEGAL MOVES", lessons[lesson_index].rule);
    else message(lessons[lesson_index].intro1, lessons[lesson_index].intro2);
    board();
}

void main(void) {
    uint8_t keys;
    DISPLAY_OFF;
    set_bkg_data(0, BG_TILE_COUNT, bg_tiles);
    set_sprite_data(0, PIECE_TILE_COUNT, piece_tiles);
    set_bkg_palette(0, 8, board_colors);
    set_sprite_palette(0, 3, ship_colors);
    set_sprite_prop(0, 0); set_sprite_prop(1, 0);
    set_sprite_prop(2, 1); set_sprite_prop(3, 2);
    SHOW_BKG; SHOW_SPRITES; SPRITES_8x8;
    title(); DISPLAY_ON;
    while (1) {
        wait_vbl_done();
        keys = joypad();
        if ((keys & J_START) && !(old_keys & J_START)) {
            if (game_mode == MODE_TITLE || game_mode == MODE_VICTORY) { lesson_index = 0; load_lesson(); }
            else if (solved) { if (++lesson_index == 3) victory(); else load_lesson(); }
            else load_lesson();
        } else if (game_mode == MODE_LESSON && !solved) {
            if ((keys & J_LEFT) && !(old_keys & J_LEFT) && cx) { --cx; board(); }
            if ((keys & J_RIGHT) && !(old_keys & J_RIGHT) && cx < 7) { ++cx; board(); }
            if ((keys & J_UP) && !(old_keys & J_UP) && cy) { --cy; board(); }
            if ((keys & J_DOWN) && !(old_keys & J_DOWN) && cy < 7) { ++cy; board(); }
            if ((keys & J_A) && !(old_keys & J_A)) click_a();
            if ((keys & J_B) && !(old_keys & J_B)) click_b();
        }
        old_keys = keys;
    }
}
