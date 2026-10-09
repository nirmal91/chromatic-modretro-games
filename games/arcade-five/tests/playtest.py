"""Boot the exact shipping ROMs and exercise real Game Boy inputs in PyBoy."""
from pathlib import Path
from pyboy import PyBoy

ROOT=Path(__file__).resolve().parents[2]
OUT=Path(__file__).resolve().parent/'evidence'
OUT.mkdir(exist_ok=True)
GAMES={
 'cargo-crush':'CARGO CRUSH',
 'storm-riders':'STORM RIDERS',
 'parcel-panic':'PARCEL PANIC',
 'lost-drones':'LOST DRONES',
 'airlock-alert':'AIRLOCK ALERT',
}
CHARS='ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:/-+!.?<>'
def glyph(c):return 0 if c==' ' else 4+CHARS.index(c)
def reads(p,x,y,s):return all(p.tilemap_background[x+i,y]-256==glyph(c) for i,c in enumerate(s))
def tap(p,k,n=2):p.button_press(k);p.tick(n,render=True);p.button_release(k);p.tick(2,render=True)
def boot(slug,title):
 p=PyBoy(str(ROOT/slug/'dist'/f'{slug}.gbc'),window='null',cgb=True,sound_emulated=False)
 p.set_emulation_speed(0);p.tick(120,render=True)
 assert reads(p,2,3,title),f'{slug}: title missing'
 p.screen.image.save(OUT/f'{slug}-title.png')
 tap(p,'start');p.tick(80,render=True)
 assert reads(p,0,0,'SCORE'),f'{slug}: gameplay HUD missing'
 p.screen.image.save(OUT/f'{slug}-play.png')
 return p

for slug,title in GAMES.items():
 p=boot(slug,title)
 try:
  if slug=='cargo-crush':
   assert p.tilemap_background[4,5]-256==49, 'Starting crate missing'
   p.button_press('up');p.tick(28);p.button_release('up');p.tick(4)
   assert p.tilemap_background[4,4]-256==49 and p.tilemap_background[4,5]-256!=49, 'Crate did not move when pushed'
   tap(p,'a')
  elif slug=='storm-riders':
   y=p.get_sprite(0).y
   tap(p,'a');p.tick(5)
   assert p.get_sprite(0).y<y, 'Flap did not gain altitude'
   tap(p,'b')
  elif slug=='parcel-panic':
   y=p.get_sprite(12).y
   tap(p,'down')
   assert p.get_sprite(12).y>y,'Lane selector did not move'
   tap(p,'a')
   assert any(p.get_sprite(i).on_screen for i in range(4,8)),'No parcel sent'
  elif slug=='lost-drones':
   x=p.get_sprite(0).x
   p.button_press('right');p.tick(9);p.button_release('right')
   assert p.get_sprite(0).x>x,'Rescuer did not move'
   tap(p,'b');tap(p,'a')
  else:
   x=p.get_sprite(3).x
   tap(p,'right')
   assert p.get_sprite(3).x>x,'Door cursor did not move'
   tap(p,'b')
   assert reads(p,0,2,'SCAN ACTIVE'),'Scan cue missing'
  p.tick(120,render=True)
  p.screen.image.save(OUT/f'{slug}-action.png')
  print(f'PASS {slug}: title, start, real inputs, rendering')
 finally:p.stop()
