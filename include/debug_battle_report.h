#ifndef POKEPLATINUM_DEBUG_BATTLE_REPORT_H
#define POKEPLATINUM_DEBUG_BATTLE_REPORT_H

// No SDK dependencies: these same reporting rules are tested on the host.
#define DEBUG_BATTLE_FAIL_EXIT    (1 << 0)
#define DEBUG_BATTLE_FAIL_TURNS   (1 << 1)
#define DEBUG_BATTLE_FAIL_PLAYER  (1 << 2)
#define DEBUG_BATTLE_FAIL_DAMAGE  (1 << 3)
#define DEBUG_BATTLE_FAIL_PP      (1 << 4)
#define DEBUG_BATTLE_FAIL_SCRIPT  (1 << 5)
#define DEBUG_BATTLE_FAIL_LAUNCH  (1 << 6)
#define DEBUG_BATTLE_FAIL_REPEAT  (1 << 7)

typedef struct DebugBattleResult {
    unsigned valid;
    unsigned result;
    unsigned turns;
    unsigned playerHP;
    unsigned opponentHP;
    unsigned failures;
    unsigned expectedFailures;
    unsigned damage;
    unsigned playerPP;
    unsigned opponentPP;
    unsigned commands[2];
    unsigned repeatMatched;
} DebugBattleResult;

typedef struct DebugBattleSummary {
    unsigned completed;
    unsigned passed;
    unsigned probes;
    unsigned failed;
    unsigned repeats;
} DebugBattleSummary;

static inline int DebugBattle_ResultsMatch(const DebugBattleResult *a, const DebugBattleResult *b)
{
    return a->result == b->result && a->turns == b->turns
        && a->playerHP == b->playerHP && a->opponentHP == b->opponentHP
        && a->damage == b->damage && a->playerPP == b->playerPP
        && a->opponentPP == b->opponentPP && a->failures == b->failures
        && a->commands[0] == b->commands[0] && a->commands[1] == b->commands[1];
}

static inline void DebugBattle_CountResult(DebugBattleSummary *summary, const DebugBattleResult *result)
{
    if (!result->valid) return;
    summary->completed++;
    if (result->repeatMatched) summary->repeats++;
    if (result->failures != result->expectedFailures) summary->failed++;
    else if (result->expectedFailures) summary->probes++;
    else summary->passed++;
}

#endif
