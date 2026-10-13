import pathlib,subprocess,shutil,tempfile,unittest
ROOT = pathlib.Path(__file__).resolve().parents[2]
def function(source, name):
    import re
    match=re.search(r'(?:static )?(?:BOOL|void|u32) '+name+r'\([^;]+?\)\n\{',source)
    assert match,name
    depth=1;end=match.end()
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[match.start():end]

class FontColorPackageTests(unittest.TestCase):
 def run_c(self,source):
  with tempfile.TemporaryDirectory() as d:
   p=pathlib.Path(d);(p/'color.c').write_text(source)
   result=subprocess.run([shutil.which('gcc'),'-std=c99',str(p/'color.c'),'-o',str(p/'color.exe')],capture_output=True,text=True)
   self.assertEqual(result.returncode,0,result.stderr)
   result=subprocess.run([str(p/'color.exe')],capture_output=True,text=True)
   self.assertEqual(result.returncode,0,result.stderr)
 def test_color_adapter_preserves_original_and_rejects_controls(self):
  src='''#include <assert.h>
#include <stddef.h>
#include <string.h>
typedef unsigned int u32;typedef unsigned char u8;typedef unsigned short charcode_t;typedef int BOOL;
enum Font{FONT_SYSTEM,FONT_NARROW};
#define TRUE 1
#define FALSE 0
#define CHAR_FORMAT_ARG 0xfffe
#define CHAR_CONTROL_SET_COLOR 0xff00
#define MAX_LETTER_HEIGHT 16
#define TEXT_SPEED_NO_TRANSFER 255
#define GET_TEXT_FG_COLOR(c) (((c)>>16)&255)
typedef struct{u32 size;charcode_t chars[64];}String;typedef struct{u32 width,height;}Window;
static String scratch;static int allocations,frees,failAllocation,draws,markers;static const String*drawn;static enum Font drawnFont;
static String*String_Init(u32 length,u32 heap){assert(length<=64);if(failAllocation)return NULL;allocations++;memset(&scratch,0,sizeof scratch);return &scratch;}
static void String_Free(String*s){assert(s==&scratch);frees++;}
static void String_AppendChar(String*s,charcode_t c){assert(s->size<64);s->chars[s->size++]=c;}
static const charcode_t*String_GetData(const String*s){return s->chars;}
static u32 String_Length(const String*s){return s->size;}
static BOOL Font_TryFitName(enum Font preferred,const String*s,u32 budget,enum Font*out,u32*width){if(!s)return 0;for(u32 i=0;i<s->size;i++)if(!s->chars[i]||s->chars[i]>509)return 0;*out=FONT_SYSTEM;*width=s->size*6;if(*width<=budget)return 1;*out=FONT_NARROW;*width=s->size*4;return *width<=budget;}
static u32 Window_GetWidth(Window*w){return w->width;}static u32 Window_GetHeight(Window*w){return w->height;}
static void Text_AddPrinterWithParamsAndColor(Window*w,enum Font f,const String*s,u32 x,u32 y,int speed,u32 c,void*cb){assert(x<w->width*8&&y+16<=w->height*8);draws++;drawn=s;drawnFont=f;}
static void Window_FillRectWithColor(Window*w,u8 c,u32 x,u32 y,u32 a,u32 b){assert(x+a<=w->width*8&&y+b<=w->height*8);markers++;}
'''
  actual=(ROOT/'src/font.c').read_text()
  for name in ['Font_PrintName','Font_TryFitNameWithColor','Font_PrintNameWithColor']:src+=function(actual,name)+'\n'
  src+='''int main(void){String s={11,{0xfffe,0xff00,1,1,300,301,302,0xfffe,0xff00,1,0}};String saved=s;enum Font f;u32 width;Window w={11,2};assert(Font_TryFitNameWithColor(FONT_SYSTEM,&s,12,&f,&width,0)&&f==FONT_NARROW&&width==12);assert(memcmp(&s,&saved,sizeof s)==0&&allocations==frees);assert(Font_PrintNameWithColor(&w,FONT_SYSTEM,&s,76,0,100,1<<16,0));assert(drawn==&s&&drawnFont==FONT_NARROW&&draws==1);assert(!Font_PrintNameWithColor(&w,FONT_SYSTEM,&s,80,0,100,1<<16,0)&&markers==14&&draws==1);
for(int n=1;n<4;n++){String bad={0};bad.size=n;bad.chars[0]=0xfffe;assert(!Font_TryFitNameWithColor(FONT_SYSTEM,&bad,999,&f,&width,0));}
s.chars[1]=0xff01;assert(!Font_TryFitNameWithColor(FONT_SYSTEM,&s,999,&f,&width,0));s=saved;s.chars[2]=2;assert(!Font_TryFitNameWithColor(FONT_SYSTEM,&s,999,&f,&width,0));s=saved;s.chars[3]=7;assert(!Font_TryFitNameWithColor(FONT_SYSTEM,&s,999,&f,&width,0));s=saved;s.chars[5]=0xe000;assert(!Font_TryFitNameWithColor(FONT_SYSTEM,&s,999,&f,&width,0));s=saved;
String adjacent={8,{0xfffe,0xff00,1,1,0xfffe,0xff00,1,0}};assert(Font_TryFitNameWithColor(FONT_SYSTEM,&adjacent,0,&f,&width,0)&&width==0);assert(allocations==frees);failAllocation=1;assert(!Font_PrintNameWithColor(&w,FONT_SYSTEM,&s,0,0,88,1<<16,0)&&draws==1);failAllocation=0;assert(!Font_PrintNameWithColor(&w,FONT_SYSTEM,&s,0,1,88,1<<16,0));assert(allocations==frees);return 0;}
'''
  self.run_c(src)

if __name__=="__main__":unittest.main()
