#ifndef POKEPLATINUM_DEBUG_MENU_VALUES_H
#define POKEPLATINUM_DEBUG_MENU_VALUES_H

// All values are unsigned 32-bit on both the DS and the host regression test.
static inline unsigned int DebugMenu_ChangeValue(unsigned int value, unsigned int minimum, unsigned int maximum, unsigned int step, int increase)
{
    if (value < minimum) {
        value = minimum;
    }
    if (value > maximum) {
        value = maximum;
    }
    if (increase) {
        if (value == maximum) {
            return minimum;
        }
        return step < maximum - value ? value + step : maximum;
    }
    if (value == minimum) {
        return maximum;
    }
    return step < value - minimum ? value - step : minimum;
}

static inline unsigned int DebugMenu_PersonalityWithNature(unsigned int personality, unsigned int nature)
{
    // The low byte determines ordinary gender and ability parity. Changing the
    // other bits may change shiny status, Unown form, and Spinda markings.
    while (personality % 25 != nature) {
        personality += 256;
    }
    return personality;
}

static inline unsigned int DebugMenu_LimitEV(unsigned int requested, unsigned int otherStatsTotal)
{
    unsigned int remaining = otherStatsTotal < 510 ? 510 - otherStatsTotal : 0;
    if (requested > 255) {
        requested = 255;
    }
    return requested < remaining ? requested : remaining;
}

// Keep damage when stats change, but never turn a live mon into wrapped HP.
static inline unsigned int DebugMenu_AdjustHP(unsigned int hp, unsigned int oldMaximum, unsigned int newMaximum)
{
    if (hp == 0 || newMaximum == 0) {
        return 0;
    }
    unsigned int damage = oldMaximum > hp ? oldMaximum - hp : 0;
    return damage >= newMaximum ? 1 : newMaximum - damage;
}

#endif
