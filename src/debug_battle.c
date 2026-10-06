#include "debug_battle.h"

#include "battle_tests_config.h"
#include "constants/heap.h"
#include "constants/narc.h"
#include "constants/overworld_weather.h"
#include "generated/abilities.h"
#include "generated/genders.h"
#include "generated/items.h"
#include "generated/moves.h"
#include "generated/species.h"
#include "generated/text_banks.h"

#include "field/field_system.h"

#include "bag.h"
#include "enc_effects.h"
#include "field_battle_data_transfer.h"
#include "field_system.h"
#include "field_task.h"
#include "field_transition.h"
#include "game_records.h"
#include "heap.h"
#include "map_header.h"
#include "map_object.h"
#include "math_util.h"
#include "message.h"
#include "move_table.h"
#include "party.h"
#include "pokedex.h"
#include "pokemon.h"
#include "poketch.h"
#include "string_gf.h"
#include "trainer_info.h"
#include "unk_0203D1B8.h"

#include "res/text/bank/battle_tests.h"


// Opt-in emulator output only. No save writes or general game logging.
// melonDS 1.1 consumes up to 120 bytes synchronously at this no$gba register.
#if BATTLE_TESTS_LOG_ENABLED
static void DebugBattle_Log(const char *format, ...)
{
    // The emulator reads main RAM directly; do not expose a possible DTCM stack buffer.
    // Calls are confined to the serial field task, never an interrupt.
    static char line[120];
    va_list args;
    va_start(args, format);
    int length = OS_VSNPrintf(line, sizeof(line), format, args);
    va_end(args);
    if (length < 0 || length >= sizeof(line)) {
        *(const char *volatile *)0x04FFFA18 = "BTEST1 ERROR truncated";
        return;
    }
    *(const char *volatile *)0x04FFFA18 = line;
}
#else
#define DebugBattle_Log(...)
#endif

typedef struct DebugBattleEnv {
    FieldBattleDTO *dto;
    GameRecords records;
    Poketch poketch;
    BagCursor bagCursor;
    u8 subscreenCursor;
    u32 fieldSeed;
    int test;
    u32 playerHP;
    u32 opponentHP;
    u32 playerPP;
    u32 opponentPP;
    int repeat;
    DebugBattleResult firstRun;
} DebugBattleEnv;

// Resident in main: the callback must survive unloading the field overlay.
static DebugBattleResult sResults[DEBUG_BATTLE_CASE_COUNT];
static BOOL sActive;
static int sFirstCase;
static int sSelectedCount = DEBUG_BATTLE_CASE_COUNT;
static u32 sCommands[2];
static int sDamagePercent;

typedef struct DebugBattleCase {
    u16 playerSpecies;
    u16 opponentSpecies;
    u16 move;
    u16 damagePercent;
    u16 expectedDamage;
    u16 name;
    u32 seed;
    u32 expectedFailures;
    u16 heldItem;
    u16 ability;
} DebugBattleCase;

// Level 50, power 60, SpAtk = SpDef = 100: floor(22 * 60 / 50) + 2 = 28.
// Expectations are constants, never derived from the engine's type chart.
static const DebugBattleCase sCases[DEBUG_BATTLE_CASE_COUNT] = {
    { SPECIES_ABRA, SPECIES_SNORLAX, MOVE_SHOCK_WAVE, 100, 28, BattleTests_Text_TestNeutral, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_ABRA, SPECIES_BULBASAUR, MOVE_SHOCK_WAVE, 100, 14, BattleTests_Text_TestResisted, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_ABRA, SPECIES_SQUIRTLE, MOVE_SHOCK_WAVE, 100, 56, BattleTests_Text_TestSuper, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_ABRA, SPECIES_GEODUDE, MOVE_SHOCK_WAVE, 100, 0, BattleTests_Text_TestImmune, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_PIKACHU, SPECIES_SNORLAX, MOVE_SHOCK_WAVE, 100, 42, BattleTests_Text_TestStab, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_PIKACHU, SPECIES_SQUIRTLE, MOVE_SHOCK_WAVE, 100, 84, BattleTests_Text_TestStabSuper, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_ABRA, SPECIES_SNORLAX, MOVE_SHOCK_WAVE, 85, 23, BattleTests_Text_TestLowRoll, DEBUG_BATTLE_SEED, 0 },
    { SPECIES_ABRA, SPECIES_SNORLAX, MOVE_SHOCK_WAVE, 100, 0, BattleTests_Text_TestProbe, DEBUG_BATTLE_SEED, DEBUG_BATTLE_FAIL_DAMAGE },
    { SPECIES_ABRA, SPECIES_SNORLAX, MOVE_JUDGMENT, 100, 54, BattleTests_Text_FairyNeutral, DEBUG_BATTLE_SEED, 0, ITEM_PIXIE_PLATE, ABILITY_NONE },
    { SPECIES_ABRA, SPECIES_DRATINI, MOVE_JUDGMENT, 100, 108, BattleTests_Text_FairySuper, DEBUG_BATTLE_SEED, 0, ITEM_PIXIE_PLATE, ABILITY_NONE },
    { SPECIES_ABRA, SPECIES_PONYTA, MOVE_JUDGMENT, 100, 27, BattleTests_Text_FairyResisted, DEBUG_BATTLE_SEED, 0, ITEM_PIXIE_PLATE, ABILITY_NONE },
    { SPECIES_ARCEUS, SPECIES_SNORLAX, MOVE_JUDGMENT, 100, 81, BattleTests_Text_FairyArceus, DEBUG_BATTLE_SEED, 0, ITEM_PIXIE_PLATE, ABILITY_MULTITYPE },
    { SPECIES_ABRA, SPECIES_CLEFAIRY, MOVE_DRAGON_BREATH, 100, 0, BattleTests_Text_FairyImmune, DEBUG_BATTLE_SEED, 0, ITEM_NONE, ABILITY_NONE },
    { SPECIES_ABRA, SPECIES_CLEFAIRY, MOVE_ACID, 100, 38, BattleTests_Text_FairyPoison, DEBUG_BATTLE_SEED, 0, ITEM_NONE, ABILITY_NONE },
    { SPECIES_ABRA, SPECIES_CLEFAIRY, MOVE_FLASH_CANNON, 100, 74, BattleTests_Text_FairySteel, DEBUG_BATTLE_SEED, 0, ITEM_NONE, ABILITY_NONE },
    { SPECIES_ABRA, SPECIES_CLEFAIRY, MOVE_AURA_SPHERE, 100, 20, BattleTests_Text_FairyFighting, DEBUG_BATTLE_SEED, 0, ITEM_NONE, ABILITY_NONE },
};
static BOOL DebugBattle_Task(FieldTask *task);

static void DebugBattle_AddMon(FieldBattleDTO *dto, int battler, int species, u32 personality, int ability, u16 move, u16 heldItem)
{
    Pokemon *mon = Pokemon_New(HEAP_ID_FIELD2);
    Pokemon_InitWith(mon, species, 50, 20, TRUE, personality, OTID_SET, 0x1234);
    u32 zero = 0;
    for (int i = 0; i < LEARNED_MOVES_MAX; i++) {
        u16 selectedMove = i == 0 ? move : MOVE_NONE;
        u8 pp = i == 0 ? MoveTable_LoadParam(move, MOVEATTRIBUTE_PP) : 0;
        Pokemon_SetValue(mon, MON_DATA_MOVE1 + i, &selectedMove);
        Pokemon_SetValue(mon, MON_DATA_MOVE1_PP_UPS + i, &zero);
        Pokemon_SetValue(mon, MON_DATA_MOVE1_PP + i, &pp);
    }
    Pokemon_SetValue(mon, MON_DATA_ABILITY, &ability);
    Pokemon_SetValue(mon, MON_DATA_HELD_ITEM, &zero);
    Pokemon_SetValue(mon, MON_DATA_STATUS, &zero);
    Pokemon_SetValue(mon, MON_DATA_FORM, &zero);
    Pokemon_SetValue(mon, MON_DATA_OT_GENDER, &zero);
    Pokemon_SetValue(mon, MON_DATA_OT_NAME, dto->trainerInfo[0]->name);
    Pokemon_SetValue(mon, MON_DATA_HELD_ITEM, &heldItem);
    Pokemon_SetArceusForm(mon);
    Pokemon_CalcLevelAndStats(mon);
    // Normalize battle stats independently of species so type changes are the
    // only relevant difference. These are temporary party copies, never saved.
    u16 stat = 100;
    Pokemon_SetValue(mon, MON_DATA_ATK, &stat);
    Pokemon_SetValue(mon, MON_DATA_DEF, &stat);
    Pokemon_SetValue(mon, MON_DATA_SP_ATK, &stat);
    Pokemon_SetValue(mon, MON_DATA_SP_DEF, &stat);
    stat = battler == BATTLER_PLAYER_1 ? 200 : 50;
    Pokemon_SetValue(mon, MON_DATA_SPEED, &stat);
    u16 hp = 300;
    Pokemon_SetValue(mon, MON_DATA_MAX_HP, &hp);
    Pokemon_SetValue(mon, MON_DATA_HP, &hp);
    Party_Init(dto->parties[battler]);
    FieldBattleDTO_AddPokemonToBattler(dto, mon, battler);
    Heap_Free(mon);
}

BOOL DebugBattle_Start(FieldSystem *fieldSystem)
{
    return DebugBattle_StartGroup(fieldSystem, DEBUG_BATTLE_GROUP_ALL);
}

int DebugBattle_GetFirstCase(void) { return sFirstCase; }
int DebugBattle_GetSelectedCount(void) { return sSelectedCount; }

BOOL DebugBattle_StartGroup(FieldSystem *fieldSystem, enum DebugBattleGroup group)
{
    if (!BATTLE_TESTS_BUILD_ENABLED || sActive || FieldSystem_IsRunningTask(fieldSystem)
        || FieldSystem_HasChildProcess(fieldSystem)
        || !FieldSystem_IsRunningFieldMap(fieldSystem)) {
        if (!sActive) {
            MI_CpuClear8(sResults, sizeof(sResults));
            sResults[0].valid = TRUE;
            sResults[0].failures = DEBUG_BATTLE_FAIL_LAUNCH;
        }
        return FALSE;
    }
    if (group != DEBUG_BATTLE_GROUP_ALL && group != DEBUG_BATTLE_GROUP_FAIRY) return FALSE;
    sFirstCase = group == DEBUG_BATTLE_GROUP_FAIRY ? 8 : 0;
    sSelectedCount = DEBUG_BATTLE_CASE_COUNT - sFirstCase;
    DebugBattleEnv *env = Heap_Alloc(HEAP_ID_FIELD2, sizeof(DebugBattleEnv));
    MI_CpuClear8(env, sizeof(DebugBattleEnv));
    env->test = sFirstCase;
    env->fieldSeed = LCRNG_GetSeed();
    env->dto = FieldBattleDTO_New(HEAP_ID_FIELD2, BATTLE_TYPE_WILD_MON | BATTLE_TYPE_DEBUG);
    FieldBattleDTO *dto = env->dto;
    FieldBattleDTO_Init(dto, fieldSystem);

    // These are normally aliases into saved data. Give the battle private copies.
    GameRecords_Init(&env->records);
    Poketch_Init(&env->poketch);
    dto->records = &env->records;
    dto->poketch = &env->poketch;
    dto->bagCursor = &env->bagCursor;
    dto->subscreenCursorOn = &env->subscreenCursor;
    dto->journalEntry = NULL;
    Bag_Init(dto->bag); // No balls: this fixture cannot write captured Pokemon to PC storage.
    Pokedex_Init(dto->pokedex);
    Options_Init(dto->options);
    Options_SetTextSpeed(dto->options, OPTIONS_TEXT_SPEED_FAST);
    Options_SetBattleStyle(dto->options, OPTIONS_BATTLE_STYLE_SET);

    TrainerInfo *trainer = dto->trainerInfo[BATTLER_PLAYER_1];
    TrainerInfo_Init(trainer);
    TrainerInfo_SetID(trainer, 0x1234);
    TrainerInfo_SetGender(trainer, GENDER_MALE);
    trainer->badgeMask = 0xFF;
    MessageLoader *loader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_BATTLE_TESTS, HEAP_ID_FIELD2);
    String *name = MessageLoader_GetNewString(loader, BattleTests_Text_PlayerName);
    TrainerInfo_SetNameFromString(trainer, name);
    String_Free(name);
    MessageLoader_Free(loader);
    FieldBattleDTO_CopyPlayerInfoToTrainerData(dto);

    DebugBattle_AddMon(dto, BATTLER_PLAYER_1, sCases[sFirstCase].playerSpecies, 0, sCases[sFirstCase].ability, sCases[sFirstCase].move, sCases[sFirstCase].heldItem);
    DebugBattle_AddMon(dto, BATTLER_ENEMY_1, sCases[sFirstCase].opponentSpecies, 25, ABILITY_BATTLE_ARMOR, MOVE_SPLASH, ITEM_NONE);
    dto->background = BACKGROUND_PLAIN;
    dto->terrain = TERRAIN_PLAIN;
    dto->fieldWeather = OVERWORLD_WEATHER_CLEAR;
    dto->timeOfDay = TIMEOFDAY_DAY;
    dto->mapHeaderID = MAP_HEADER_ROUTE_201;
    dto->mapLabelTextID = MapHeader_GetMapLabelTextID(MAP_HEADER_ROUTE_201);
    dto->mapEvolutionMethod = 0;
    dto->seed = DEBUG_BATTLE_SEED;
    MI_CpuClear8(sResults, sizeof(sResults));
    DebugBattle_Log("BTEST1 BEGIN fixture=%u cases=%u group=%u", DEBUG_BATTLE_FIXTURE_VERSION, sSelectedCount, group);
    FieldSystem_CreateTask(fieldSystem, DebugBattle_Task, env);
    return TRUE;
}

static BOOL DebugBattle_Task(FieldTask *task)
{
    FieldSystem *fieldSystem = FieldTask_GetFieldSystem(task);
    DebugBattleEnv *env = FieldTask_GetEnv(task);
    int *state = FieldTask_GetState(task);
    switch (*state) {
    case 0:
        MapObjectMan_PauseAllMovement(fieldSystem->mapObjMan);
        FieldTransition_StartEncounterEffect(task, EncEffects_CutInEffect(env->dto), EncEffects_BGM(env->dto));
        (*state)++;
        break;
    case 1:
        FieldTransition_FinishMap(task);
        (*state)++;
        break;
    case 2:
        // Rebuild both parties between cases; never inherit HP, PP or status.
        DebugBattle_AddMon(env->dto, BATTLER_PLAYER_1, sCases[env->test].playerSpecies, 0, sCases[env->test].ability, sCases[env->test].move, sCases[env->test].heldItem);
        DebugBattle_AddMon(env->dto, BATTLER_ENEMY_1, sCases[env->test].opponentSpecies, 25, ABILITY_BATTLE_ARMOR, MOVE_SPLASH, ITEM_NONE);
        env->playerHP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[0], 0), MON_DATA_HP, NULL);
        env->opponentHP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[1], 0), MON_DATA_HP, NULL);
        env->playerPP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[0], 0), MON_DATA_MOVE1_PP, NULL);
        env->opponentPP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[1], 0), MON_DATA_MOVE1_PP, NULL);
        env->dto->resultMask = BATTLE_IN_PROGRESS;
        env->dto->totalTurnsElapsed = 0;
        MI_CpuClear8(sCommands, sizeof(sCommands));
        sActive = TRUE;
        sDamagePercent = sCases[env->test].damagePercent;
        // Reset immediately before starting the battle, after the field transition.
        LCRNG_SetSeed(sCases[env->test].seed);
        env->dto->seed = sCases[env->test].seed;
        FieldSystem_StartBattleProcess(fieldSystem, env->dto);
        (*state)++;
        break;
    case 3:
        if (FieldSystem_HasChildProcess(fieldSystem)) {
            break;
        }
        sActive = FALSE;
        DebugBattleResult *result = &sResults[env->test];
        MI_CpuClear8(result, sizeof(*result));
        const DebugBattleCase *test = &sCases[env->test];
        result->result = env->dto->resultMask;
        result->turns = env->dto->totalTurnsElapsed;
        result->playerHP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[0], 0), MON_DATA_HP, NULL);
        result->opponentHP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[1], 0), MON_DATA_HP, NULL);
        result->playerPP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[0], 0), MON_DATA_MOVE1_PP, NULL);
        result->opponentPP = Pokemon_GetValue(Party_GetPokemonBySlotIndex(env->dto->parties[1], 0), MON_DATA_MOVE1_PP, NULL);
        result->commands[0] = sCommands[0];
        result->commands[1] = sCommands[1];
        result->damage = env->opponentHP - result->opponentHP;
        result->expectedFailures = test->expectedFailures;
        if (result->result != BATTLE_RESULT_PLAYER_FLED) result->failures |= DEBUG_BATTLE_FAIL_EXIT;
        if (result->turns != 1) result->failures |= DEBUG_BATTLE_FAIL_TURNS;
        if (result->playerHP != env->playerHP) result->failures |= DEBUG_BATTLE_FAIL_PLAYER;
        if (result->damage != test->expectedDamage) result->failures |= DEBUG_BATTLE_FAIL_DAMAGE;
        if (result->playerPP + 1 != env->playerPP || result->opponentPP + 1 != env->opponentPP) result->failures |= DEBUG_BATTLE_FAIL_PP;
        if (sCommands[0] != 2 || sCommands[1] != 2) result->failures |= DEBUG_BATTLE_FAIL_SCRIPT;
        if (env->repeat == 0) {
            env->firstRun = *result;
            env->repeat = 1;
            *state = 2;
            break;
        }
        result->repeatMatched = DebugBattle_ResultsMatch(&env->firstRun, result);
        result->failures |= env->firstRun.failures;
        if (!result->repeatMatched) result->failures |= DEBUG_BATTLE_FAIL_REPEAT;
        result->valid = TRUE;
        DebugBattle_Log("BTEST1 CASE id=%u fail=%u expected=%u repeat=%u damage=%u target=%u",
            env->test + 1, result->failures, result->expectedFailures,
            result->repeatMatched, result->damage, test->expectedDamage);
        env->repeat = 0;
        if (++env->test < DEBUG_BATTLE_CASE_COUNT) {
            *state = 2;
            break;
        }
        // Deliberately omit UpdateFieldSystem: party, items, and progression stay outside the fixture.
        FieldTransition_StartMap(task);
        (*state)++;
        break;
    case 4:
        MapObjectMan_UnpauseAllMovement(fieldSystem->mapObjMan);
        FieldTransition_FadeIn(task);
        (*state)++;
        break;
    case 5:
        LCRNG_SetSeed(env->fieldSeed);
        FieldBattleDTO_Free(env->dto);
        Heap_Free(env);
        FieldSystem_ResumeProcessing();
#if BATTLE_TESTS_LOG_ENABLED
        DebugBattleSummary summary = DebugBattle_GetSummary();
        DebugBattle_Log("BTEST1 END completed=%u pass=%u probe=%u fail=%u repeat=%u",
            summary.completed, summary.passed, summary.probes, summary.failed, summary.repeats);
#endif
        return TRUE;
    }
    return FALSE;
}

const DebugBattleResult *DebugBattle_GetResult(int index)
{
    return &sResults[index >= 0 && index < DEBUG_BATTLE_CASE_COUNT ? index : 0];
}

BOOL DebugBattle_IsActive(void)
{
    return BATTLE_TESTS_BUILD_ENABLED && sActive;
}

void DebugBattle_RecordCommand(int battler)
{
    if (battler < 2) sCommands[battler]++;
}

int DebugBattle_DamagePercent(int randomPercent)
{
    return DebugBattle_IsActive() ? sDamagePercent : randomPercent;
}

DebugBattleSummary DebugBattle_GetSummary(void)
{
    DebugBattleSummary summary = { 0 };
    for (int i = 0; i < DEBUG_BATTLE_CASE_COUNT; i++) {
        DebugBattle_CountResult(&summary, &sResults[i]);
    }
    return summary;
}

u32 DebugBattle_GetCaseName(int index)
{
    return sCases[index].name;
}

u32 DebugBattle_GetExpectedDamage(int index)
{
    return sCases[index].expectedDamage;
}
