"""Synthetic protocol validation only; these are not emulator results."""
import unittest
from parse_battle_log import parse

def fixture():
    targets=[28,14,56,0,42,84,23,0,54,108,27,81,0,38,74,20]
    lines=['BTEST1 BEGIN fixture=4 cases=16']
    for i,target in enumerate(targets,1):
        mask=8 if i==8 else 0
        damage=28 if i==8 else target
        lines.append(f'BTEST1 CASE id={i} fail={mask} expected={mask} repeat=1 damage={damage} target={target}')
    lines.append('BTEST1 END completed=16 pass=15 probe=1 fail=0 repeat=16')
    return '\n'.join(lines)

class FairyProtocolTests(unittest.TestCase):
    def test_fixture_v4(self):
        self.assertEqual(parse(fixture())['status'],'PASS')
    def test_wrong_fairy_damage_fails_even_if_failure_mask_lies(self):
        self.assertEqual(parse(fixture().replace('damage=108 target=108','damage=54 target=108'))['status'],'FAIL')
    def test_missing_fairy_cases_rejected(self):
        with self.assertRaises(ValueError): parse('\n'.join(fixture().splitlines()[:9])+ '\nBTEST1 END completed=8 pass=7 probe=1 fail=0 repeat=8')
    def test_changed_oracle_rejected(self):
        with self.assertRaises(ValueError): parse(fixture().replace('target=81','target=54'))

if __name__=='__main__': unittest.main()
