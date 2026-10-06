"""Protocol regressions for explicit v5 group selection; not runtime evidence."""
import unittest
from parse_battle_log import parse
from test_fairy_battle_log import fixture

def fairy():
    lines=fixture().splitlines()
    return '\n'.join(['BTEST1 BEGIN fixture=5 cases=8 group=1']+lines[9:17]+['BTEST1 END completed=8 pass=8 probe=0 fail=0 repeat=8'])

class GroupTests(unittest.TestCase):
    def test_all_preserves_baseline_and_probe(self):
        result=parse(fixture().replace('fixture=4 cases=16','fixture=5 cases=16 group=0'))
        self.assertEqual(result['status'],'PASS')
        self.assertEqual(result['suites'][0]['group'],'all')
        self.assertEqual(result['suites'][0]['summary']['probe'],1)
    def test_fairy_keeps_original_case_ids(self):
        result=parse(fairy())
        self.assertEqual(result['status'],'PASS')
        self.assertEqual(result['suites'][0]['group'],'fairy')
        self.assertEqual([c['id'] for c in result['suites'][0]['cases']],list(range(9,17)))
    def test_wrong_group_total_rejected(self):
        with self.assertRaises(ValueError):parse(fairy().replace('cases=8 group=1','cases=16 group=1'))
    def test_baseline_case_in_fairy_group_rejected(self):
        with self.assertRaises(ValueError):parse(fairy().replace('id=9 ','id=1 '))
    def test_fairy_failure_not_hidden(self):
        result=parse(fairy().replace('damage=54 target=54','damage=46 target=54'))
        self.assertEqual(result['status'],'FAIL')
    def test_missing_end_rejected(self):
        with self.assertRaises(ValueError):parse('\n'.join(fairy().splitlines()[:-1]))
    def test_multiple_groups_identified(self):
        text=fixture().replace('fixture=4 cases=16','fixture=5 cases=16 group=0')+'\n'+fairy()
        self.assertEqual([s['group'] for s in parse(text)['suites']],['all','fairy'])

if __name__=='__main__':unittest.main()
