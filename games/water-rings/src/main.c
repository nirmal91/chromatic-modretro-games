#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "art.h"

/* Positions and velocities use signed 12.4 fixed point (sixteenths of a pixel). */
typedef struct { int16_t x, y, vx, vy; uint8_t caught; } Ring;
static Ring rings[8];
static uint8_t stacks[2], score, old_keys, state, frame;
static uint16_t elapsed, best;
static const uint8_t pegs[] = {55,103};
static const palette_color_t tank_colors[] = {RGB8(8,44,62),RGB8(68,150,171),RGB8(16,80,105),RGB8(179,235,219)};
static const palette_color_t ring_colors[] = {RGB8(8,44,62),RGB8(94,43,40),RGB8(255,163,71),RGB8(255,231,124)};
static const palette_color_t blue_colors[] = {RGB8(8,44,62),RGB8(40,50,106),RGB8(154,132,240),RGB8(226,218,255)};
static const palette_color_t bubble_colors[] = {RGB8(8,44,62),RGB8(38,112,141),RGB8(138,228,244),RGB8(255,255,255)};

static uint8_t glyph(char c) {
    uint8_t i;
    for(i=0; letters[i]; ++i) if(letters[i]==c) return i+4;
    return 0;
}
static void text(uint8_t x,uint8_t y,const char *s) {
    uint8_t tile;
    while(*s && x<20) { tile=glyph(*s++); set_bkg_tiles(x++,y,1,1,&tile); }
}
static void number(uint8_t x,uint8_t y,uint16_t value,uint8_t width) {
    uint8_t buf[4],i;
    for(i=0;i<width;++i) { buf[width-i-1]=glyph('0'+value%10); value/=10; }
    set_bkg_tiles(x,y,width,1,buf);
}
static void tile(uint8_t x,uint8_t y,uint8_t t) {set_bkg_tiles(x,y,1,1,&t);}
static void clear(void) {
    uint8_t row[20]={0},y;
    for(y=0;y<18;++y) set_bkg_tiles(0,y,20,1,row);
}
static void hide_all(void) { uint8_t i; for(i=0;i<20;++i) hide_sprite(i); }
static void chime(void) { NR21_REG=0x80; NR22_REG=0x93; NR23_REG=0xB0; NR24_REG=0x87; }
static void tank(void) {
    uint8_t x,y;
    clear();
    text(4,0,"WATER RINGS");
    text(1,1,"RINGS"); number(7,1,score,1); text(8,1,"/8"); text(12,1,"SEC"); number(16,1,elapsed/60,3);
    for(x=1;x<19;++x) { tile(x,3,2); tile(x,15,DECOR+3); }
    for(y=3;y<16;++y) { tile(1,y,1); tile(18,y,1); }
    for(y=9;y<15;++y) {tile(6,y,DECOR);tile(12,y,DECOR);}
    tile(6,8,3);tile(12,8,3);tile(6,15,DECOR+1);tile(12,15,DECOR+1);
    tile(2,13,DECOR+2);tile(2,14,DECOR+2);tile(17,14,DECOR+2);
    tile(4,15,DECOR+4);tile(14,15,DECOR+4);
    text(1,16,"A LEFT  B RIGHT JET"); text(2,17,"D PAD TILT  START");
}
static void title(void) {
    hide_all();clear();state=0;
    text(4,2,"WATER RINGS");text(2,4,"A POCKET WATER TOY");
    text(2,7,"A / B : WATER JETS");text(2,9,"D PAD : TILT TANK");
    text(2,11,"LAND ALL 8 RINGS");text(2,12,"ON THE TWO PEGS");
    text(2,15,"START TO SPLASH");
    move_sprite(0,56,62);move_sprite(1,112,62);
}
static void start_game(void) {
    uint8_t i;
    score=0;elapsed=0;frame=0;stacks[0]=stacks[1]=0;state=1;
    for(i=0;i<8;++i) {
        rings[i].x=(24+i*15)*16; rings[i].y=(110-(i%3)*4)*16;
        rings[i].vx=0;rings[i].vy=0;rings[i].caught=0;
    }
    hide_all();tank();
}
static int16_t distance(int16_t a,int16_t b) { return a>b?a-b:b-a; }
static void jet(Ring *r,int16_t nozzle) {
    int16_t d=distance(r->x/16,nozzle);
    if(d<62) {
        /* A spreading stream rises toward its peg; tilt changes its drift. */
        r->vy-= (62-d)/9+2;
        if(r->x/16 < nozzle) r->vx+=2+d/18;
        else if(r->x/16 > nozzle) r->vx-=2+d/18;
    }
}
static void physics(uint8_t keys) {
    uint8_t i,p; int16_t previous; Ring *r;
    for(i=0;i<8;++i) {
        r=&rings[i];if(r->caught) continue;
        previous=r->y;
        r->vy+=2; /* net sinking force after buoyancy */
        if(keys&J_A) jet(r,55);
        if(keys&J_B) jet(r,103);
        if(keys&J_LEFT) r->vx-=2;
        if(keys&J_RIGHT) r->vx+=2;
        if(keys&J_UP) --r->vy;
        if(keys&J_DOWN) ++r->vy;
        /* Water drag, including slow movement, without a floating-point library. */
        if(r->vx>0) r->vx-=1+r->vx/24; else if(r->vx<0) r->vx+=1+(-r->vx)/24;
        if(r->vy>0) r->vy-=r->vy/28; else if(r->vy<0) r->vy+=(-r->vy)/28;
        if(r->vy>40)r->vy=40;if(r->vy< -56)r->vy=-56;
        if(r->vx>32)r->vx=32;if(r->vx< -32)r->vx=-32;
        r->x+=r->vx;r->y+=r->vy;
        if(r->x<20*16){r->x=20*16;r->vx=-r->vx/2;}
        if(r->x>140*16){r->x=140*16;r->vx=-r->vx/2;}
        if(r->y<32*16){r->y=32*16;r->vy=8;}
        if(r->y>115*16){r->y=115*16;r->vy=0;}
        for(p=0;p<2;++p) {
            if(stacks[p]<4 && r->vy>0 && previous<68*16 && r->y>=68*16 && distance(r->x/16,pegs[p])<=6) {
                r->caught=p+1;r->x=pegs[p]*16;r->y=(105-stacks[p]*8)*16;
                ++stacks[p];++score;number(7,1,score,1);chime();break;
            }
        }
    }
}
static void draw(uint8_t keys) {
    uint8_t i,y;
    for(i=0;i<8;++i) move_sprite(i, rings[i].x/16+4,rings[i].y/16+12);
    for(i=0;i<6;++i) {
        y=114-((frame*2+i*15)%79);
        if(keys&(i<3?J_A:J_B)) move_sprite(8+i,(i<3?52:100)+(i%3)*4,y+16);
        else hide_sprite(8+i);
    }
}
void main(void) {
    uint8_t i,keys,pressed;
    DISPLAY_OFF;set_bkg_data(0,BG_COUNT,bg_data);set_sprite_data(0,2,spr_data);
    BGP_REG=0xE4;OBP0_REG=0xE4;OBP1_REG=0xE4;
    if(_cpu==CGB_TYPE) {
        set_bkg_palette(0,1,tank_colors);set_sprite_palette(0,1,ring_colors);
        set_sprite_palette(1,1,blue_colors);set_sprite_palette(2,1,bubble_colors);
    }
    for(i=0;i<8;++i){set_sprite_tile(i,0);set_sprite_prop(i,(i&1)?1:0);}
    for(i=8;i<14;++i){set_sprite_tile(i,1);set_sprite_prop(i,2);}
    NR52_REG=0x80;NR50_REG=0x77;NR51_REG=0x22;
    SHOW_BKG;SHOW_SPRITES;DISPLAY_ON;title();
    for(;;) {
        wait_vbl_done(); keys=joypad();pressed=keys&~old_keys;old_keys=keys;
        if(state==0) {if(pressed&(J_START|J_A))start_game();continue;}
        if(pressed&J_SELECT) {start_game();continue;}
        if(state==3) {if(pressed&(J_START|J_A))start_game();continue;}
        if(pressed&J_START) {
            state=state==1?2:1;
            if(state==2)text(6,6,"PAUSED");else text(6,6,"      ");
        }
        if(state==2)continue;
        ++frame;if(elapsed<59940)++elapsed;
        if(!(frame%30)) number(16,1,elapsed/60,3);
        physics(keys);draw(keys);
        if(score==8) {
            state=3;if(best==0 || elapsed<best)best=elapsed;
            text(4,5,"ALL 8 LANDED");text(3,6,"A PERFECT SPLASH");
            text(4,11,"BEST SEC");number(13,11,best/60,3);
            text(2,13,"START PLAY AGAIN");
        }
    }
}
