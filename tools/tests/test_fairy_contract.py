"""Fairy integration contracts; run with Python 3 from either feature checkout.

Host checks complement, but do not replace, DS/emulator acceptance.
"""
from pathlib import Path
import json, re, subprocess, unittest, tempfile, shutil, os

ROOT = Path(__file__).resolve().parents[2]
BASE = 'c248fb3f8'

def read(name):
    return (ROOT / name).read_text(encoding='utf-8')

def baseline(name):
    repo=Path(os.environ.get('FAIRY_BASELINE_REPO', ROOT))
    return subprocess.check_output(['git', '-c', f'safe.directory={repo.as_posix()}', '-C', str(repo), 'show', f'{BASE}:{name}']).decode()

def chart(source):
    table = re.search(r'sTypeMatchupMultipliers\[\]\[3\] = \{(.*?)\n\};', source, re.S)[1]
    values = {'NOT_VERY_EFF': .5, 'SUPER_EFF': 2, 'IMMUNE': 0}
    return {(a,b):values[v] for a,b,v in re.findall(r'\{ TYPE_(\w+), TYPE_(\w+), TYPE_MULTI_(\w+) \}', table)}

class FairyContract(unittest.TestCase):
    def test_stable_ids(self):
        types = read('generated/pokemon_types.txt').splitlines()
        self.assertEqual(types[9], 'TYPE_MYSTERY')
        self.assertEqual(types[18:], ['TYPE_FAIRY','NUM_POKEMON_TYPES'])
        self.assertEqual(types[:18], baseline('generated/pokemon_types.txt').splitlines()[:-1])
        for file, new in [('items','ITEM_PIXIE_PLATE'), ('item_hold_effects','HOLD_EFFECT_ARCEUS_FAIRY')]:
            before = baseline(f'generated/{file}.txt').splitlines()
            after = read(f'generated/{file}.txt').splitlines()
            if file == 'items': before,after = before[:-1],after[:-1]
            self.assertEqual(after, before + [new])
        self.assertEqual(read('res/moves/curse/data.json'), baseline('res/moves/curse/data.json'))

    def test_chart_and_poketch_agree_for_all_361_pairs(self):
        actual = chart(read('src/battle/battle_lib.c'))
        original = chart(baseline('src/battle/battle_lib.c'))
        self.assertEqual({k:v for k,v in actual.items() if 'FAIRY' not in k},original)
        atk = {'FIGHTING':2,'DRAGON':2,'DARK':2,'FIRE':.5,'POISON':.5,'STEEL':.5}
        defense = {'FIGHTING':.5,'BUG':.5,'DARK':.5,'DRAGON':0,'POISON':2,'STEEL':2}
        types = [x.removeprefix('TYPE_') for x in read('generated/pokemon_types.txt').splitlines()[:-1]]
        for t in types:
            self.assertEqual(actual.get(('FAIRY',t),1),atk.get(t,1))
            self.assertEqual(actual.get((t,'FAIRY'),1),defense.get(t,1))
        rows = re.findall(r'\[TYPE_(\w+)\]\s*= \{(.*?)\}',read('src/applications/poketch/move_tester/main.c'))
        self.assertEqual(len(rows),19)
        vals = {'______':1,'X(2_0)':2,'X(0_5)':.5,'X(0_0)':0}
        for a,row in rows:
            entries = [v.strip() for v in row.split(',')]
            self.assertEqual(len(entries),19)
            for b,v in zip(types,entries): self.assertEqual(vals[v],actual.get((a,b),1),(a,b))
        src=read('src/battle/battle_lib.c')
        self.assertLess(src.index('{ TYPE_DRAGON, TYPE_FAIRY,'),src.index('{ 0xFE, 0xFE,'))

    def test_species_and_plate(self):
        expected={**dict.fromkeys('cleffa clefairy clefable togepi snubbull granbull'.split(),['TYPE_FAIRY']*2),
                  **dict.fromkeys('ralts kirlia gardevoir mime_jr mr_mime'.split(),['TYPE_PSYCHIC','TYPE_FAIRY']),
                  **dict.fromkeys('igglybuff jigglypuff wigglytuff azurill'.split(),['TYPE_NORMAL','TYPE_FAIRY']),
                  **dict.fromkeys('marill azumarill'.split(),['TYPE_WATER','TYPE_FAIRY']),
                  **dict.fromkeys('togetic togekiss'.split(),['TYPE_FAIRY','TYPE_FLYING']),
                  'mawile':['TYPE_STEEL','TYPE_FAIRY']}
        self.assertEqual(len(expected),20)
        for mon,types in expected.items(): self.assertEqual(json.loads(read(f'res/pokemon/{mon}/data.json'))['types'],types,mon)
        plate=json.loads(read('res/items/data/pixie_plate.json'))
        self.assertEqual(plate['effectParam'],20)
        self.assertEqual(plate['holdEffect'],'HOLD_EFFECT_ARCEUS_FAIRY')
        self.assertIn('{ HOLD_EFFECT_ARCEUS_FAIRY, TYPE_FAIRY }',read('src/battle/battle_lib.c'))

    def test_append_only_archives_and_pokedex_animation(self):
        for name in ['res/items/item_icon.order','res/graphics/battle/sprites.order']:
            old=baseline(name).splitlines();new=read(name).splitlines()
            self.assertEqual(new[:len(old)],old)
        old=json.loads(baseline('res/graphics/pokedex/type_icons_anim.json'))
        new=json.loads(read('res/graphics/pokedex/type_icons_anim.json'))
        self.assertEqual(new['sequences'][:18],old['sequences'])
        self.assertEqual(new['animationResults'][:18],old['animationResults'])
        self.assertEqual(new['animationResults'][18]['index'],18)
        self.assertEqual(new['sequenceCount'],19)

    def test_judgment_all_existing_plate_mappings_extended(self):
        for name in ['src/battle/battle_lib.c','src/battle/trainer_ai/trainer_ai.c','src/pokemon.c']:
            src=read(name)
            self.assertEqual(src.count('case HOLD_EFFECT_ARCEUS_DARK:'),src.count('case HOLD_EFFECT_ARCEUS_FAIRY:'))
        script=read('res/battle/scripts/effects/effect_script_0268.s')
        self.assertIn('HOLD_EFFECT_ARCEUS_FAIRY, _Fairy',script)
        self.assertIn('_Fairy:\n    UpdateVar OPCODE_SET, BTLVAR_MOVE_TYPE, TYPE_FAIRY\n    GoTo _175',script)

    def test_actual_hall_rank_functions_with_canaries(self):
        cc=shutil.which('cc')
        if not cc: self.skipTest('C compiler required for executable Hall storage test')
        src=read('src/battle_hall_save.c')
        start=src.index('u8 BattleHall_GetRankOfType(')
        end=src.index('void BattleHallStreakFlags_Init(',start)
        program='''#include <stdint.h>
#include <assert.h>
#include <string.h>
typedef uint8_t u8;
#define HALL_NUM_TYPES 18
'''+src[start:end]+'''
int main(void) {
    u8 memory[11]; memset(memory, 0xa5, sizeof memory);
    u8 *ranks=memory+1;
    BattleHall_ResetRankOfAllTypes(ranks);
    assert(memory[0]==0xa5 && memory[10]==0xa5);
    for (int i=0;i<18;i++) { BattleHall_SetRankOfType(i,ranks,10); }
    for (int i=0;i<18;i++) { assert(BattleHall_GetRankOfType(i,ranks)==10); }
    BattleHall_SetRankOfType(18,ranks,9); BattleHall_SetRankOfType(255,ranks,9);
    assert(BattleHall_GetRankOfType(18,ranks)==0);
    assert(BattleHall_GetRankOfType(255,ranks)==0);
    assert(memory[0]==0xa5 && memory[10]==0xa5);
    for (int i=0;i<9;i++) assert(ranks[i]==0xaa);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp);(p/'hall.c').write_text(program)
            subprocess.run([cc,'-std=c99','-Wall','-Wextra','-Werror',str(p/'hall.c'),'-o',str(p/'hall')],check=True)
            subprocess.run([str(p/'hall')],check=True)

    def test_actual_arceus_equip_remove_helper(self):
        cc=shutil.which('cc')
        if not cc: self.skipTest('C compiler required for executable form helper test')
        src=read('src/pokemon.c')
        def function(signature):
            start=src.index(signature+'\n{')
            return src[start:src.index('\n}\n',start)+3]
        enums='enum { '+','.join(read('generated/pokemon_types.txt').splitlines())+' };\n'
        enums+='enum { '+','.join(read('generated/item_hold_effects.txt').splitlines())+' };\n'
        program='''#include <stdint.h>
#include <stddef.h>
#include <assert.h>
typedef uint8_t u8; typedef uint16_t u16;
typedef struct { int species, ability, heldItem, form; } BoxPokemon;
enum { MON_DATA_SPECIES, MON_DATA_ABILITY, MON_DATA_HELD_ITEM, MON_DATA_FORM };
enum { SPECIES_ARCEUS=493, ABILITY_MULTITYPE=121, ITEM_PARAM_HOLD_EFFECT=0, HEAP_ID_SYSTEM=0 };
static int BoxPokemon_GetValue(BoxPokemon *m,int field,void *unused) {
    (void)unused;
    switch(field) { case MON_DATA_SPECIES:return m->species; case MON_DATA_ABILITY:return m->ability;
    case MON_DATA_HELD_ITEM:return m->heldItem; default:return m->form; }
}
static void BoxPokemon_SetValue(BoxPokemon *m,int field,const void *value) {
    assert(field==MON_DATA_FORM); m->form=*(const int *)value;
}
// Isolate form lifecycle from item archive I/O; heldItem carries the hold effect.
static int Item_LoadParam(int effect,int field,int heap) { (void)field; (void)heap; return effect; }
u8 Pokemon_GetArceusTypeOf(u16 itemHoldEffect);
'''+enums+function('void BoxPokemon_SetArceusForm(BoxPokemon *boxMon)')+function('u8 Pokemon_GetArceusTypeOf(u16 itemHoldEffect)')+'''
int main(void) {
    BoxPokemon mon={SPECIES_ARCEUS,ABILITY_MULTITYPE,HOLD_EFFECT_ARCEUS_FAIRY,0};
    BoxPokemon_SetArceusForm(&mon); assert(mon.form==TYPE_FAIRY);
    mon.heldItem=HOLD_EFFECT_NONE;
    BoxPokemon_SetArceusForm(&mon); assert(mon.form==TYPE_NORMAL);
    mon.heldItem=HOLD_EFFECT_ARCEUS_STEEL;
    BoxPokemon_SetArceusForm(&mon); assert(mon.form==TYPE_STEEL);
    mon.species=1; mon.form=0; mon.heldItem=HOLD_EFFECT_ARCEUS_FAIRY;
    BoxPokemon_SetArceusForm(&mon); assert(mon.form==0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp);(p/'arceus.c').write_text(program)
            subprocess.run([cc,'-std=c99','-Wall','-Wextra','-Werror',str(p/'arceus.c'),'-o',str(p/'arceus')],check=True)
            subprocess.run([str(p/'arceus')],check=True)


    def test_actual_item_cache_includes_last_record(self):
        cc=shutil.which('cc')
        if not cc: self.skipTest('C compiler required for executable item cache test')
        src=read('src/item.c')
        start=src.index('void *ItemTable_Load(enum HeapID heapID)')
        end=src.index('\n}\n',start)+3
        program=r"""#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
enum HeapID { HEAP_TEST };
enum { NUM_ITEMS=468, ITEM_FILE_TYPE_DATA=0, NARC_INDEX_ITEMTOOL__ITEMDATA__PL_ITEM_DATA=0 };
typedef struct { unsigned char bytes[36]; } ItemData;
static int Item_FileID(int item,int kind) { assert(item==468 && kind==0); return 446; }
static void *NARC_AllocAndReadFromMemberByIndexPair(int narc,int member,enum HeapID heap,int offset,size_t size) {
    (void)narc;(void)member;(void)heap;(void)offset;
    assert(size==447*sizeof(ItemData));
    ItemData *table=calloc(447,sizeof(ItemData));
    table[446].bytes[2]=147; table[446].bytes[3]=20;
    return table;
}
"""+src[start:end]+r"""
int main(void) {
    ItemData *table=ItemTable_Load(HEAP_TEST);
    assert(table[446].bytes[2]==147 && table[446].bytes[3]==20);
    free(table); return 0;
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp);(p/'item.c').write_text(program)
            subprocess.run([cc,'-std=c99','-Wall','-Wextra','-Werror',str(p/'item.c'),'-o',str(p/'item')],check=True)
            subprocess.run([str(p/'item')],check=True)

    def test_pokedex_retains_every_original_cell(self):
        name='res/graphics/pokedex/type_icons_cell.json'
        old=json.loads(baseline(name));new=json.loads(read(name))
        self.assertEqual(new['cells'][:18],old['cells'])
        self.assertEqual(new['labels'][:18],old['labels'])
        self.assertEqual([o['Attr2']['CharName'] for o in new['cells'][18]['OAM']],[71,73])

if __name__ == '__main__': unittest.main(verbosity=2)
