#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "art.h"

#define TITLE 0
#define PLAY 1
#define PAUSE 2
#define WIN 3
#define OVER 4
#define COINS 20
#define FOES 5
#define LENGTH 144
/* Coordinates are world pixels; vertical velocity uses quarter pixels. */
uint16_t player_x, camera_x, checkpoint;
int16_t player_y, velocity_y, y_fraction;
uint8_t game_state, lives, collected, grounded, invincible;
static uint8_t old_keys, frame, coin_live[COINS], foe_live[FOES];
static uint16_t foe_x[FOES];
static int8_t foe_dir[FOES];
static const uint16_t coin_x[COINS]={64,80,96,128,144,272,288,304,384,400,528,544,560,640,656,784,800,816,1024,1040};
static const uint8_t coin_y[COINS]={80,80,80,64,64,80,80,80,64,64,80,80,80,64,64,80,80,80,80,80};
static const palette_color_t bg_pal[]={RGB8(204,242,241),RGB8(31,56,71),RGB8(66,152,125),RGB8(155,217,139)};
static const palette_color_t hero_pal[]={RGB8(204,242,241),RGB8(34,42,60),RGB8(221,100,83),RGB8(255,224,164)};
static const palette_color_t foe_pal[]={RGB8(204,242,241),RGB8(34,42,60),RGB8(136,106,190),RGB8(247,238,210)};
static const palette_color_t coin_pal[]={RGB8(204,242,241),RGB8(109,68,32),RGB8(242,178,56),RGB8(255,236,137)};
static uint8_t glyph(char c){ uint8_t i; for(i=0;glyph_chars[i];i++)if(glyph_chars[i]==c)return i+6;return 0; }
static void text(uint8_t x,uint8_t y,const char*s,uint8_t window){uint8_t t;while(*s&&x<20){t=glyph(*s++);if(window)set_win_tiles(x++,y,1,1,&t);else set_bkg_tiles(x++,y,1,1,&t);}}
static void number(uint8_t x,uint8_t y,uint8_t n){uint8_t a[2];a[0]=glyph('0'+n/10);a[1]=glyph('0'+n%10);set_win_tiles(x,y,2,1,a);}
static void hide_sprites(void){uint8_t i;for(i=0;i<40;i++)move_sprite(i,0,0);}
static void clear(void){uint8_t row[32]={0},y;for(y=0;y<32;y++)set_bkg_tiles(0,y,32,1,row);move_bkg(0,0);HIDE_WIN;hide_sprites();}
/* Ground gaps and elevated ledges repeat through four handcrafted stretches. */
static uint8_t tile_at(uint16_t x,uint8_t y){uint8_t local=x%32;if(x>=LENGTH)return 1;if(y>=15){if(x<128&&local>=22&&local<=24)return 0;return 1;}if(y==12&&local>=7&&local<=12)return 2;if(y==10&&local>=15&&local<=19)return 2;if(x==139&&y>=6&&y<=15)return y==15?5:4;if(y==4&&(local==3||local==4||local==5))return 3;return 0;}
static uint8_t solid(int16_t x,int16_t y){uint8_t t;if(x<0)return 1;if(y<0)return 0;if(y>=144)return 0;t=tile_at((uint16_t)x/8,(uint8_t)y/8);return t==1||t==2;}
static void column(uint16_t x){uint8_t y,tiles[18];for(y=0;y<18;y++)tiles[y]=tile_at(x,y);set_bkg_tiles(x%32,0,1,18,tiles);}
static void draw_world(void){uint16_t i;for(i=camera_x/8;i<camera_x/8+32;i++)column(i);move_bkg((uint8_t)camera_x,0);}
static void hud(void){uint8_t row[20]={0};set_win_tiles(0,0,20,1,row);set_win_tiles(0,1,20,1,row);text(0,0,"COINS",1);number(6,0,collected);text(9,0,"LIVES",1);number(15,0,lives);text(0,1,game_state==PAUSE?"PAUSED - START":"A JUMP  B RUN",1);move_win(7,128);SHOW_WIN;}
static void sound(uint8_t pitch){NR10_REG=0;NR11_REG=0x80;NR12_REG=0x92;NR13_REG=pitch;NR14_REG=0x87;}
static void screen(uint8_t state){game_state=state;clear();text(3,3,"POCKET JUMPER",0);if(state==TITLE){text(2,6,"A LITTLE BIG QUEST",0);text(2,9,"LEFT RIGHT TO MOVE",0);text(2,10,"A JUMP  B TO RUN",0);text(2,12,"COINS AND CRITTERS",0);text(2,13,"REACH THE FLAG",0);}else{ text(4,7,state==WIN?"FLAG REACHED!":"TRY AGAIN!",0);text(3,9,"COINS",0);{uint8_t t=glyph('0'+collected/10);set_bkg_tiles(10,9,1,1,&t);t=glyph('0'+collected%10);set_bkg_tiles(11,9,1,1,&t);}}text(3,16,"PRESS START OR A",0);}
static void respawn(void){player_x=checkpoint;player_y=112;velocity_y=0;y_fraction=0;grounded=0;invincible=90;camera_x=player_x>72?player_x-72:0;draw_world();hud();}
static void start(void){uint8_t i;lives=3;collected=0;checkpoint=16;for(i=0;i<COINS;i++)coin_live[i]=1;for(i=0;i<FOES;i++){foe_live[i]=1;foe_x[i]=120+i*256;foe_dir[i]=1;}game_state=PLAY;clear();respawn();}
static void hurt(void){sound(40);if(--lives==0)screen(OVER);else respawn();}
static void render(void){uint8_t i;int16_t sx;hide_sprites();sx=(int16_t)player_x-camera_x;if(!invincible||(frame&4))move_sprite(0,sx+8,player_y+16);set_sprite_tile(0,grounded&&((frame>>3)&1)?1:0);for(i=0;i<COINS;i++){sx=(int16_t)coin_x[i]-camera_x;if(coin_live[i]&&sx>=-7&&sx<160)move_sprite(i+1,sx+8,coin_y[i]+16);}for(i=0;i<FOES;i++){sx=(int16_t)foe_x[i]-camera_x;if(foe_live[i]&&sx>=-7&&sx<160)move_sprite(i+21,sx+8,128);}}
static void update(uint8_t keys,uint8_t pressed){uint8_t speed=(keys&J_B)?2:1,i;int16_t nx,ny,previous_y;uint16_t next_camera,old_col; if(pressed&J_START){game_state=PAUSE;hud();return;}if(invincible)invincible--;if(keys&J_LEFT){nx=player_x-speed;if(nx>=0&&!solid(nx,player_y)&&!solid(nx,player_y+7))player_x=nx;set_sprite_prop(0,S_FLIPX);}if(keys&J_RIGHT){nx=player_x+speed;if(!solid(nx+7,player_y)&&!solid(nx+7,player_y+7))player_x=nx;set_sprite_prop(0,0);}if((pressed&J_A)&&grounded){velocity_y=-48;grounded=0;sound(180);}/* Releasing jump early makes a shorter hop. */if(!(keys&J_A)&&velocity_y<-16)velocity_y=-16;
previous_y=player_y;velocity_y+=3;if(velocity_y>28)velocity_y=28;y_fraction+=velocity_y;ny=player_y+y_fraction/4;y_fraction%=4;grounded=0;
if(velocity_y>=0){for(i=0;i<8&&player_y<ny;i++){if(solid(player_x,player_y+8)||solid(player_x+7,player_y+8)){grounded=1;velocity_y=0;y_fraction=0;break;}player_y++;}}else{for(i=0;i<16&&player_y>ny;i++){if(solid(player_x,player_y-1)||solid(player_x+7,player_y-1)){velocity_y=0;y_fraction=0;break;}player_y--;}}
if(velocity_y>=0&&(solid(player_x,player_y+8)||solid(player_x+7,player_y+8))){grounded=1;velocity_y=0;y_fraction=0;}
if(player_y>143){hurt();return;}for(i=0;i<COINS;i++){if(coin_live[i]&&player_x+7>=coin_x[i]&&player_x<coin_x[i]+8&&player_y+7>=coin_y[i]&&player_y<coin_y[i]+8){coin_live[i]=0;collected++;sound(220);hud();}}
for(i=0;i<FOES;i++){if(!foe_live[i])continue;if(!(frame&1)){nx=foe_x[i]+foe_dir[i];if(!solid(nx+4,120)||solid(nx+8,112)||nx<i*256+104||nx>i*256+160)foe_dir[i]=-foe_dir[i];else foe_x[i]=nx;}if(player_x+7>=foe_x[i]&&player_x<foe_x[i]+8&&player_y+7>=112&&player_y<120){if(velocity_y>0&&previous_y+7<=114){foe_live[i]=0;velocity_y=-32;sound(130);}else if(!invincible){hurt();return;}}}
if(player_x>=139*8){screen(WIN);sound(240);return;}if(player_x>checkpoint+256&&player_x%256<80)checkpoint=(player_x/256)*256+16;
next_camera=player_x>72?player_x-72:0;if(next_camera>LENGTH*8-160)next_camera=LENGTH*8-160;old_col=camera_x/8;if(next_camera/8!=old_col){if(next_camera>camera_x)column(next_camera/8+31);else column(next_camera/8);}camera_x=next_camera;move_bkg((uint8_t)camera_x,0);render();}
void main(void){uint8_t i,keys,pressed;DISPLAY_OFF;set_bkg_data(0,BG_TILE_COUNT,bg_tiles);set_sprite_data(0,4,sprite_tiles);BGP_REG=0xe4;OBP0_REG=0xe4;OBP1_REG=0xe4;if(_cpu==CGB_TYPE){set_bkg_palette(0,1,bg_pal);set_sprite_palette(0,1,hero_pal);set_sprite_palette(1,1,foe_pal);set_sprite_palette(2,1,coin_pal);}for(i=0;i<COINS;i++){set_sprite_tile(i+1,3);set_sprite_prop(i+1,2);}for(i=0;i<FOES;i++){set_sprite_tile(i+21,2);set_sprite_prop(i+21,1);}NR52_REG=0x80;NR50_REG=0x77;NR51_REG=0x11;SHOW_BKG;SHOW_SPRITES;screen(TITLE);DISPLAY_ON;while(1){wait_vbl_done();frame++;keys=joypad();pressed=keys&~old_keys;old_keys=keys;if(game_state==PLAY)update(keys,pressed);else if(game_state==PAUSE){if(pressed&J_START){game_state=PLAY;hud();}}else if(pressed&(J_START|J_A))start();}}
