#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "../../meteor-mayhem/src/art.h"
#include "art_extra.h"

#ifndef GAME_ID
#error Define GAME_ID from 1 to 5
#endif

static const palette_color_t bg_pal[] = { RGB8(5,10,26), RGB8(68,105,143), RGB8(96,203,213), RGB8(247,242,205) };
static const palette_color_t cyan[] = { RGB8(5,10,26), RGB8(38,140,172), RGB8(77,230,214), RGB8(252,250,227) };
static const palette_color_t red[] = { RGB8(5,10,26), RGB8(118,39,91), RGB8(234,84,105), RGB8(255,207,133) };
static const palette_color_t gold[] = { RGB8(5,10,26), RGB8(110,67,43), RGB8(247,183,76), RGB8(255,249,195) };
static uint8_t frame, keys, old_keys, game_state, hp, level, rng=83;
static uint16_t score, best;
static uint8_t rand8(void) { rng ^= rng << 3; rng ^= rng >> 5; rng ^= rng << 1; rng += DIV_REG; return rng; }
static uint8_t glyph(char c) { uint8_t i; if (c==' ') return 0; for(i=0;glyph_chars[i];++i) if(glyph_chars[i]==c) return i+4; return 0; }
static void text(uint8_t x,uint8_t y,const char*s) { uint8_t i=0,t; while(s[i] && x+i<20) { t=glyph(s[i]); set_bkg_tiles(x+i,y,1,1,&t); ++i; } }
static void number(uint8_t x,uint8_t y,uint16_t n,uint8_t width) { uint8_t b[5],i; if(n>9999)n=9999; for(i=0;i<width;++i){b[width-1-i]=glyph('0'+n%10);n/=10;}set_bkg_tiles(x,y,width,1,b); }
static void clear(void) { uint8_t row[20],x,y; for(x=0;x<20;++x)row[x]=0;for(y=0;y<18;++y)set_bkg_tiles(0,y,20,1,row);for(x=0;x<24;++x)hide_sprite(x); }
static void tile(uint8_t x,uint8_t y,uint8_t t){set_bkg_tiles(x,y,1,1,&t);}
static void spr(uint8_t i,uint8_t type,uint8_t palette,uint8_t x,uint8_t y){set_sprite_tile(i,type);set_sprite_prop(i,palette);move_sprite(i,x+8,y+16);}
static uint8_t hit(uint8_t ax,uint8_t ay,uint8_t bx,uint8_t by,uint8_t gap){return (ax>bx?ax-bx:bx-ax)<gap && (ay>by?ay-by:by-ay)<gap;}
static void beep(uint8_t pitch){NR21_REG=0x41;NR22_REG=0x82;NR23_REG=pitch;NR24_REG=0x87;}
static void hud(void){text(0,0,"SCORE");number(6,0,score,4);text(12,0,"HP");number(15,0,hp,1);text(0,17,"ROUND");number(6,17,level,1);}
static void finish(uint8_t won){game_state=won?3:2;if(score>best)best=score;clear();text(5,4,won?"MISSION CLEAR":"GAME OVER");text(4,7,"SCORE");number(10,7,score,4);text(4,9,"BEST");number(10,9,best,4);text(2,14,"START TO REPLAY");}

#if GAME_ID==1
#define TITLE "CARGO CRUSH"
#define SUB "SPACE DEPOT SIEGE"
#define HELP1 "D PAD MOVE / PUSH"
#define HELP2 "A PULSE   B BRAKE"
#define HELP3 "CRUSH ALL DRONES"
static uint8_t px,py,pulse,brake,blocks[8][16],foex[5],foey[5],foelive[5],foes,remaining,steps;
static void layout(void){uint8_t x,y,i,row[20];clear();hud();for(y=2;y<16;++y){for(x=0;x<20;++x)row[x]=((x+y*3)%37==0)?1:0;set_bkg_tiles(0,y,20,1,row);}for(y=0;y<8;++y)for(x=0;x<16;++x)blocks[y][x]=0;for(i=0;i<8;++i){x=2+(i*7)%16;y=1+(i*5)%8;blocks[y][x]=1;tile(x+2,y+4,49);}px=2;py=4;foes=2+level;if(foes>5)foes=5;remaining=foes;for(i=0;i<5;++i){foelive[i]=i<foes;foex[i]=12+i%3;foey[i]=1+i;}}
static void draw(void){uint8_t i;spr(0,10,0,px*8+16,py*8+32);for(i=0;i<5;++i)if(foelive[i])spr(i+1,11,1,foex[i]*8+16,foey[i]*8+32);else hide_sprite(i+1);text(11,17,"LEFT");number(16,17,remaining,1);}
static void start(void){level=1;hp=3;score=0;pulse=0;brake=0;steps=0;layout();game_state=1;}
static void tick(void){int8_t dx=0,dy=0;uint8_t nx,ny,bx,by,i;
 if(frame%7==0){if(keys&J_LEFT)dx=-1;else if(keys&J_RIGHT)dx=1;else if(keys&J_UP)dy=-1;else if(keys&J_DOWN)dy=1;
 if(dx||dy){nx=px+dx;ny=py+dy;if(nx<16&&ny<8){if(blocks[ny][nx]){bx=nx+dx;by=ny+dy;if(bx<16&&by<8&&!blocks[by][bx]){blocks[ny][nx]=0;blocks[by][bx]=1;tile(nx+2,ny+4,0);tile(bx+2,by+4,49);for(i=0;i<5;++i)if(foelive[i]&&foex[i]==bx&&foey[i]==by){foelive[i]=0;remaining--;score+=25;beep(165);}px=nx;py=ny;}}else{px=nx;py=ny;}}}
 if(!(keys&J_B)||brake==0){steps++;if(steps%(7-level)==0){for(i=0;i<5;++i)if(foelive[i]){nx=foex[i];ny=foey[i];if((rand8()&1)&&nx!=px)nx+=(nx<px?1:-1);else if(ny!=py)ny+=(ny<py?1:-1);if(nx<16&&ny<8&&!blocks[ny][nx]){foex[i]=nx;foey[i]=ny;}if(foex[i]==px&&foey[i]==py){if(hp)hp--;beep(41);px=1;py=1;}}}}}
 draw();
 if((keys&J_A)&&!(old_keys&J_A)&&pulse==0){pulse=60;for(i=0;i<5;++i)if(foelive[i]&&hit(px,py,foex[i],foey[i],3)){foelive[i]=0;remaining--;score+=10;}beep(105);}if(pulse)pulse--;if(brake)brake--;else if((keys&J_B)&&!(old_keys&J_B))brake=90;
 if(!remaining){score+=100;if(level==3)finish(1);else{level++;layout();beep(220);}}if(!hp)finish(0);
}
#elif GAME_ID==2
#define TITLE "STORM RIDERS"
#define SUB "WINGS ABOVE NEBULA"
#define HELP1 "LEFT RIGHT STEER"
#define HELP2 "A FLAP   B DASH"
#define HELP3 "DIVE ON THEIR WINGS"
static int16_t px,py,vy;static uint8_t ex[5],ey[5],alive[5],kills,needed,flap,dash,inv;
static void arena(void){uint8_t x,y;clear();for(y=2;y<16;++y)for(x=0;x<20;++x)if((x*13+y*17)%31==0)tile(x,y,1);for(x=0;x<20;++x){tile(x,1,3);tile(x,16,3);}hud();text(10,17,"TARGET");number(17,17,needed,1);}
static void wave(void){uint8_t i;needed=2+level;if(needed>5)needed=5;for(i=0;i<5;++i){alive[i]=i<needed;ex[i]=20+i*23;ey[i]=42+i*17;}px=76;py=92;vy=0;inv=45;arena();}
static void start(void){score=0;hp=3;level=1;kills=0;dash=0;flap=0;wave();game_state=1;}
static void tick(void){uint8_t i;if(keys&J_LEFT)px-=2;if(keys&J_RIGHT)px+=2;if(px<0)px=152;if(px>152)px=0;if((keys&J_A)&&flap==0){vy=-5;flap=10;beep(175);}if(flap)flap--;if((keys&J_B)&&!(old_keys&J_B)&&!dash){vy=-7;dash=80;beep(210);}if(dash)dash--;if(vy<4)vy++;py+=vy;if(py<20){py=20;vy=0;}if(py>120){py=120;vy=-2;}if(inv)inv--;
 spr(0,12,0,px,py);for(i=0;i<5;++i)if(alive[i]){if(frame%(3+i)==0){ex[i]+=(ex[i]<px?1:-1);if(ey[i]>py+8)ey[i]--;else if(ey[i]<py+8)ey[i]++;}spr(i+1,13,1,ex[i],ey[i]);if(hit(px,py,ex[i],ey[i],9)){if(py+3<ey[i]||vy>1){alive[i]=0;kills++;score+=20+(level*5);vy=-4;beep(200);}else if(!inv){hp--;inv=65;vy=-5;beep(50);}}}else hide_sprite(i+1);
 text(0,17,"DOWN");number(5,17,kills,1);if(kills>=needed){score+=50;if(level==3)finish(1);else{level++;kills=0;wave();}}if(!hp)finish(0);}
#elif GAME_ID==3
#define TITLE "PARCEL PANIC"
#define SUB "GALACTIC EXPRESS"
#define HELP1 "UP DOWN PICK LANE"
#define HELP2 "A SEND   B CATCH"
#define HELP3 "SERVE BEFORE ARRIVAL"
static uint8_t lane,customers[4],parcel[4],returned[4],served,misses,timer,spawn;
static void arena(void){uint8_t y,x;clear();hud();for(y=0;y<4;++y){uint8_t row=3+y*3;for(x=1;x<20;++x)tile(x,row+1,50);text(0,row,"|");}text(0,17,"SERVED");number(7,17,served,2);}
static void start(void){uint8_t i;score=0;hp=3;level=1;lane=0;served=0;misses=0;timer=0;spawn=0;for(i=0;i<4;++i){customers[i]=i==0?15:0;parcel[i]=0;returned[i]=0;}arena();game_state=1;}
static void tick(void){uint8_t i,y;if((keys&J_UP)&&!(old_keys&J_UP)&&lane)lane--;if((keys&J_DOWN)&&!(old_keys&J_DOWN)&&lane<3)lane++;
 if((keys&J_A)&&!(old_keys&J_A)&&!parcel[lane]&&!returned[lane]){parcel[lane]=2;beep(150);}if((keys&J_B)&&!(old_keys&J_B)&&returned[lane]&&returned[lane]<5){returned[lane]=0;score+=10;beep(205);}
 if(frame%7==0){for(i=0;i<4;++i){if(parcel[i]){parcel[i]+=2;if(customers[i]&&parcel[i]>=customers[i]&&customers[i]<20){parcel[i]=0;returned[i]=customers[i];customers[i]=0;served++;score+=20;beep(220);}else if(parcel[i]>19)parcel[i]=0;}if(returned[i]){returned[i]--;if(returned[i]==0){hp--;beep(40);}}if(customers[i]&&customers[i]<20){customers[i]--;if(customers[i]<2){hp--;customers[i]=0;beep(40);}}}
 if(++spawn>=15-level*2){spawn=0;for(i=0;i<4;++i)if(!customers[i]&&!returned[i]){customers[i]=19;break;}}}
 for(i=0;i<4;++i){y=3+i*3;spr(i,15,1,customers[i]?customers[i]*8:168,y*8);spr(i+4,14,2,parcel[i]?parcel[i]*8:168,y*8);spr(i+8,14,0,returned[i]?returned[i]*8:168,y*8);}spr(12,10,0,0,(3+lane*3)*8);
 number(7,17,served,2);if(served>=10*level){if(level==3)finish(1);else{level++;score+=100;served=0;arena();}}if(!hp)finish(0);}
#elif GAME_ID==4
#define TITLE "LOST DRONES"
#define SUB "RESCUE THE STARS"
#define HELP1 "D PAD NAVIGATE"
#define HELP2 "A DASH   B DECOY"
#define HELP3 "ESCORT TO THE GATE"
static uint8_t px,py,dx[3],dy[3],following[3],enemyx[3],enemyy[3],delivered,dash,decoy,inv,gate;
static void arena(void){uint8_t x,y,i;clear();for(y=2;y<16;++y)for(x=0;x<20;++x)if((x*11+y*7)%39==0)tile(x,y,1);for(x=0;x<20;++x){tile(x,1,3);tile(x,16,3);}hud();text(9,17,"RESCUED");number(17,17,delivered,1);tile(18,8,51);tile(18,9,51);for(i=0;i<3;++i){dx[i]=3+i*5;dy[i]=4+(i*3)%9;following[i]=0;enemyx[i]=12+i*2;enemyy[i]=5+i*3;}px=2;py=9;gate=0;}
static void start(void){score=0;hp=5;level=1;delivered=0;dash=0;decoy=0;inv=120;arena();game_state=1;}
static void tick(void){uint8_t i,ox=px,oy=py,follow=0;if(frame%3==0){if(keys&J_LEFT)px--;if(keys&J_RIGHT)px++;if(keys&J_UP)py--;if(keys&J_DOWN)py++;if(px<1)px=1;if(px>18)px=18;if(py<2)py=2;if(py>15)py=15;
 for(i=0;i<3;++i)if(following[i]==1){uint8_t tx=ox,ty=oy,j;for(j=0;j<i;++j)if(following[j]==1){tx=dx[j];ty=dy[j];}if(dx[i]<tx)dx[i]++;else if(dx[i]>tx)dx[i]--;if(dy[i]<ty)dy[i]++;else if(dy[i]>ty)dy[i]--;}}
 if((keys&J_A)&&!(old_keys&J_A)&&!dash){dash=45;if(px<16)px+=2;beep(190);}if(dash)dash--;if((keys&J_B)&&!(old_keys&J_B)&&!decoy){decoy=90;beep(125);}if(decoy)decoy--;if(inv)inv--;
 for(i=0;i<3;++i){if(following[i]==0&&hit(px,py,dx[i],dy[i],2)){following[i]=1;score+=10;beep(210);}if(following[i]==1)follow++;if(following[i]<2)spr(i+1,16,following[i]==1?0:2,dx[i]*8,dy[i]*8);else hide_sprite(i+1);if(frame%(11+i*3)==0){if(!decoy){if(enemyx[i]<px)enemyx[i]++;else if(enemyx[i]>px)enemyx[i]--;if(enemyy[i]<py)enemyy[i]++;else if(enemyy[i]>py)enemyy[i]--;}}spr(i+4,17,1,enemyx[i]*8,enemyy[i]*8);if(!inv&&hit(px,py,enemyx[i],enemyy[i],2)){hp--;inv=80;px=2;py=9;beep(40);}}
 spr(0,10,0,px*8,py*8);if(px>=17&&py>=7&&py<=10&&follow){for(i=0;i<3;++i)if(following[i]==1){following[i]=2;hide_sprite(i+1);delivered++;score+=35;}beep(230);}
 number(17,17,delivered,1);if(delivered==3){if(level==3)finish(1);else{level++;score+=100;delivered=0;arena();}}if(!hp)finish(0);}
#elif GAME_ID==5
#define TITLE "AIRLOCK ALERT"
#define SUB "THREE DOORS. AIM."
#define HELP1 "LEFT RIGHT DOOR"
#define HELP2 "A FIRE   B SCAN"
#define HELP3 "SAVE FRIENDS STOP FOES"
static uint8_t door,entity[3],clock[3],resolved,scan,spawn,streak;
static void arena(void){uint8_t x,i;clear();hud();for(i=0;i<3;++i){x=2+i*6;tile(x,6,52);tile(x+1,6,52);tile(x+2,6,52);tile(x,12,52);tile(x+1,12,52);tile(x+2,12,52);entity[i]=0;clock[i]=0;}text(0,17,"CLEARED");number(8,17,resolved,2);}
static void start(void){score=0;hp=3;level=1;door=1;resolved=0;scan=0;spawn=0;streak=0;arena();game_state=1;}
static void tick(void){uint8_t i,x;if((keys&J_LEFT)&&!(old_keys&J_LEFT)&&door)door--;if((keys&J_RIGHT)&&!(old_keys&J_RIGHT)&&door<2)door++;if((keys&J_B)&&!(old_keys&J_B))scan=35;if(scan)scan--;
 if((keys&J_A)&&!(old_keys&J_A)){if(entity[door]==2){score+=20+streak*5;streak++;if(streak>6)streak=6;resolved++;entity[door]=0;beep(220);}else if(entity[door]==1){hp--;streak=0;entity[door]=0;beep(35);}else{streak=0;beep(80);}}
 if(frame%6==0){for(i=0;i<3;++i)if(entity[i]){clock[i]++;if(clock[i]>14-level*2){if(entity[i]==2){hp--;streak=0;beep(40);}else{score+=10;resolved++;beep(170);}entity[i]=0;}}if(++spawn>=6-level){spawn=0;i=rand8()%3;if(!entity[i]){entity[i]=(rand8()&3)?2:1;clock[i]=0;}}}
 for(i=0;i<3;++i){x=2+i*6;spr(i,entity[i]==2?17:18,entity[i]==2?1:0,entity[i]?x*8+4:168,8*8);if(entity[i]){text(x,11,entity[i]==2?"!":"+");}else text(x,11," ");}spr(3,19,2,(2+door*6)*8+4,13*8);if(scan)text(0,2,"SCAN ACTIVE");else text(0,2,"           ");
 number(8,17,resolved,2);if(resolved>=10*level){if(level==3)finish(1);else{level++;score+=100;resolved=0;arena();}}if(!hp)finish(0);}
#endif

static void title(void){uint8_t x,y;clear();for(y=1;y<17;++y)for(x=0;x<20;++x)if((x*13+y*17)%47==0)tile(x,y,1);text(2,3,TITLE);text(2,5,SUB);text(2,8,HELP1);text(2,9,HELP2);text(2,10,HELP3);text(2,14,"START TO LAUNCH");spr(0,10,0,70,102);game_state=0;}
void main(void){DISPLAY_OFF;set_bkg_data(0,BG_TILE_COUNT,bg_tiles);set_bkg_data(BG_TILE_COUNT,EXTRA_BG_COUNT,extra_bg_tiles);set_sprite_data(0,SPRITE_TILE_COUNT,sprite_tiles);set_sprite_data(SPRITE_TILE_COUNT,EXTRA_SPRITE_COUNT,extra_sprite_tiles);set_bkg_palette(0,1,bg_pal);set_sprite_palette(0,1,cyan);set_sprite_palette(1,1,red);set_sprite_palette(2,1,gold);SHOW_BKG;SHOW_SPRITES;NR52_REG=0x80;NR50_REG=0x77;NR51_REG=0xff;DISPLAY_ON;title();while(1){wait_vbl_done();frame++;keys=joypad();if(game_state==0||game_state==2||game_state==3){if((keys&J_START)&&!(old_keys&J_START))start();}else{tick();if(game_state==1&&frame%8==0)hud();}old_keys=keys;}}
