"""Compare built Fairy archives with a verified clean upstream build.

Usage: python3 tools/tests/verify_fairy_archives.py BASELINE_BUILD FEATURE_BUILD
"""
from pathlib import Path
import struct, sys

def members(path):
    blob=path.read_bytes()
    assert blob[:4]==b'NARC',path
    pos=struct.unpack_from('<H',blob,12)[0]
    offsets=None;payload=None
    while pos<len(blob):
        kind,size=struct.unpack_from('<4sI',blob,pos)
        assert size>=8 and pos+size<=len(blob),(path,pos,size)
        if kind==b'BTAF':
            count=struct.unpack_from('<H',blob,pos+8)[0]
            offsets=[struct.unpack_from('<II',blob,pos+12+8*i) for i in range(count)]
        elif kind==b'GMIF':payload=blob[pos+8:pos+size]
        pos+=size
    assert offsets is not None and payload is not None,path
    return [payload[a:b] for a,b in offsets]

def main():
    before,after=map(Path,sys.argv[1:3])
    for name,extra in [('res/pokemon/pl_otherpoke.narc',4),('res/items/item_icon.narc',1),('res/items/pl_item_data.narc',1)]:
        old,new=members(before/name),members(after/name)
        assert len(new)==len(old)+extra,(name,len(old),len(new))
        assert new[:len(old)]==old,f'Existing member changed or shifted: {name}'
        print(f'PASS {name}: {len(old)} original members byte-identical; {extra} appended')
    for name in ['res/pokemon/zukan_data.narc','res/pokemon/zukan_data_gira.narc']:
        old,new=members(before/name),members(after/name)
        assert len(new)==len(old)+1,(name,len(old),len(new))
        # Type indices intentionally change membership; every body-shape and
        # pre-type member retains its exact index and bytes.
        assert new[:27]==old[:27],name
        assert new[44:58]==old[44:58],name
        fairy=struct.unpack('<'+'H'*(len(new[-1])//2),new[-1])
        assert len(fairy)==20,(name,len(fairy))
        print(f'PASS {name}: original non-type indices unchanged; 20 Fairy species appended')

if __name__=='__main__':main()
