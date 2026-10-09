"""Input-driven cartridge smoke tests. Requires PyBoy; uses linker symbols for assertions."""
from pathlib import Path
from pyboy import PyBoy
ROOT=Path(__file__).resolve().parent.parent
symbols={}
for line in (ROOT/'dist/pocket-jumper.noi').read_text().splitlines():
    parts=line.split()
    if len(parts)==3 and parts[0]=='DEF': symbols[parts[1].lstrip('_')]=int(parts[2],16)
p=PyBoy(str(ROOT/'dist/pocket-jumper.gbc'),window='null',sound_emulated=False)
p.set_emulation_speed(0)
def byte(name): return p.memory[symbols[name]]
def word(name):
    address=symbols[name];return p.memory[address]+256*p.memory[address+1]
def tap(key): p.button_press(key);p.tick(2);p.button_release(key);p.tick(2)
p.tick(180); assert byte('game_state')==0
p.screen.image.save(str(ROOT/'dist/title.png'))
tap('start');p.tick(40);assert byte('game_state')==1
assert word('player_y')==112
x=word('player_x');p.button_press('right');p.tick(12);p.button_release('right');assert word('player_x')>x
before=word('player_y');p.button_press('a');p.tick(8);assert word('player_y')<before;p.button_release('a');p.tick(50)
tap('start');assert byte('game_state')==2
x=word('player_x');p.button_press('right');p.tick(30);p.button_release('right');assert word('player_x')==x
tap('start');assert byte('game_state')==1
# Follow terrain using genuine controller inputs. No writes to game memory.
p.button_press('right');p.button_press('b')
jump_frames=0;max_x=0
for frame in range(1500):
    x=word('player_x');y=word('player_y');max_x=max(max_x,x)
    if byte('game_state')!=1:break
    local=x%256
    # Climb the two ledges, then jump past the critter/gap area.
    need_jump=(34<=local<=52 or 98<=local<=116 or 150<=local<=175 or 216<=local<=231)
    if byte('grounded') and need_jump and not jump_frames:
        p.button_press('a');jump_frames=18
    if jump_frames:
        jump_frames-=1
        if not jump_frames:p.button_release('a')
    p.tick(1)
    if 330<x<360:p.screen.image.save(str(ROOT/'dist/scrolling.png'))
assert max_x>320,(max_x,byte('game_state'),byte('lives'))
assert byte('game_state')==3, 'Route must reach the flag'
win_coins=byte('collected')
assert win_coins>0, 'At least one coin must be collected through controller input'
p.screen.image.save(str(ROOT/'dist/win.png'))
p.button_release('right');p.button_release('b');p.button_release('a');p.tick(10);tap('start');p.tick(40)
assert byte('game_state')==1 and byte('lives')==3
for life in range(3):
    p.button_press('right');p.button_press('b');jump_frames=0
    for attempt in range(200):
        x=word('player_x')
        if x>=182:break
        if byte('grounded') and (34<=x<=52 or 98<=x<=116 or 150<=x<=175) and not jump_frames:
            p.button_press('a');jump_frames=18
        if jump_frames:
            jump_frames-=1
            if not jump_frames:p.button_release('a')
        p.tick(1)
    p.button_release('right');p.button_release('b');p.button_release('a');p.tick(100)
    assert byte('lives')==2-life
assert byte('game_state')==4
p.screen.image.save(str(ROOT/'dist/game-over.png'))
tap('a');p.tick(30);assert byte('game_state')==1 and byte('lives')==3
print({'movement':True,'jump':True,'pause':True,'scrolling':max_x,'state':byte('game_state'),'lives':byte('lives'),'win_coins':win_coins,'win':True,'fall_lives':True,'game_over_restart':True})
p.screen.image.save(str(ROOT/'dist/playtest-final.png'))
p.stop()
