"""Real-input regression of the shipping cartridge. No RAM writes or cheats."""
import hashlib, json, re, shutil, tempfile
from pathlib import Path
from pyboy import PyBoy

ROOT=Path(__file__).resolve().parents[1]
ROUTE=json.loads((ROOT/'tests/route.json').read_text())
MANIFEST=json.loads((ROOT/'dist/manifest.json').read_text())
ROM=ROOT/'dist/midnight-courier.gbc'
SHA=hashlib.sha256(ROM.read_bytes()).hexdigest()
assert SHA==MANIFEST['romSha256'], 'ROM changed: export matching debug symbols and repackage.'
for relative,expected in MANIFEST['sourceFiles'].items():
    assert hashlib.sha256((ROOT/relative).read_bytes()).hexdigest()==expected, f'Source changed without rebuilding: {relative}'
for relative in ['dist/symbols.noi','dist/globals.i']:
    assert hashlib.sha256((ROOT/relative).read_bytes()).hexdigest()==MANIFEST['debugFiles'][relative]
base=int(re.search(r'DEF _script_memory (0x[0-9A-Fa-f]+)',(ROOT/'dist/symbols.noi').read_text())[1],16)
slots={k.lower():int(v) for k,v in re.findall(r'^VAR_(\w+) = (\d+)$',(ROOT/'dist/globals.i').read_text(),re.M)}
results=[]
shots=ROOT/'test-results';shots.mkdir(exist_ok=True)
class Run:
    def __init__(self,name):
        self.name=name;self.temp=tempfile.TemporaryDirectory();self.rom=Path(self.temp.name)/ROM.name;shutil.copyfile(ROM,self.rom)
        self.gb=PyBoy(str(self.rom),window='null',sound_emulated=False);self.gb.set_emulation_speed(0);self.held=set();self.steps=0
        self.gb.tick(ROUTE['initialFrames'],True)
    def step(self,buttons,frames):
        held=set(buttons)
        for key in self.held-held:self.gb.button_release(key)
        for key in held-self.held:self.gb.button_press(key)
        self.held=held;self.gb.tick(frames,True);self.steps+=1
    def values(self):
        return {n:self.gb.memory[base+2*i]|(self.gb.memory[base+2*i+1]<<8) for n,i in slots.items()}
    def check(self,name,expected):
        vals=self.values()
        for key,value in expected.items():assert vals[key]==value,f'{self.name}/{name}: {key} expected {value}, got {vals[key]}'
        self.gb.screen.image.save(shots/f'{self.name}-{name}.png');results.append({'scenario':self.name,'check':name,'values':vals})
    def play(self,steps,checkpoints=False):
        for i,action in enumerate(steps,1):
            self.step(**action)
            if checkpoints:
                for cp in ROUTE['checkpoints']:
                    if cp['afterStep']==i:self.check(cp['name'],cp['expect'])
    def close(self):self.gb.stop(save=False);self.temp.cleanup()

def scenario(name,fn):
    r=Run(name)
    try:fn(r)
    finally:r.close()
    print(f'PASS {name}')

def delivery(r):
    r.play(ROUTE['steps'],True)
    r.check('fresh-title',{'shields':3,'cores':0,'log':0,'state':0})
def ghost(r):
    # Skip the deliberately wrong SUN entry. All movement and state changes are real inputs.
    r.play(ROUTE['steps'][:50]+ROUTE['steps'][54:ROUTE['winIndex']])
    r.check('perfect-delivery',{'shields':3,'cores':3,'log':1,'state':5})
    seconds=r.values()['seconds'];r.step([],180);r.check('timer-stopped',{'seconds':seconds,'state':5})
def timeout(r):
    r.play(ROUTE['steps'][:ROUTE['escapeIndex']]);r.check('escape',{'state':4})
    r.step([],3000);r.check('time-expired',{'state':6,'seconds':0})
    r.step(['start'],90);r.step([],90);r.check('retry',{'state':0,'shields':3,'cores':0})
def damage(r):
    r.play(ROUTE['steps'][:ROUTE['escapeIndex']]);r.step(['up'],200);r.step([],120)
    r.check('zero-shields',{'state':6,'shields':0})
def live_grid(r):
    r.play(ROUTE['steps'][:33]);r.check('grid-is-live',{'grid':0,'shields':3,'state':2})
    r.step(['up'],64);r.step([],120)
    r.check('live-beam-hurts',{'grid':0,'shields':2,'state':2})

def locked_lift(r):
    r.play(ROUTE['steps'][:33])
    # Relay, no second core: intentionally touch the powered grid first,
    # then test its locked lift on a separate branch after cutting power.
    r.play(ROUTE['steps'][33:40]);r.step(['a'],1);r.step([],30)
    r.step(['up'],32);r.step([],8);r.step(['left'],48);r.step([],8)
    r.step(['up'],64);r.step([],120)
    assert r.gb.memory[0xFF4A] < 144, 'Locked-lift dialogue did not appear'
    r.check('lift-rejects-missing-core',{'cores':1,'state':2})

for name,fn in [('delivery',delivery),('ghost',ghost),('timeout',timeout),('damage',damage),('live-grid',live_grid),('locked-lift',locked_lift)]:scenario(name,fn)
(shots/'results.json').write_text(json.dumps({'romSha256':SHA,'scenarios':6,'checks':results},indent=2)+'\n')
print(f'{len(results)} checks passed on {SHA}')
