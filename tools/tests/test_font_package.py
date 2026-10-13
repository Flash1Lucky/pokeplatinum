"""Exercise the actual C selector/printer with native-width tables and geometry guards."""
import json, pathlib, shutil, subprocess, tempfile, unittest
ROOT = pathlib.Path(__file__).resolve().parents[2]
def function(source, name):
    import re
    match=re.search(r'(?:static )?(?:BOOL|void|u32) '+name+r'\([^;]+?\)\n\{',source)
    assert match,name
    depth=1;end=match.end()
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[match.start():end]


class NameFontFamilyTests(unittest.TestCase):
    def test_actual_selector_and_printer(self):
        families=['system','message','subscreen','unown','narrow','narrower','short','shorter']
        widths=[json.loads((ROOT/'res/fonts'/('font_'+f+'.json')).read_text())['glyphWidths'] for f in families]
        source='''#include <assert.h>
#include <stddef.h>
typedef unsigned char u8;typedef unsigned short charcode_t;typedef unsigned int u32;typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define MAX_LETTER_HEIGHT 16
#define TEXT_SPEED_NO_TRANSFER 255
#define GET_TEXT_FG_COLOR(c) (((c)>>16)&255)
enum Font {FONT_SYSTEM,FONT_MESSAGE,FONT_SUBSCREEN,FONT_UNOWN,FONT_NARROW,FONT_NARROWER,FONT_SHORT,FONT_SHORTER,FONT_MAX};
typedef struct {u32 size;charcode_t chars[64];} String;
typedef struct {struct {u32 numGlyphs;} header;} FontManager;
typedef struct {FontManager *fontManagers[FONT_MAX];} FontWork;
static FontWork work,*sFontWork=&work;
typedef struct {u32 width,height;} Window;
static const charcode_t *String_GetData(const String*s){return s->chars;}
static u32 String_Length(const String*s){return s->size;}
static u32 Window_GetWidth(Window*w){return w->width;}
static u32 Window_GetHeight(Window*w){return w->height;}
static int draws,markers;static u32 drawX;static enum Font drawFont;
static void Text_AddPrinterWithParamsAndColor(Window*w,enum Font f,const String*s,u32 x,u32 y,int speed,u32 color,void*cb){draws++;drawX=x;drawFont=f;assert(x<w->width*8&&y+16<=w->height*8);}
static void Window_FillRectWithColor(Window*w,u8 c,u32 x,u32 y,u32 a,u32 b){markers++;assert(x+a<=w->width*8&&y+b<=w->height*8);}
'''
        source+='static const u8 widths[8][509]={'+','.join('{'+','.join(map(str,w))+'}' for w in widths)+'};\n'
        source+='static u32 Font_CalcStringWidth(enum Font f,const String*s,u32 spacing){u32 n=0;for(u32 i=0;i<s->size;i++)n+=widths[f][s->chars[i]-1]+spacing;return n;}\n'
        actual=(ROOT/'src/font.c').read_text()
        source+=function(actual,'Font_TryFitName')+'\n'+function(actual,'Font_PrintName')+'\n'
        source+='''int main(void){
FontManager m={{509}};for(int f=0;f<FONT_MAX;f++)work.fontManagers[f]=&m;
String s={0};enum Font chosen;u32 measured;int order[]={0,4,6,7,5};
unsigned lengths[]={0,1,12,16,27},budgets[]={0,1,32,63,64,71,87,88,112,128,180};
for(unsigned c=1;c<=509;c++)for(unsigned n=0;n<NELEMS(lengths);n++)for(unsigned b=0;b<NELEMS(budgets);b++){
 s.size=lengths[n];for(unsigned i=0;i<s.size;i++)s.chars[i]=c;
 int expected=-1;for(unsigned i=0;i<NELEMS(order);i++)if(widths[order[i]][c-1]*s.size<=budgets[b]){expected=order[i];break;}
 BOOL ok=Font_TryFitName(FONT_SYSTEM,&s,budgets[b],&chosen,&measured);
 assert(ok==(expected>=0));if(ok){assert(chosen==expected);assert(measured==widths[expected][c-1]*s.size);}
}
s.size=1;unsigned bad[]={0,510,0xfffe,0xffff,0xe000};for(unsigned i=0;i<5;i++){s.chars[0]=bad[i];assert(!Font_TryFitName(FONT_SYSTEM,&s,1000,&chosen,&measured));}
s.chars[0]=321;s.size=12;for(unsigned i=0;i<12;i++)s.chars[i]=321;
assert(!Font_TryFitName(FONT_SYSTEM,&s,64,&chosen,&measured)); // full-width user-edited W stays 72px
Window w={8,2};draws=markers=0;assert(!Font_PrintName(&w,FONT_SYSTEM,&s,0,0,64,1,1<<16));assert(draws==0&&markers==14);
s.size=1;draws=markers=0;assert(Font_PrintName(&w,FONT_SYSTEM,&s,0,0,999,1,1<<16));assert(draws==1&&drawX==29&&markers==0&&drawFont==FONT_SYSTEM);
assert(!Font_PrintName(&w,FONT_SYSTEM,&s,0,1,64,0,1<<16)); // no unsafe short line box
assert(!Font_PrintName(&w,FONT_SYSTEM,&s,64,0,64,0,1<<16));assert(!Font_PrintName(NULL,FONT_SYSTEM,&s,0,0,64,0,1<<16));
for(int f=0;f<FONT_MAX;f++)work.fontManagers[f]=NULL;assert(!Font_TryFitName(FONT_SYSTEM,&s,999,&chosen,&measured));
work.fontManagers[FONT_NARROW]=&m;assert(Font_TryFitName(FONT_SYSTEM,&s,999,&chosen,&measured)&&chosen==FONT_NARROW);
assert(!Font_TryFitName(FONT_MAX,&s,999,&chosen,&measured));assert(!Font_TryFitName(FONT_SYSTEM,NULL,999,&chosen,&measured));assert(!Font_TryFitName(FONT_SYSTEM,&s,999,NULL,&measured));
sFontWork=NULL;assert(!Font_TryFitName(FONT_SYSTEM,&s,999,&chosen,&measured));return 0;}
'''
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d);(p/'fit.c').write_text(source)
            c=subprocess.run([shutil.which('gcc'),'-std=c99','-O1',str(p/'fit.c'),'-o',str(p/'fit.exe')],capture_output=True,text=True,timeout=30)
            self.assertEqual(c.returncode,0,c.stderr)
            c=subprocess.run([str(p/'fit.exe')],capture_output=True,text=True,timeout=30);self.assertEqual(c.returncode,0,c.stderr)

if __name__=='__main__':unittest.main()
