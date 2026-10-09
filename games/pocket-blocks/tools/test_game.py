"""Input-driven smoke checks plus explicitly prepared WRAM board scenarios."""
from pathlib import Path
import json
from pyboy import PyBoy
root=Path(__file__).resolve().parents[1]
rom=root/'dist/pocket-blocks.gbc'
symbols={s.split()[1]:int(s.split()[2],16) for s in (root/'dist/pocket-blocks.noi').read_text().splitlines() if s.startswith('DEF ')}
import os
dmg=os.environ.get('POCKET_BLOCKS_DMG')=='1'
p=PyBoy(str(rom),window='null',sound_emulated=False,cgb=not dmg)
def read(name): return p.memory[symbols['_'+name]]
def write(name,value): p.memory[symbols['_'+name]]=value
def press(key,frames=12):
    p.button_press(key);p.tick(frames);p.button_release(key);p.tick(12)
def word(name): return read(name)+p.memory[symbols['_'+name]+1]*256
checks=[]
p.tick(120);p.screen.image.save(root/('dist/title-dmg.png' if dmg else 'dist/title.png'))
assert read('state')==0
press('start');assert read('state')==1
p.screen.image.save(root/('dist/play-dmg.png' if dmg else 'dist/play.png'));checks.append('Title and Start enter live 10x16 game')
x=read('piece_x');press('left');assert read('piece_x')<x;checks.append('Left moves live piece')
x=read('piece_x');press('right');assert read('piece_x')>x;checks.append('Right moves live piece')
r=read('rotation');press('a');assert read('rotation')==(r+1)%4;checks.append('A rotates live piece')
r=read('rotation');press('b');assert read('rotation')==(r-1)%4;checks.append('B rotates counterclockwise')
before=word('score');press('down');assert word('score')>before;checks.append('Soft drop advances piece and awards score')
press('start');assert read('paused')==1
y=read('piece_y');p.tick(120);assert read('piece_y')==y;checks.append('Pause freezes gravity')
press('start');assert read('paused')==0
before=word('score');press('up');assert word('score')>before;checks.append('Hard drop locks piece and awards score')
# Prepare four bottom rows, leaving a one-cell vertical gap for an I shape.
# This deterministic fixture tests the actual ROM lock/clear/scoring path.
for y in range(16):
    for x in range(10):p.memory[symbols['_board']+y*10+x]=1 if y>=12 and x!=5 else 0
write('type',0);write('rotation',1);write('piece_x',3);write('piece_y',0);write('gravity',0)
write('lines',9);p.memory[symbols['_lines']+1]=0
write('level',1);before=word('score');press('up')
assert word('lines')==13 and read('level')==2
assert word('score')>=before+800
assert all(p.memory[symbols['_board']+y*10+x]==0 for y in range(12,16) for x in range(10))
checks.append('Prepared four-line clear awards 800 points and advances level at ten lines')
p.screen.image.save(root/('dist/line-clear-dmg.png' if dmg else 'dist/line-clear.png'))
for y in range(16):
    for x in range(10):p.memory[symbols['_board']+y*10+x]=1 if y==0 and 3<=x<=6 else 0
# Existing piece must first lock lower in the well, then spawn collides at row zero.
write('type',1);write('rotation',0);write('piece_x',3);write('piece_y',10);write('gravity',0)
press('up');assert read('state')==2;checks.append('Blocked spawn shows game over')
p.tick(30);p.screen.image.save(root/('dist/game-over-dmg.png' if dmg else 'dist/game-over.png'))
press('start');assert read('state')==1 and word('score')==0 and word('lines')==0
checks.append('Start restarts and resets score/lines')
p.stop(save=False)
(root/('dist/test-results-dmg.json' if dmg else 'dist/test-results.json')).write_text(json.dumps({'rom':rom.name,'mode':'DMG' if dmg else 'CGB','checks':checks,'board_fixtures':'Line clear and top-out use explicitly prepared WRAM; movement, pause, rotations and drops use buttons.'},indent=2)+'\n')
print('\n'.join(checks))
