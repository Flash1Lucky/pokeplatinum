from pathlib import Path
import re,struct,sys
from verify_fairy_archives import members
from verify_fairy_v2_assets import ids,palette

def main():
    build=Path(sys.argv[1]);data=members(build/'res/items/item_icon.narc')
    mapping=ids(build/'res/items/item_icon.naix')
    assert len(data)==712 and mapping['pixie_plate_NCLR']==711
    entry=re.search(r'\[468\]\s*=\s*\{(.*?)\}',(build/'res/items/item_id_map.h').read_text(),re.S)[1]
    assert '.iconID = flame_plate_NCGR' in entry and '.paletteID = pixie_plate_NCLR' in entry
    colors=[(180,180,180),(49,49,49),(193,141,171),(255,173,201),(255,199,231),(255,216,238),(255,232,252),(214,156,191)]+[(0,0,0)]*8
    expected=b''.join(struct.pack('<H',(r//8)|((g//8)<<5)|((b//8)<<10)) for r,g,b in colors)
    packed=palette(data[711])
    assert len(packed)==512,'Item palette must retain the upstream 256-color padded layout'
    assert packed==expected+bytes(480),'Built Pixie Plate palette differs from original pink art'
    print('PASS Pixie Plate: original plate tile + original pink Fairy palette at appended member 711; 712 total members')

if __name__=='__main__':main()
