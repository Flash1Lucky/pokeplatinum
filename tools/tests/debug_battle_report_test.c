// cc -std=c99 -Wall -Wextra -Werror -Iinclude tools/tests/debug_battle_report_test.c -o <test>
#include <assert.h>
#include <stdio.h>
#include "debug_battle_report.h"

int main(void)
{
    DebugBattleSummary summary = { 0 };
    DebugBattleResult normal = { 0 };
    DebugBattle_CountResult(&summary, &normal);
    assert(summary.completed == 0);
    normal.valid = normal.repeatMatched = 1;
    normal.damage = 28;
    normal.playerPP = 19;
    normal.opponentPP = 39;
    normal.turns = 1;
    normal.result = 5;
    normal.commands[0] = normal.commands[1] = 2;
    DebugBattle_CountResult(&summary, &normal);
    assert(summary.passed == 1 && summary.repeats == 1);

    DebugBattleResult probe = normal;
    probe.expectedFailures = probe.failures = DEBUG_BATTLE_FAIL_DAMAGE;
    DebugBattle_CountResult(&summary, &probe);
    assert(summary.probes == 1 && summary.failed == 0);
    probe.failures = 0; // Unexpected pass must fail the suite.
    DebugBattle_CountResult(&summary, &probe);
    probe.failures = DEBUG_BATTLE_FAIL_DAMAGE | DEBUG_BATTLE_FAIL_PP;
    DebugBattle_CountResult(&summary, &probe);
    assert(summary.failed == 2 && summary.probes == 1);

    DebugBattleResult repeat = normal;
    assert(DebugBattle_ResultsMatch(&normal, &repeat));
    repeat.damage++;
    assert(!DebugBattle_ResultsMatch(&normal, &repeat));
    repeat = normal;
    repeat.opponentPP--;
    assert(!DebugBattle_ResultsMatch(&normal, &repeat));
    repeat = normal;
    repeat.commands[1]++;
    assert(!DebugBattle_ResultsMatch(&normal, &repeat));
    repeat = normal;
    repeat.failures = DEBUG_BATTLE_FAIL_DAMAGE;
    assert(!DebugBattle_ResultsMatch(&normal, &repeat));
    repeat.failures |= DEBUG_BATTLE_FAIL_REPEAT;
    repeat.repeatMatched = 0;
    DebugBattle_CountResult(&summary, &repeat);
    assert(summary.failed == 3 && summary.completed == 5 && summary.repeats == 4);
    puts("battle reporting tests passed");
    return 0;
}
