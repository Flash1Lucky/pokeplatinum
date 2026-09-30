#include "overlay005/debug_battle_menu.h"
#include "constants/heap.h"
#include "constants/charcode.h"
#include "constants/narc.h"
#include "generated/text_banks.h"
#include "field/field_system.h"
#include "battle_tests_config.h"
#include "debug_battle.h"
#include "field_system.h"
#include "font.h"
#include "heap.h"
#include "message.h"
#include "render_window.h"
#include "string_gf.h"
#include "string_template.h"
#include "sys_task.h"
#include "system.h"
#include "text.h"
#include "res/text/bank/battle_tests.h"

#define DEBUG_COLOR_WHITE 15
#define DEBUG_TEXT_BLACK TEXT_COLOR(1, 2, 15)

typedef struct ResultView {
    Window *window;
    MessageLoader *msgLoader;
    StringTemplate *template;
    int value;
} ResultView;

typedef struct BattleTestMenu {
    FieldSystem *fieldSystem;
    Window *window;
    int page;
} BattleTestMenu;

static void Print(ResultView *view, u32 id, u32 x, u32 y, u32 speed, u32 color)
{
    String *source = MessageLoader_GetNewString(view->msgLoader, id);
    String *text = String_Init(256, HEAP_ID_FIELD1);
    StringTemplate_Format(view->template, text, source);
    Text_AddPrinterWithParamsAndColor(view->window, FONT_SYSTEM, text, x, y, speed, color, NULL);
    String_Free(text);
    String_Free(source);
}

static void Render(ResultView *subMenu)
{
    Window_FillTilemap(subMenu->window, DEBUG_COLOR_WHITE);
    if (subMenu->value == 0) {
        DebugBattleSummary summary = DebugBattle_GetSummary();
        u32 status = BattleTests_Text_NoBattleResult;
        if (summary.failed) status = BattleTests_Text_BattleFail;
        else if (summary.completed == DEBUG_BATTLE_CASE_COUNT && summary.repeats == DEBUG_BATTLE_CASE_COUNT) status = BattleTests_Text_BattlePass;
        Print(subMenu, status, 0, 0, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK);
        StringTemplate_SetNumber(subMenu->template, 0, summary.completed, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
        StringTemplate_SetNumber(subMenu->template, 1, DEBUG_BATTLE_CASE_COUNT, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
        StringTemplate_SetNumber(subMenu->template, 2, summary.passed, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
        StringTemplate_SetNumber(subMenu->template, 3, summary.probes, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
        StringTemplate_SetNumber(subMenu->template, 4, summary.failed, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
        StringTemplate_SetNumber(subMenu->template, 5, summary.repeats, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
        Print(subMenu, BattleTests_Text_BattleSummary, 0, 16, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK);
        return;
    }
    int index = subMenu->value - 1;
    const DebugBattleResult *result = DebugBattle_GetResult(index);
    if (!result->valid) {
        Print(subMenu, BattleTests_Text_NoBattleResult, 0, 0, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK);
        return;
    }
    u32 status = result->failures == 0 ? BattleTests_Text_BattlePass : BattleTests_Text_BattleFail;
    if (result->expectedFailures) {
        status = result->failures == result->expectedFailures ? BattleTests_Text_BattleXFail : BattleTests_Text_BattleFail;
    }
    Print(subMenu, status, 0, 0, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK);
    Print(subMenu, DebugBattle_GetCaseName(index), 0, 16, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK);
    StringTemplate_SetNumber(subMenu->template, 0, result->failures, 3, PADDING_MODE_NONE, CHARSET_MODE_EN);
    StringTemplate_SetNumber(subMenu->template, 1, result->damage, 3, PADDING_MODE_NONE, CHARSET_MODE_EN);
    StringTemplate_SetNumber(subMenu->template, 2, DebugBattle_GetExpectedDamage(index), 3, PADDING_MODE_NONE, CHARSET_MODE_EN);
    StringTemplate_SetNumber(subMenu->template, 3, result->repeatMatched, 1, PADDING_MODE_NONE, CHARSET_MODE_EN);
    StringTemplate_SetNumber(subMenu->template, 4, result->playerPP, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
    StringTemplate_SetNumber(subMenu->template, 5, result->result, 2, PADDING_MODE_NONE, CHARSET_MODE_EN);
    Print(subMenu, BattleTests_Text_BattleResult, 0, 32, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK);
}

void DebugBattleMenu_Render(Window *window, int page)
{
    ResultView view;
    view.window = window;
    view.value = page;
    view.msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_BATTLE_TESTS, HEAP_ID_FIELD1);
    view.template = StringTemplate_Default(HEAP_ID_FIELD1);
    Render(&view);
    MessageLoader_Free(view.msgLoader);
    StringTemplate_Free(view.template);
    Window_CopyToVRAM(window);
}

static void Refresh(BattleTestMenu *menu)
{
    DebugBattleMenu_Render(menu->window, menu->page);
    MessageLoader *loader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_BATTLE_TESTS, HEAP_ID_FIELD1);
    String *text = MessageLoader_GetNewString(loader, BattleTests_Text_Controls);
    Text_AddPrinterWithParamsAndColor(menu->window, FONT_SYSTEM, text, 0, 144, TEXT_SPEED_INSTANT, DEBUG_TEXT_BLACK, NULL);
    String_Free(text);
    MessageLoader_Free(loader);
    Window_CopyToVRAM(menu->window);
}

static void Close(SysTask *task, BattleTestMenu *menu)
{
    Window_EraseStandardFrame(menu->window, TRUE);
    Window_ClearAndCopyToVRAM(menu->window);
    Window_Remove(menu->window);
    Heap_Free(menu->window);
    Heap_Free(menu);
    SysTask_Done(task);
}

static void HandleInput(SysTask *task, void *data)
{
    BattleTestMenu *menu = data;
    if (gSystem.pressedKeys & PAD_BUTTON_B) {
        Close(task, menu);
        FieldSystem_ResumeProcessing();
        return;
    }
    if (gSystem.pressedKeys & PAD_BUTTON_A) {
        if (DebugBattle_Start(menu->fieldSystem)) {
            Close(task, menu);
            return;
        }
        menu->page = 0;
        Refresh(menu);
    } else if (gSystem.pressedKeys & PAD_KEY_UP) {
        if (menu->page < DEBUG_BATTLE_CASE_COUNT) menu->page++;
        Refresh(menu);
    } else if (gSystem.pressedKeys & PAD_KEY_DOWN) {
        if (menu->page > 0) menu->page--;
        Refresh(menu);
    }
}

void DebugBattleMenu_Open(FieldSystem *fieldSystem)
{
    if (!BATTLE_TESTS_BUILD_ENABLED) return;
    BattleTestMenu *menu = Heap_Alloc(HEAP_ID_FIELD1, sizeof(*menu));
    menu->fieldSystem = fieldSystem;
    menu->page = 0;
    menu->window = Window_New(HEAP_ID_FIELD1, 1);
    LoadStandardWindowGraphics(fieldSystem->bgConfig, BG_LAYER_MAIN_3, 660, 11, STANDARD_WINDOW_SYSTEM, HEAP_ID_FIELD1);
    Window_Add(fieldSystem->bgConfig, menu->window, BG_LAYER_MAIN_3, 2, 1, 26, 20, 13, 8);
    Window_DrawStandardFrame(menu->window, TRUE, 660, 11);
    Refresh(menu);
    FieldSystem_PauseProcessing();
    SysTask_Start(HandleInput, menu, 0);
}
