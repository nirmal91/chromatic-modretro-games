"""Package a successful debug build from the Chromatic plugin, retaining exact identity."""
from pathlib import Path
import hashlib,json,shutil
R=Path(__file__).resolve().parents[1];out=R/'dist';out.mkdir(exist_ok=True)
rom=R/'build/midnight-courier.gbc';debug=Path(str(rom)+'.debug')
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
shutil.copy2(rom,out/rom.name)
for source,name in [(debug/'symbols.noi','symbols.noi'),(debug/'globals.i','globals.i')]:shutil.copy2(source,out/name)
files=[R/'project.gbsproj']+sorted((R/'project').rglob('*.gbsres'))+sorted(p for p in (R/'assets').rglob('*') if p.is_file() and p.suffix in {'.png','.json','.gbsres'})
manifest={'name':'Midnight Courier','version':'1.0.0','target':'Game Boy Color','romSha256':sha(rom),'romBytes':rom.stat().st_size,'compiler':'GB Studio 4.3.2','gbdk':'4.5.0','pyboy':'2.7.0','sourceFiles':{p.relative_to(R).as_posix():sha(p) for p in files},'debugFiles':{p.relative_to(R).as_posix():sha(p) for p in [out/'symbols.noi',out/'globals.i']}}
(out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
(out/'SHA256SUMS').write_text(f'{sha(rom)}  midnight-courier.gbc\n')
print(f'Packaged {rom.stat().st_size} bytes; SHA256 {sha(rom)}')
