// Run: cc -std=c99 -Wall -Wextra -Werror -Iinclude tools/tests/debug_menu_values_test.c -o <test>; <test>
#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "overlay005/debug_menu_values.h"

int main(void)
{
    assert(UINT_MAX == 0xffffffffu);
    assert(DebugMenu_ChangeValue(0, 0, 1, 1000000000, 1) == 1);
    assert(DebugMenu_ChangeValue(1, 0, 1, 1000000000, 1) == 0);
    assert(DebugMenu_ChangeValue(UINT_MAX - 1, 0, UINT_MAX, 1000000000, 1) == UINT_MAX);
    assert(DebugMenu_ChangeValue(UINT_MAX, 0, UINT_MAX, 1, 1) == 0);
    assert(DebugMenu_ChangeValue(0, 0, UINT_MAX, 1, 0) == UINT_MAX);
    assert(DebugMenu_ChangeValue(4, 1, 100, 1000000000, 0) == 1);
    assert(DebugMenu_ChangeValue(255, 0, 0, 1, 1) == 0);
    assert(DebugMenu_LimitEV(255, 500) == 10);
    assert(DebugMenu_LimitEV(255, 510) == 0);
    assert(DebugMenu_LimitEV(255, 1020) == 0);
    assert(DebugMenu_LimitEV(UINT_MAX, 0) == 255);
    assert(DebugMenu_AdjustHP(30, 200, 50) == 1);
    assert(DebugMenu_AdjustHP(0, 200, 50) == 0);
    assert(DebugMenu_AdjustHP(50, 50, 100) == 100);
    assert(DebugMenu_AdjustHP(30, 50, 100) == 80);
    assert(DebugMenu_AdjustHP(999, 50, 25) == 25);
    unsigned int state = UINT_MAX;
    for (int i = 0; i < 10000; i++) {
        // Include the overflow boundary before pseudorandom samples.
        for (unsigned int nature = 0; nature < 25; nature++) {
            unsigned int changed = DebugMenu_PersonalityWithNature(state, nature);
            assert(changed % 25 == nature);
            assert((changed & 255) == (state & 255));
            if (state % 25 == nature) {
                assert(changed == state);
            }
        }
        state = state * 1664525u + 1013904223u;
    }
    puts("PASS: numeric boundaries, EV/HP bounds, 250000 PID/nature cases");
    return 0;
}
