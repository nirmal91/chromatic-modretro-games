#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "art.h"

#define WIDTH 18
#define HEIGHT 13
#define CELLS 234
#define TITLE 0
#define PLAY 1
#define PAUSED 2
#define OVER 3
#define WON 4

static const palette_color_t palette[] = {
    RGB8(232,242,208), RGB8(172,205,126), RGB8(77,142,86), RGB8(25,56,44)
};
static uint8_t xs[CELLS], ys[CELLS];
static uint8_t length, direction, queued, food_x, food_y, timer, speed;
static uint8_t state, old_keys, rng = 83;
static uint16_t score, best;

static uint8_t glyph(char c) {
    uint8_t i;
    for (i = 0; glyph_chars[i]; i++) if (glyph_chars[i] == c) return i + 5;
    return 0;
}
static void text(uint8_t x, uint8_t y, const char *s) {
    uint8_t tile;
    while (*s && x < 20) { tile = glyph(*s++); set_bkg_tiles(x++,y,1,1,&tile); }
}
static void number(uint8_t x, uint8_t y, uint16_t n) {
    uint8_t digits[4], i;
    for (i = 0; i < 4; i++) { digits[3-i] = glyph('0' + n % 10); n /= 10; }
    set_bkg_tiles(x,y,4,1,digits);
}
static void cell(uint8_t x, uint8_t y, uint8_t tile) {
    set_bkg_tiles(x+1,y+3,1,1,&tile);
}
static void clear(void) {
    uint8_t row[20], i;
    for (i=0;i<20;i++) row[i]=0;
    for (i=0;i<18;i++) set_bkg_tiles(0,i,20,1,row);
}
static void tone(uint8_t low, uint8_t envelope) {
    NR21_REG=0x80; NR22_REG=envelope; NR23_REG=low; NR24_REG=0x87;
}
static uint8_t occupied(uint8_t x, uint8_t y, uint8_t count) {
    uint8_t i;
    for(i=0;i<count;i++) if(xs[i]==x && ys[i]==y) return 1;
    return 0;
}
static void food(void) {
    uint16_t index;
    /* Scan from a random offset: bounded even when only one empty cell remains. */
    rng ^= rng << 3; rng ^= rng >> 5; rng ^= rng << 1;
    index = rng % CELLS;
    while(occupied(index % WIDTH,index / WIDTH,length)) { index++; if(index==CELLS) index=0; }
    food_x=index % WIDTH; food_y=index / WIDTH;
    cell(food_x,food_y,3);
}
static void hud(void) {
    text(0,0,"SCORE"); number(5,0,score);
    text(11,0,"BEST"); number(16,0,best);
    text(1,17,"START PAUSE  A GO");
}
static void board(void) {
    uint8_t x,y,tile=1,i;
    clear(); hud();
    for(x=0;x<20;x++) {set_bkg_tiles(x,2,1,1,&tile);set_bkg_tiles(x,16,1,1,&tile);}
    for(y=3;y<16;y++) {set_bkg_tiles(0,y,1,1,&tile);set_bkg_tiles(19,y,1,1,&tile);}
    for(i=1;i<length;i++) cell(xs[i],ys[i],2);
    cell(xs[0],ys[0],4); cell(food_x,food_y,3);
}
static void start(void) {
    uint8_t i;
    length=4; direction=queued=0; timer=0; speed=12; score=0; state=PLAY;
    for(i=0;i<length;i++) {xs[i]=5-i;ys[i]=6;}
    food(); board();
}
static void finish(uint8_t result) {
    state=result; if(score>best) best=score; hud();
    text(3,7,"              "); text(3,8,result==WON ? "GARDEN CLEARED!" : "  GAME OVER   ");
    text(3,9,"              "); text(3,10," A / START GO ");
    tone(50,0xA3);
}
static void move(void) {
    uint8_t x=xs[0],y=ys[0],eat,i;
    direction=queued;
    if(direction==0) x++; else if(direction==1) y++; else if(direction==2) x--; else y--;
    if(x>=WIDTH || y>=HEIGHT) {finish(OVER);return;}
    eat=(x==food_x && y==food_y);
    /* On a non-growth step the tail vacates its cell, so it is legal to enter it. */
    if(occupied(x,y,length-(eat ? 0 : 1))) {finish(OVER);return;}
    if(!eat) cell(xs[length-1],ys[length-1],0);
    else {length++; score+=10; if(score>best) best=score; speed=12-(score/50 > 8 ? 8 : score/50);tone(200,0x93);}
    for(i=length-1;i>0;i--) {xs[i]=xs[i-1];ys[i]=ys[i-1];}
    xs[0]=x;ys[0]=y;cell(xs[1],ys[1],2);cell(x,y,4);
    if(eat) {hud();if(length==CELLS) finish(WON);else food();}
}
void main(void) {
    uint8_t keys,pressed,wanted;
    DISPLAY_OFF;
    set_bkg_data(0,TILE_COUNT,tiles);
    BGP_REG=0xE4;
    if(_cpu==CGB_TYPE) set_bkg_palette(0,1,palette);
    NR52_REG=0x80;NR50_REG=0x77;NR51_REG=0x22;
    clear(); text(4,3,"POCKET SNAKE");text(3,6,"GROW YOUR GARDEN");
    text(3,9,"D-PAD TO STEER");text(3,11,"EAT THE FRUIT");text(3,12,"AVOID THE WALLS");
    text(3,15,"PRESS A / START");
    SHOW_BKG; DISPLAY_ON;
    while(1) {
        wait_vbl_done();keys=joypad();pressed=keys & ~old_keys;old_keys=keys;
        if(state==TITLE || state==OVER || state==WON) {
            rng+=DIV_REG;
            if(pressed & (J_A|J_START)) start();
            continue;
        }
        if(pressed & J_START) {
            if(state==PAUSED) {state=PLAY;board();}
            else {state=PAUSED;text(5,8,"  PAUSED  ");text(4,10,"START RESUME");}
            continue;
        }
        if(state==PAUSED) {if(pressed & J_SELECT) start();continue;}
        wanted=direction;
        if(keys & J_RIGHT) wanted=0;else if(keys & J_DOWN) wanted=1;
        else if(keys & J_LEFT) wanted=2;else if(keys & J_UP) wanted=3;
        if((wanted ^ 2)!=direction) queued=wanted;
        if(++timer>=speed) {timer=0;move();}
    }
}
