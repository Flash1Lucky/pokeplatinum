#ifndef POKEPLATINUM_DEBUG_BATTLE_H
#define POKEPLATINUM_DEBUG_BATTLE_H

#include "field/field_system_decl.h"
#include "debug_battle_report.h"

#define DEBUG_BATTLE_FIXTURE_VERSION 5
#define DEBUG_BATTLE_SEED            0x504C4154
#define DEBUG_BATTLE_CASE_COUNT      16

enum DebugBattleGroup {
    DEBUG_BATTLE_GROUP_ALL,
    DEBUG_BATTLE_GROUP_FAIRY,
};

BOOL DebugBattle_Start(FieldSystem *fieldSystem);
BOOL DebugBattle_StartGroup(FieldSystem *fieldSystem, enum DebugBattleGroup group);
int DebugBattle_GetFirstCase(void);
int DebugBattle_GetSelectedCount(void);
const DebugBattleResult *DebugBattle_GetResult(int index);
BOOL DebugBattle_IsActive(void);
void DebugBattle_RecordCommand(int battler);
int DebugBattle_DamagePercent(int randomPercent);
DebugBattleSummary DebugBattle_GetSummary(void);
u32 DebugBattle_GetCaseName(int index);
u32 DebugBattle_GetExpectedDamage(int index);

#endif
