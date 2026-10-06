"""Verify generated resources, including the exact v1 visual regressions."""
from pathlib import Path
import re, struct, sys
from verify_fairy_archives import members

def unpack(data):
    if data[0]!=0x10:return data
    size=int.from_bytes(data[1:4],'little');out=bytearray();i=4
    while len(out)<size:
        flags=data[i];i+=1
        for bit in range(7,-1,-1):
            if len(out)>=size:break
            if flags&(1<<bit):
                a,b=data[i:i+2];i+=2
                count=(a>>4)+3;distance=((a&15)<<8)+b+1
                for _ in range(count):out.append(out[-distance])
            else:out.append(data[i]);i+=1
    return bytes(out[:size])

def blocks(data):
    data=unpack(data);pos=16
    while pos<len(data):
        size=struct.unpack_from('<I',data,pos+4)[0]
        yield data[pos:pos+4],data[pos:pos+size]
        pos+=size

def pixels(data):
    for kind,b in blocks(data):
        if kind==b'RAHC':
            size,offset=struct.unpack_from('<II',b,24)
            return b[8+offset:8+offset+size]
    raise AssertionError('No NCGR character block')

def palette(data):
    for kind,b in blocks(data):
        if kind==b'TTLP':
            size,offset=struct.unpack_from('<II',b,16)
            return b[8+offset:8+offset+size]
    raise AssertionError('No NCLR palette block')

def ids(path):
    return {key:int(value) for key,value in re.findall(r'^#define (\w+) (\d+)$',path.read_text(),re.M)}

def glyph(data):
    raw=pixels(data)
    # Both symbols are native 32x16 tiled 4bpp. Compare letter/shadow positions,
    # independent of their different background palette ramps.
    result=[]
    for y in range(16):
        for x in range(32):
            pixel=(y//8*4+x//8)*64+(y%8)*8+x%8
            value=(raw[pixel//2]>>(4*(pixel%2)))&15
            result.append(value in (14,15))
    return result

def main():
    before,after=map(Path,sys.argv[1:3])
    name='res/graphics/pokedex/zukan'
    old,new=members(before/(name+'.narc')),members(after/(name+'.narc'))
    index=ids(after/(name+'.naix'))['type_icons_NCGR_lz']
    old_pixels,new_pixels=pixels(old[index]),pixels(new[index])
    assert new_pixels[:len(old_pixels)]==old_pixels,'Original Pokedex tiles/background changed'
    assert len(new_pixels)==len(old_pixels)+384,(len(old_pixels),len(new_pixels))
    print('PASS Pokedex: all original rendered tile bytes preserved; Fairy appends 384 bytes')

    name='res/graphics/battle/sprites'
    old,new=members(before/(name+'.narc')),members(after/(name+'.narc'))
    assert new[:len(old)]==old,'Existing battle/contest/Mystery resources changed'
    mapping=ids(after/(name+'.naix'))
    assert glyph(new[mapping['type_icons_fairy_NCGR_lz']])!=glyph(new[mapping['type_icons_mystery_NCGR_lz']]),'Fairy still has Mystery letters'
    print('PASS battle icons: existing resources unchanged; Fairy glyph differs from Mystery')

    data=members(after/'res/items/pl_item_data.narc')
    assert data[446][2:4]==bytes([147,20]),'Pixie Plate hold effect/boost absent from built archive'
    mapping=(after/'res/items/item_id_map.h').read_text()
    assert re.search(r'\[468\]\s*=\s*\{\s*\.dataID\s*=\s*446',mapping)
    print('PASS Pixie Plate: item 468 maps to record 446, hold effect 147 and boost 20')

    data=members(after/'res/pokemon/pl_otherpoke.narc')
    for index in (253,254):
        block=next(b for kind,b in blocks(data[index]) if kind==b'RAHC')
        assert struct.unpack_from('<I',block,12)[0]==3,'Arceus must use native 4bpp format'
        assert len(pixels(data[index]))==6400,'Arceus must contain two 80x80 4bpp frames'
    for index,filename in [(255,'normal.pal'),(256,'shiny.pal')]:
        colors=(after.parent/'res/pokemon/arceus/forms/fairy'/filename).read_text().splitlines()[3:19]
        expected=b''.join(struct.pack('<H',(int(r)//8)|((int(g)//8)<<5)|((int(b)//8)<<10)) for r,g,b in (line.split() for line in colors))
        assert palette(data[index])==expected,(index,filename)
    mapping=ids(after/'res/pokemon/pl_otherpoke.naix')
    assert mapping['zz_arceus_fairy_back_NCGR']==253 and mapping['zz_arceus_fairy_front_NCGR']==254
    assert mapping['zz_arceus_fairy_normal_NCLR']==255 and mapping['zz_arceus_fairy_shiny_NCLR']==256
    print('PASS Fairy Arceus: back/front/normal/shiny IDs 253–256 and both built palettes match source')

if __name__=='__main__':main()
