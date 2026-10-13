"""Read-only format and coverage validator for hand-edited Platinum font sources."""
from pathlib import Path
import json
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
PALETTE = [144, 200, 255, 56, 56, 56, 216, 216, 216, 255, 255, 255]

def validate(root=ROOT):
    folder = root / 'res/fonts'
    original = Image.open(folder / 'font_system.png')
    original_meta = json.loads((folder / 'font_system.json').read_text())
    result = {}
    for family in ['narrow', 'narrower', 'short', 'shorter']:
        image = Image.open(folder / f'font_{family}.png')
        meta = json.loads((folder / f'font_{family}.json').read_text())
        assert image.mode == 'P' and image.size == (256, 512), family
        assert image.getpalette()[:12] == PALETTE, family
        assert set(image.tobytes()) <= {0, 1, 2, 3}, family
        assert set(meta) == {'maxGlyphWidth', 'maxGlyphHeight', 'glyphWidths'}, family
        assert meta['maxGlyphWidth'] == meta['maxGlyphHeight'] == 16, family
        widths = meta['glyphWidths']
        assert len(widths) == 509 and all(type(w) is int and 0 <= w <= 16 for w in widths), family
        modified = []
        for i in range(509):
            x, y = i % 16 * 16, i // 16 * 16
            box = (x, y, x + 16, y + 16)
            tile, baseline = image.crop(box), original.crop(box)
            if tile.tobytes() != baseline.tobytes() or widths[i] != original_meta['glyphWidths'][i]:
                modified.append(i + 1)
                # Unchanged baseline cells can contain guide pixels outside advance.
                assert all(tile.getpixel((xx, yy)) not in (1, 2)
                           for yy in range(16) for xx in range(widths[i], 16)), (family, i + 1)
        result[family] = {'glyphs': 509, 'modified_vs_system': modified,
                          'unchanged_slots': 509 - len(modified), 'valid': True}
    return result

if __name__ == '__main__':
    print(json.dumps(validate(), indent=2))
