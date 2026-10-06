"""Deterministic, hand-authored native 160x144 pixel backgrounds. Pillow only."""
from pathlib import Path
import json, random
from PIL import Image, ImageDraw
ROOT=Path(__file__).resolve().parents[1]
C=['#071821','#306850','#86c06c','#e0f8cf']
FONT={
'A':['01110','10001','10001','11111','10001','10001','10001'], 'B':['11110','10001','10001','11110','10001','10001','11110'],
'C':['01111','10000','10000','10000','10000','10000','01111'], 'D':['11110','10001','10001','10001','10001','10001','11110'],
'E':['11111','10000','10000','11110','10000','10000','11111'], 'F':['11111','10000','10000','11110','10000','10000','10000'],
'G':['01111','10000','10000','10111','10001','10001','01111'], 'H':['10001','10001','10001','11111','10001','10001','10001'],
'I':['111','010','010','010','010','010','111'], 'J':['00111','00010','00010','00010','10010','10010','01100'],
'K':['10001','10010','10100','11000','10100','10010','10001'], 'L':['10000','10000','10000','10000','10000','10000','11111'],
'M':['10001','11011','10101','10101','10001','10001','10001'], 'N':['10001','11001','10101','10011','10001','10001','10001'],
'O':['01110','10001','10001','10001','10001','10001','10001','01110'], 'P':['11110','10001','10001','11110','10000','10000','10000'],
'Q':['01110','10001','10001','10001','10101','10010','01101'], 'R':['11110','10001','10001','11110','10100','10010','10001'],
'S':['01111','10000','10000','01110','00001','00001','11110'], 'T':['11111','00100','00100','00100','00100','00100','00100'],
'U':['10001','10001','10001','10001','10001','10001','01110'], 'V':['10001','10001','10001','10001','10001','01010','00100'],
'W':['10001','10001','10001','10101','10101','10101','01010'], 'X':['10001','10001','01010','00100','01010','10001','10001'],
'Y':['10001','10001','01010','00100','00100','00100','00100'], 'Z':['11111','00001','00010','00100','01000','10000','11111'],
'0':['01110','10001','10011','10101','11001','10001','01110'], '1':['010','110','010','010','010','010','111'],
'2':['01110','10001','00001','00010','00100','01000','11111'], '3':['11110','00001','00001','01110','00001','00001','11110'],
'4':['00010','00110','01010','10010','11111','00010','00010'], '5':['11111','10000','10000','11110','00001','00001','11110'],
'6':['01110','10000','10000','11110','10001','10001','01110'], '7':['11111','00001','00010','00100','01000','01000','01000'],
'8':['01110','10001','10001','01110','10001','10001','01110'], '9':['01110','10001','10001','01111','00001','00001','01110'],
' ':['000']*7, '-':['00000','00000','00000','11111','00000','00000','00000'], '>':['100','010','001','010','100','000','000'],
'/':['00001','00001','00010','00100','01000','10000','10000'], ':':['0','1','0','0','1','0','0'], '.':['0','0','0','0','0','1','1']}
def txt(im,text,y,scale=1,color=3,x=None):
    text=text.upper(); w=sum(len(FONT.get(c,FONT[' '])[0])+1 for c in text)*scale-scale
    if x is None:x=(160-w)//2
    d=ImageDraw.Draw(im)
    for c in text:
        a=FONT.get(c,FONT[' '])
        for yy,row in enumerate(a):
            for xx,p in enumerate(row):
                if p=='1':d.rectangle((x+xx*scale,y+yy*scale,x+(xx+1)*scale-1,y+(yy+1)*scale-1),fill=C[color])
        x+=(len(a[0])+1)*scale

def new():return Image.new('RGB',(160,144),C[0])
def stars(im,seed=7):
    d=ImageDraw.Draw(im); r=random.Random(seed)
    for _ in range(75):
        x,y=r.randrange(160),r.randrange(144);d.point((x,y),fill=C[r.choice([1,1,2,3])])
def ship(im,x,y):
    d=ImageDraw.Draw(im)
    d.polygon([(x,y+9),(x+12,y+5),(x+17,y),(x+28,y),(x+34,y+7),(x+46,y+10),(x+33,y+17),(x+9,y+17)],fill=C[1],outline=C[2])
    d.rectangle((x+19,y+3,x+28,y+8),fill=C[3]);d.line((x+8,y+13,x+36,y+13),fill=C[2])
    for q in [10,30]:d.rectangle((x+q,y+18,x+q+5,y+21),fill=C[3])
def title():
    im=new();stars(im);d=ImageDraw.Draw(im)
    d.ellipse((100,51,181,130),fill=C[1],outline=C[2]);d.arc((79,79,187,108),0,360,fill=C[3],width=2)
    for y in range(110,144,4):d.line((0,y,159,y),fill=C[1])
    for x in range(-80,240,24):d.line((80,105,x,143),fill=C[2])
    d.rectangle((7,7,152,48),fill=C[0]);txt(im,'MIDNIGHT',10,2);txt(im,'COURIER',29,2,color=2)
    txt(im,'AN ORBITAL HEIST',54,color=2);ship(im,20,78)
    d.rectangle((25,114,134,128),fill=C[0],outline=C[2]);txt(im,'PRESS START',118)
    txt(im,'NIRMAL UTWANI',135,color=2)
    return im

def room(label):
    im=new();d=ImageDraw.Draw(im)
    for y in range(2,17):
        for x in range(1,19):
            d.rectangle((x*8,y*8,x*8+7,y*8+7),fill=C[1])
            d.point((x*8+1,y*8+1),fill=C[0]);d.line((x*8,y*8+7,x*8+7,y*8+7),fill=C[0])
    for y in range(2,18):
        for x in [0,19]:
            d.rectangle((x*8,y*8,x*8+7,y*8+7),fill=C[0],outline=C[2]);d.line((x*8+2,y*8+2,x*8+2,y*8+5),fill=C[3])
    d.rectangle((0,136,159,143),fill=C[0]);txt(im,label,137,color=2)
    d.line((0,15,159,15),fill=C[2]);return im

def crate(im,x,y,w,h):
    d=ImageDraw.Draw(im)
    for yy in range(y,y+h):
        for xx in range(x,x+w):
            a,b=xx*8,yy*8;d.rectangle((a,b,a+7,b+7),fill=C[0],outline=C[2]);d.line((a+2,b+2,a+5,b+5),fill=C[1])
def core(im,x,y):
    d=ImageDraw.Draw(im);a,b=x*8,y*8
    d.rectangle((a,b,a+15,b+15),fill=C[0],outline=C[2]);d.polygon([(a+7,b+2),(a+12,b+7),(a+7,b+13),(a+3,b+7)],fill=C[3]);d.line((a+7,b+4,a+9,b+7),fill=C[2])
def door(im,x=9,y=2):
    d=ImageDraw.Draw(im);a,b=x*8,y*8;d.rectangle((a,b,a+15,b+15),fill=C[0],outline=C[2]);d.polygon([(a+4,b+9),(a+8,b+5),(a+12,b+9)],fill=C[3]);d.line((a+8,b+6,a+8,b+13),fill=C[3])
def laser(im,x,y,w):
    d=ImageDraw.Draw(im)
    for xx in range(x,x+w):
        a,b=xx*8,y*8;d.rectangle((a,b,a+7,b+7),fill=C[0]);d.line((a,b+3,a+7,b+3),fill=C[3]);d.line((a,b+5,a+7,b+5),fill=C[2])
def terminal(im,x,y,label):
    d=ImageDraw.Draw(im);a,b=x*8,y*8;d.rectangle((a,b,a+15,b+15),fill=C[0],outline=C[2]);d.rectangle((a+3,b+2,a+12,b+9),fill=C[2]);d.line((a+4,b+13,a+11,b+13),fill=C[3]);txt(im,label,b+3,color=0,x=a+6)
def freight():
    im=room('01 / FREIGHT');crate(im,6,7,4,7);crate(im,12,4,2,3);crate(im,2,7,2,3)
    core(im,15,4);core(im,3,4);door(im);laser(im,12,10,6)
    txt(im,'CORE',25,x=116);txt(im,'LOG',25,x=20);return im

def relay():
    im=room('02 / RELAY');crate(im,6,10,3,4);crate(im,11,4,2,3);door(im);core(im,15,4);terminal(im,15,12,'0')
    laser(im,1,8,18);txt(im,'CUT GRID',84,x=108);txt(im,'CORE',25,x=116);return im

def archive():
    im=room('03 / ARCHIVE');door(im)
    for x,n in [(3,'1'),(9,'2'),(15,'3')]:terminal(im,x,6,n)
    txt(im,'SUN',34,x=24);txt(im,'MOON',34,x=65);txt(im,'STAR',34,x=112)
    txt(im,'MOON SUN STAR',86,color=3)
    crate(im,5,12,2,3);crate(im,13,12,2,3);laser(im,8,13,4);return im

def escape():
    im=room('04 / LAST TRAIN');door(im);d=ImageDraw.Draw(im)
    for y,x,w in [(12,1,13),(8,6,13),(4,1,13)]:laser(im,x,y,w)
    for x in [3,16]:
        for y in range(3,16,3):d.rectangle((x*8+3,y*8+2,x*8+4,y*8+5),fill=C[2])
    return im

def end(win):
    im=new();stars(im,19 if win else 9);d=ImageDraw.Draw(im)
    if win:
        d.ellipse((-40,90,200,290),fill=C[1],outline=C[2]);ship(im,58,65)
        txt(im,'DELIVERY',14,2);txt(im,'COMPLETE',34,2,color=2);txt(im,'YOU STOLE THE DAWN.',113,color=3)
    else:
        for r in range(38,4,-7):d.rectangle((80-r,70-r,80+r,70+r),outline=C[2 if r%2 else 1])
        txt(im,'SIGNAL',14,2);txt(im,'LOST',34,2,color=2);txt(im,'BAD NIGHT. TRY AGAIN.',108)
    txt(im,'START / NEW RUN',128,color=3);return im

if __name__=='__main__':
    for name,fn in [('title',title),('freight',freight),('relay',relay),('archive',archive),('escape',escape),('win',lambda:end(True)),('lost',lambda:end(False))]:
        fn().save(ROOT/'assets'/'backgrounds'/f'{name}.png')
    print('Authored seven backgrounds at native 160x144 resolution.')
