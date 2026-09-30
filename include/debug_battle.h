#ifndef POKEPLATINUM_DEBUG_BATTLE_H
#define POKEPLATINUM_DEBUG_BATTLE_H

#include "field/field_system_decl.h"
#include "debug_battle_report.h"

#define DEBUG_BATTLE_FIXTURE_VERSION 3
#define DEBUG_BATTLE_SEED            0x504C4154
#define DEBUG_BATTLE_CASE_COUNT      8

BOOL DebugBattle_Start(FieldSystem *fieldSystem);
const DebugBattleResult *DebugBattle_GetResult(int index);
BOOL DebugBattle_IsActive(void);
void DebugBattle_RecordCommand(int battler);
int DebugBattle_DamagePercent(int randomPercent);
DebugBattleSummary DebugBattle_GetSummary(void);
u32 DebugBattle_GetCaseName(int index);
u32 DebugBattle_GetExpectedDamage(int index);

#endif
