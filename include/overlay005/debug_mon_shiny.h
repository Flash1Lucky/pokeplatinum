#ifndef POKEPLATINUM_DEBUG_MON_SHINY_H
#define POKEPLATINUM_DEBUG_MON_SHINY_H

static inline unsigned int DebugMon_IsShiny(unsigned int ot, unsigned int pid)
{
    return ((ot >> 16) ^ (ot & 0xffff) ^ (pid >> 16) ^ (pid & 0xffff)) < 8;
}

// Preserve OT, nature, gender/ability parity, and Wurmple's evolution
// branch. The editor applies the PID through the engine's re-encryption helper.
// PID-dependent cosmetics (notably Spinda spots) can change; the UI warns first.
static unsigned int DebugMon_WithShininess(unsigned int ot, unsigned int pid, unsigned int shiny, unsigned int genderRatio)
{
    if (DebugMon_IsShiny(ot, pid) == shiny) {
        return pid;
    }
    if (!shiny) {
        for (unsigned int offset = 1; offset <= 65536; offset++) {
            unsigned int candidate = pid + offset * 256;
            if (candidate % 25 == pid % 25 && !DebugMon_IsShiny(ot, candidate)
                && ((candidate >> 16) % 10 < 5) == ((pid >> 16) % 10 < 5)) {
                return candidate;
            }
        }
        return pid;
    }
    unsigned int trainerXor = (ot >> 16) ^ (ot & 0xffff);
    for (unsigned int offset = 0; offset < 32768; offset++) {
        unsigned int low = (pid + offset * 2) & 0xffff;
        if (genderRatio > 0 && genderRatio < 254
            && ((low & 255) < genderRatio) != ((pid & 255) < genderRatio)) {
            continue;
        }
        for (unsigned int bits = 0; bits < 8; bits++) {
            unsigned int high = trainerXor ^ low ^ bits;
            unsigned int candidate = (high << 16) | low;
            if (candidate % 25 == pid % 25 && (high % 10 < 5) == ((pid >> 16) % 10 < 5)) {
                return candidate;
            }
        }
    }
    // No compatible PID: leave all traits intact; caller displays a failure.
    return pid;
}

#endif
