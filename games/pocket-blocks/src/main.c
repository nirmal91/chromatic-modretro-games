#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "art.h"
#define WIDTH 10
#define HEIGHT 16
/* Each bit describes a cell of a 4x4 shape, left-to-right, top-to-bottom. */
const uint16_t shapes[7][4] = {
 {0x00f0,0x4444,0x0f00,0x2222},
 {0x0066,0x0066,0x0066,0x0066},
 {0x0072,0x0262,0x0270,0x0232},
 {0x0071,0x0226,0x0470,0x0322},
 {0x0074,0x0622,0x0170,0x0223},
 {0x0036,0x0462,0x0360,0x0231},
 {0x0063,0x0264,0x0630,0x0132}
};
const palette_color_t colors[] = { RGB8(13,21,33),RGB8(118,223,199),RGB8(47,124,153),RGB8(236,246,220) };
uint8_t board[HEIGHT][WIDTH];
uint8_t state, paused, type, rotation, next_piece, bag[7], bag_pos=7;
int8_t piece_x,piece_y;
uint16_t score, lines;
uint8_t level, old_keys, gravity, repeat_timer, rng=93;
static uint8_t glyph(char c) { if(c>='A' && c<='Z') return 16+c-'A'; if(c>='0' && c<='9') return 42+c-'0'; if(c==':')return 52;if(c=='!')return 53;if(c=='+')return 54;if(c=='-')return 55;return 0; }
static void text(uint8_t x,uint8_t y,const char *s) { uint8_t t; while(*s && x<20) {t=glyph(*s++);set_bkg_tiles(x++,y,1,1,&t);} }
static void number(uint8_t x,uint8_t y,uint16_t n,uint8_t digits) {uint8_t a[5],i;for(i=digits;i;--i){a[i-1]=glyph('0'+n%10);n/=10;}set_bkg_tiles(x,y,digits,1,a);}
static void clear(void) { uint8_t a[20]={0},y; for(y=0;y<18;++y)set_bkg_tiles(0,y,20,1,a); }
static void beep(uint8_t pitch) { NR21_REG=0x80;NR22_REG=0x82;NR23_REG=pitch;NR24_REG=0x87; }
static uint8_t random8(void) { rng^=rng<<3;rng^=rng>>5;rng^=rng<<1;return rng; }
static uint8_t pick(void) { uint8_t i,j,t; if(bag_pos>=7){for(i=0;i<7;++i)bag[i]=i;for(i=6;i>0;--i){j=random8()%(i+1);t=bag[i];bag[i]=bag[j];bag[j]=t;}bag_pos=0;}return bag[bag_pos++]; }
static const uint16_t cell_bits[]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
static uint8_t occupied(uint8_t t,uint8_t r,uint8_t x,uint8_t y) {return (shapes[t][r] & cell_bits[y*4+x])!=0;}
static uint8_t fits(int8_t px,int8_t py,uint8_t r) {uint8_t x,y;int8_t bx,by;for(y=0;y<4;++y)for(x=0;x<4;++x)if(occupied(type,r,x,y)){bx=px+x;by=py+y;if(bx<0||bx>=WIDTH||by>=HEIGHT)return 0;if(by>=0 && board[by][bx])return 0;}return 1;}
static void hud(void) {text(13,1,"NEXT");text(13,7,"SCORE");number(13,8,score,5);text(13,10,"LINES");number(13,11,lines,4);text(13,13,"LEVEL");number(13,14,level,2);text(13,16,paused?"PAUSE":"     ");}
static void draw(void) {uint8_t x,y,t,row[10];int8_t ghost=piece_y;for(y=0;y<HEIGHT;++y){for(x=0;x<WIDTH;++x)row[x]=board[y][x]?board[y][x]+2:0;set_bkg_tiles(1,y+1,WIDTH,1,row);}if(state==1){while(fits(piece_x,ghost+1,rotation))++ghost;for(y=0;y<4;++y)for(x=0;x<4;++x)if(occupied(type,rotation,x,y)){if(ghost+y>=0){t=2;set_bkg_tiles(1+piece_x+x,1+ghost+y,1,1,&t);}if(piece_y+y>=0){t=type+3;set_bkg_tiles(1+piece_x+x,1+piece_y+y,1,1,&t);}}}for(y=0;y<4;++y)for(x=0;x<4;++x){t=occupied(next_piece,0,x,y)?next_piece+3:0;set_bkg_tiles(13+x,3+y,1,1,&t);}hud();}
static void game_over(void){state=2;paused=0;draw();text(1,6,"STACK FULL");text(2,8,"START TO");text(3,9,"RETRY");beep(0x35);}
static void spawn(void){type=next_piece;next_piece=pick();piece_x=3;piece_y=0;rotation=0;gravity=0;if(!fits(piece_x,piece_y,rotation))game_over();}
static void lock(void){uint8_t x,y,full,count=0;int8_t row,k;static const uint16_t rewards[]={0,100,300,500,800};for(y=0;y<4;++y)for(x=0;x<4;++x)if(occupied(type,rotation,x,y)){if(piece_y+y<0){game_over();return;}board[piece_y+y][piece_x+x]=type+1;}for(row=HEIGHT-1;row>=0;--row){full=1;for(x=0;x<WIDTH;++x)if(!board[row][x])full=0;if(full){++count;for(k=row;k>0;--k)for(x=0;x<WIDTH;++x)board[k][x]=board[k-1][x];for(x=0;x<WIDTH;++x)board[0][x]=0;++row;}}if(count){score+=rewards[count]*level;lines+=count;level=1+lines/10;if(level>20)level=20;beep(0xe8);}else beep(0x90);spawn();if(state==1)draw();}
static void drop(void){if(fits(piece_x,piece_y+1,rotation)){++piece_y;draw();}else lock();}
static void rotate(int8_t dir){uint8_t r=(rotation+dir)&3;int8_t offsets[5]={0,-1,1,-2,2};uint8_t i;for(i=0;i<5;++i)if(fits(piece_x+offsets[i],piece_y,r)){piece_x+=offsets[i];rotation=r;beep(0xc0);draw();return;}}
static void start(void){uint8_t x,y,t=1;clear();for(y=0;y<HEIGHT;++y){for(x=0;x<WIDTH;++x)board[y][x]=0;set_bkg_tiles(0,y+1,1,1,&t);set_bkg_tiles(11,y+1,1,1,&t);}for(x=0;x<12;++x)set_bkg_tiles(x,17,1,1,&t);text(0,0,"POCKET BLOCKS");score=0;lines=0;level=1;paused=0;state=1;bag_pos=7;rng+=DIV_REG;next_piece=pick();spawn();repeat_timer=0;draw();}
static void title(void){clear();text(3,2,"POCKET BLOCKS");text(3,4,"MAKE ROOM!");text(1,7,"LEFT RIGHT  MOVE");text(1,8,"DOWN        SOFT");text(1,9,"A B         TURN");text(1,10,"UP          DROP");text(1,11,"START       PAUSE");text(2,14,"START TO STACK");text(2,16,"7 SHAPES  1 WELL");state=0;}
void main(void){uint8_t keys,pressed,delay;DISPLAY_OFF;set_bkg_data(0,TILE_COUNT,art);BGP_REG=0xe4;if(_cpu==CGB_TYPE)set_bkg_palette(0,1,colors);NR52_REG=0x80;NR50_REG=0x77;NR51_REG=0xff;SHOW_BKG;title();DISPLAY_ON;while(1){wait_vbl_done();keys=joypad();pressed=keys&~old_keys;if(state!=1){if(pressed&J_START)start();old_keys=keys;continue;}if(pressed&J_START){paused=!paused;hud();}if(!paused){if(pressed&J_A)rotate(1);if(pressed&J_B)rotate(-1);if(pressed&J_UP){while(fits(piece_x,piece_y+1,rotation)){++piece_y;score+=2;}lock();}else{if(!(keys&(J_LEFT|J_RIGHT)))repeat_timer=0;else if((pressed&(J_LEFT|J_RIGHT))||repeat_timer==0){int8_t dx=(keys&J_LEFT)?-1:1;if(fits(piece_x+dx,piece_y,rotation)){piece_x+=dx;draw();}repeat_timer=(pressed&(J_LEFT|J_RIGHT))?12:4;}else --repeat_timer;delay=level<15?48-(level-1)*3:6;if(keys&J_DOWN)delay=3;if(++gravity>=delay){gravity=0;if((keys&J_DOWN)&&fits(piece_x,piece_y+1,rotation))++score;drop();}}}old_keys=keys;}}
