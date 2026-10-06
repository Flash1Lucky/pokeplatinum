"""Synthetic protocol tests, NOT emulator evidence."""
import unittest
from parse_battle_log import parse, EXPECTED_DAMAGE


def fixture():
    rows = ['emulator startup diagnostic', 'BTEST1 BEGIN fixture=3 cases=8']
    for i, target in enumerate(EXPECTED_DAMAGE, 1):
        mask = 8 if i == 8 else 0
        damage = 28 if i == 8 else target
        rows.append(f'BTEST1 CASE id={i} fail={mask} expected={mask} repeat=1 damage={damage} target={target}')
    rows.append('BTEST1 END completed=8 pass=7 probe=1 fail=0 repeat=8')
    return '\n'.join(rows)


class ProtocolTests(unittest.TestCase):
    def test_success_with_expected_failure_probe(self):
        self.assertEqual(parse(fixture())['status'], 'PASS')

    def test_emulator_diagnostic_is_not_test_success(self):
        with self.assertRaises(ValueError): parse('Game is now booting\n')

    def test_truncated_suite(self):
        with self.assertRaises(ValueError): parse(fixture().rsplit('\n', 1)[0])

    def test_extra_failure_on_probe_is_failure(self):
        data = fixture().replace('id=8 fail=8', 'id=8 fail=9').replace('pass=7 probe=1 fail=0', 'pass=7 probe=0 fail=1')
        self.assertEqual(parse(data)['status'], 'FAIL')

    def test_probe_unexpectedly_passes(self):
        data = fixture().replace('id=8 fail=8', 'id=8 fail=0').replace('pass=7 probe=1 fail=0', 'pass=7 probe=0 fail=1')
        self.assertEqual(parse(data)['status'], 'FAIL')

    def test_repeat_mismatch_is_failure(self):
        data = fixture().replace('id=1 fail=0 expected=0 repeat=1', 'id=1 fail=128 expected=0 repeat=0').replace('pass=7 probe=1 fail=0 repeat=8', 'pass=6 probe=1 fail=1 repeat=7')
        self.assertEqual(parse(data)['status'], 'FAIL')

    def test_duplicate_and_summary_mismatch(self):
        for data in [fixture().replace('id=2 ', 'id=1 '), fixture().replace('pass=7 ', 'pass=8 ')]:
            with self.assertRaises(ValueError): parse(data)

    def test_second_suite_incomplete(self):
        with self.assertRaises(ValueError): parse(fixture() + '\nBTEST1 BEGIN fixture=3 cases=8')


if __name__ == '__main__': unittest.main()
