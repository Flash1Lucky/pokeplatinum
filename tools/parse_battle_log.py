"""Validate BTEST1 harness records; emulator diagnostics alone never mean PASS."""
import argparse
import json
import re
from pathlib import Path

EXPECTED_DAMAGE = [28, 14, 56, 0, 42, 84, 23, 0]
FAIRY_EXPECTED_DAMAGE = EXPECTED_DAMAGE + [54, 108, 27, 81, 0, 38, 74, 20]


def fields(line, kind, keys):
    match = re.fullmatch('BTEST1 ' + kind + ''.join(' ' + k + r'=(\d+)' for k in keys), line)
    if not match:
        raise ValueError('Malformed ' + kind + ' record')
    return dict(zip(keys, map(int, match.groups())))


def parse(text):
    suites = []
    current = None
    for line in text.splitlines():
        if not line.startswith('BTEST1 '):
            continue
        if line.startswith('BTEST1 BEGIN '):
            if current is not None:
                raise ValueError('Previous suite incomplete')
            begin = fields(line, 'BEGIN', ['fixture', 'cases', 'group'] if line.startswith('BTEST1 BEGIN fixture=5 ') else ['fixture', 'cases'])
            group = 'all'
            first_id = 1
            if begin == {'fixture': 3, 'cases': 8}:
                targets = EXPECTED_DAMAGE
            elif begin in ({'fixture': 4, 'cases': 16}, {'fixture': 5, 'cases': 16, 'group': 0}):
                targets = FAIRY_EXPECTED_DAMAGE
            elif begin == {'fixture': 5, 'cases': 8, 'group': 1}:
                targets = FAIRY_EXPECTED_DAMAGE
                first_id = 9
                group = 'fairy'
            else:
                raise ValueError('Unsupported fixture; review parser and expectations')
            current = []
        elif line.startswith('BTEST1 CASE '):
            if current is None:
                raise ValueError('CASE without BEGIN')
            case = fields(line, 'CASE', ['id', 'fail', 'expected', 'repeat', 'damage', 'target'])
            case_id = len(current) + first_id
            if case_id > len(targets) or case['id'] != case_id:
                raise ValueError('Duplicate, missing, or unordered case')
            if case['expected'] != (8 if case_id == 8 else 0) or case['target'] != targets[case_id - 1]:
                raise ValueError('Unexpected fixture expectations')
            case['passed'] = case['fail'] == case['expected'] and case['repeat'] == 1
            if case_id != 8 and case['damage'] != case['target']:
                case['passed'] = False
            if case_id == 8 and case['damage'] != 28:
                case['passed'] = False
            current.append(case)
        elif line.startswith('BTEST1 END '):
            if current is None or len(current) != begin['cases']:
                raise ValueError('END without all expected cases')
            summary = fields(line, 'END', ['completed', 'pass', 'probe', 'fail', 'repeat'])
            counts = {'completed': begin['cases'], 'pass': 0, 'probe': 0, 'fail': 0,
                      'repeat': sum(c['repeat'] == 1 for c in current)}
            for case in current:
                key = 'fail' if case['fail'] != case['expected'] else ('probe' if case['expected'] else 'pass')
                counts[key] += 1
            if summary != counts:
                raise ValueError('Summary disagrees with case records')
            suites.append({'status': 'PASS' if all(c['passed'] for c in current) else 'FAIL', 'summary': summary, 'cases': current, 'fixture': begin['fixture'], 'group': group})
            current = None
        else:
            raise ValueError('Unknown or error BTEST1 record')
    if current is not None or not suites:
        raise ValueError('No complete suite / missing completion marker')
    return {'status': 'PASS' if all(s['status'] == 'PASS' for s in suites) else 'FAIL', 'suites': suites}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('log', type=Path)
    args = ap.parse_args()
    try:
        result = parse(args.log.read_text(encoding='utf-8', errors='replace'))
    except (ValueError, OSError) as error:
        print(json.dumps({'status': 'INCOMPLETE_OR_INVALID', 'reason': str(error)}))
        return 2
    print(json.dumps(result, indent=2))
    return 0 if result['status'] == 'PASS' else 1


if __name__ == '__main__':
    raise SystemExit(main())
