#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "overlay005/debug_mon_shiny.h"

int main(void)
{
    assert(UINT_MAX == 0xffffffffu);
    unsigned int pid = UINT_MAX, ot = 0;
    for (unsigned int i = 0; i < 100000; i++) {
        const unsigned int ratios[] = { 0,31,63,127,191,225,254,255 };
        unsigned int ratio=ratios[i%8];
        for (unsigned int shiny = 0; shiny < 2; shiny++) {
            unsigned int out = DebugMon_WithShininess(ot, pid, shiny, ratio);
            assert(DebugMon_IsShiny(ot, out) == shiny);
            assert(out % 25 == pid % 25);
            assert((out & 1) == (pid & 1));
            if(ratio>0 && ratio<254) assert(((out & 255)<ratio)==((pid & 255)<ratio));
            assert(((out >> 16) % 10 < 5) == ((pid >> 16) % 10 < 5));
            assert(DebugMon_WithShininess(ot, out, shiny, ratio) == out);
            if (DebugMon_IsShiny(ot, pid) == shiny) assert(out == pid);
            unsigned int back = DebugMon_WithShininess(ot, out, !shiny, ratio);
            if (DebugMon_IsShiny(ot, back) != !shiny) { printf("FAIL ot=%08x pid=%08x out=%08x back=%08x target=%u\n", ot,pid,out,back,!shiny); return 1; }
            assert(back % 25 == pid % 25 && (back & 1) == (pid & 1));
        }
        pid = pid * 1664525u + 1013904223u;
        ot = ot * 1103515245u + 12345u;
    }
    puts("PASS: 200000 shiny states plus inverse toggles; nature, gender, ability parity, evolution branch, idempotence");
    return 0;
}
