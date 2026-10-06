"""Execute the real submenu handler and variable callbacks with host UI/save stubs."""
from pathlib import Path
import subprocess,tempfile,sys
ROOT=Path(__file__).resolve().parents[2]
if len(sys.argv)>1:ROOT=Path(sys.argv[1])
src=(ROOT/'src/overlay005/debug_menu.c').read_text()
def function(signature):
 start=src.index(signature+'\n{');return src[start:src.index('\n}\n',start)+3]
parts=[function(s) for s in [
 'static void DebugSubMenu_ChangeType(DebugSubMenu *subMenu, enum DebugSubMenuType type, int initialValue)',
 'static void SubMenuChoice_SelectVar(DebugSubMenu *subMenu)',
 'static void SubMenuChoice_VarValue(DebugSubMenu *subMenu)',
 'static void Task_DebugSubMenu_HandleInput(SysTask *task, void *data)']]
stub=r'''
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
typedef uint16_t u16;
typedef int SysTask;
typedef struct { int saveData; } FieldSystem;
typedef struct { FieldSystem *fieldSystem; } DebugMenu;
typedef struct { u16 value; } VarsFlags;
enum DebugSubMenuType { DEBUG_SUBMENU_SELECT_VAR, DEBUG_SUBMENU_VAR_VALUE, DEBUG_SUBMENU_SET_FLAG, DEBUG_SUBMENU_ADD_ITEM, DEBUG_SUBMENU_ITEM_QUANTITY, DEBUG_SUBMENU_BATTLE_RESULT };
typedef struct { enum DebugSubMenuType type; int value,digits,data[2]; DebugMenu *debugMenu; void *sprite; struct { void *spriteList; } spriteResMan; } DebugSubMenu;
typedef struct { void (*choiceFunc)(DebugSubMenu*); void (*renderFunc)(DebugSubMenu*); int closeOnChoice,preserveSprite,min,max; } DebugSubMenuConfig;
enum { PAD_KEY_UP=1,PAD_KEY_DOWN=2,PAD_KEY_LEFT=4,PAD_KEY_RIGHT=8,PAD_KEY=15,PAD_BUTTON_A=16,PAD_BUTTON_B=32,ITEM_UNUSED_113=113,ITEM_UNUSED_134=134,MAX_SUBMENU_DIGITS=5,SUBMENU_VALUE_SET_MIN=-1 };
static int keys,exits,cancels,writes,flags,items;
static VarsFlags save;
#define JOY_NEW(k) (keys & (k))
static const int sPowersOfTen[]={1,10,100,1000,10000};
static VarsFlags *SaveData_GetVarsFlags(int unused) { (void)unused;return &save; }
static u16 *VarsFlags_GetVarAddress(VarsFlags *s,int id) { assert(id==16513);return &s->value; }
static void DebugSubMenu_ExitToField(SysTask *t,DebugSubMenu *s) {(void)t;(void)s;exits++;}
static void DebugSubMenu_Close(SysTask *t,DebugSubMenu *s) {(void)t;(void)s;cancels++;}
static void SpriteList_Update(void *p) {(void)p;}
static void Sprite_DeleteAndFreeResources(void *p) {(void)p;}
static void render(DebugSubMenu *s) {(void)s;}
static void SubMenuChoice_SelectVar(DebugSubMenu *s);
static void SubMenuChoice_VarValue(DebugSubMenu *s);
static void DebugSubMenu_ChangeType(DebugSubMenu *s,enum DebugSubMenuType t,int value);
static void writeValue(DebugSubMenu *s) { writes++;SubMenuChoice_VarValue(s); }
static void toggleFlag(DebugSubMenu *s) {(void)s;flags^=1;}
static void chooseItem(DebugSubMenu *s) {DebugSubMenu_ChangeType(s,DEBUG_SUBMENU_ITEM_QUANTITY,1);}
static void addItem(DebugSubMenu *s) {(void)s;items++;}
static const DebugSubMenuConfig sSubMenuConfigs[]={
 {SubMenuChoice_SelectVar,render,0,0,16384,32767},
 {writeValue,render,1,0,0,65535},
 {toggleFlag,render,0,0,0,4095},
 {chooseItem,render,0,0,1,468},
 {addItem,render,0,1,1,999},
 {NULL,render,0,0,0,10}
};
'''
main=r'''
int main(void) {
 FieldSystem fs={0};DebugMenu menu={&fs};
 DebugSubMenu s={0};s.debugMenu=&menu;s.type=DEBUG_SUBMENU_SELECT_VAR;s.value=16513;save.value=1;
 keys=PAD_BUTTON_A;Task_DebugSubMenu_HandleInput(NULL,&s);
 assert(s.type==DEBUG_SUBMENU_VAR_VALUE && s.data[0]==16513 && s.value==1 && s.digits==0);
 assert(exits==0 && writes==0 && save.value==1);
 keys=PAD_KEY_UP;Task_DebugSubMenu_HandleInput(NULL,&s);assert(s.value==2 && save.value==1 && writes==0);
 keys=PAD_BUTTON_A;Task_DebugSubMenu_HandleInput(NULL,&s);assert(save.value==2 && writes==1 && exits==1);
 exits=writes=0;s.type=DEBUG_SUBMENU_SELECT_VAR;s.value=16513;save.value=1;
 keys=PAD_BUTTON_A;Task_DebugSubMenu_HandleInput(NULL,&s);
 keys=PAD_KEY_UP;Task_DebugSubMenu_HandleInput(NULL,&s);
 keys=PAD_BUTTON_B;Task_DebugSubMenu_HandleInput(NULL,&s);
 assert(save.value==1 && writes==0 && exits==0 && cancels==1);
 s.type=DEBUG_SUBMENU_SELECT_VAR;s.value=16513;keys=PAD_BUTTON_B;Task_DebugSubMenu_HandleInput(NULL,&s);
 assert(save.value==1 && writes==0 && cancels==2);
 s.type=DEBUG_SUBMENU_SET_FLAG;keys=PAD_BUTTON_A;Task_DebugSubMenu_HandleInput(NULL,&s);assert(flags==1 && exits==0);
 s.type=DEBUG_SUBMENU_ADD_ITEM;Task_DebugSubMenu_HandleInput(NULL,&s);assert(s.type==DEBUG_SUBMENU_ITEM_QUANTITY && items==0 && exits==0);
 Task_DebugSubMenu_HandleInput(NULL,&s);assert(items==1 && exits==0);
 return 0;
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.c').write_text(stub+'\n'.join(parts)+main)
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
print('PASS actual submenu handler: first A enters without writing/closing; second A writes once/closes; B cancels both stages; flag and item transitions preserved')
