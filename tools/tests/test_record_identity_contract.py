"""Portable compact identity encoding contract; no live mon/save integration."""
import pathlib,subprocess,tempfile,shutil,unittest
ROOT=pathlib.Path(__file__).resolve().parents[2]
class RecordIdentityContract(unittest.TestCase):
 def test_slots_forms_and_wide_resolution_are_atomic(self):
  source=r"""
#include <assert.h>
#include <string.h>
#include "roster_record.h"
int main(void){
 unsigned char a[32],b[32],aa[32],bb[32];unsigned short abilities[3]={1,300,65535},result;
 for(unsigned slot=0;slot<3;slot++)for(unsigned form=0;form<65536;form++){
  memset(a,0xA5,sizeof a);memset(b,0xA5,sizeof b);assert(RosterRecord_SetIdentity(a,b,slot,form));
  assert(RosterRecord_Validate(a,b));assert(RosterRecord_GetSlot(a)==slot);assert(RosterRecord_GetForm(b)==form);
  assert((b[ROSTER_RECORD_LEGACY_FORM_OFFSET]&7)==5);result=0;
  assert(RosterRecord_ResolveAbility(slot,abilities,&result));assert(result==abilities[slot]);
 }
 memcpy(aa,a,32);memcpy(bb,b,32);assert(!RosterRecord_SetIdentity(a,b,3,1));assert(!memcmp(aa,a,32)&&!memcmp(bb,b,32));
 assert(!RosterRecord_SetIdentity(a,b,1,65536));assert(!memcmp(aa,a,32)&&!memcmp(bb,b,32));
 abilities[2]=0;result=777;assert(!RosterRecord_ResolveAbility(2,abilities,&result));assert(result==777);
 assert(!RosterRecord_ResolveAbility(3,abilities,&result));a[ROSTER_RECORD_ABILITY_SLOT_OFFSET]=3;assert(!RosterRecord_Validate(a,b));
 a[ROSTER_RECORD_ABILITY_SLOT_OFFSET]=0;b[ROSTER_RECORD_LEGACY_FORM_OFFSET]|=8;assert(!RosterRecord_Validate(a,b));return 0;
}
"""
  with tempfile.TemporaryDirectory() as d:
   p=pathlib.Path(d);(p/'identity.c').write_text(source)
   c=subprocess.run([shutil.which('gcc'),'-std=c99','-I'+str(ROOT/'include'),str(p/'identity.c'),'-o',str(p/'identity.exe')],capture_output=True,text=True)
   self.assertEqual(c.returncode,0,c.stderr)
   c=subprocess.run([str(p/'identity.exe')],capture_output=True,text=True);self.assertEqual(c.returncode,0,c.stderr)
if __name__=='__main__':unittest.main()
