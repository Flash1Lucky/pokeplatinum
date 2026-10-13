#ifndef POKEPLATINUM_ROSTER_RECORD_H
#define POKEPLATINUM_ROSTER_RECORD_H

/* Logical A/B encoding shared by development schemas 2 and 3.
 * Callers must supply the logical, decrypted 32-byte A/B blocks obtained
 * through Platinum's shuffle/decryption context. Never use on encrypted data.
 * Ability slot is authoritative; resolved ability IDs live in personal data.
 * Form is authoritative at B+0x1A; the former five-bit field must be zero.
 */
#define ROSTER_RECORD_ABILITY_SLOT_OFFSET 0x0DU
#define ROSTER_RECORD_FORM_OFFSET 0x1AU
#define ROSTER_RECORD_LEGACY_FORM_OFFSET 0x18U

static inline unsigned int RosterRecord_GetSlot(const unsigned char *blockA)
{
    return blockA[ROSTER_RECORD_ABILITY_SLOT_OFFSET];
}

static inline unsigned int RosterRecord_GetForm(const unsigned char *blockB)
{
    return blockB[ROSTER_RECORD_FORM_OFFSET]
        | ((unsigned int)blockB[ROSTER_RECORD_FORM_OFFSET + 1U] << 8);
}

static inline int RosterRecord_Validate(const unsigned char *blockA,
    const unsigned char *blockB)
{
    return RosterRecord_GetSlot(blockA) < 3U
        && (blockB[ROSTER_RECORD_LEGACY_FORM_OFFSET] & 0xF8U) == 0;
}

/* Atomic validation: rejected input does not change either block.
 * Availability and species/form registry membership are caller responsibilities.
 */
static inline int RosterRecord_SetIdentity(unsigned char *blockA,
    unsigned char *blockB, unsigned int slot, unsigned int form)
{
    if (slot >= 3U || form > 0xFFFFU) {
        return 0;
    }
    blockA[ROSTER_RECORD_ABILITY_SLOT_OFFSET] = (unsigned char)slot;
    blockB[ROSTER_RECORD_FORM_OFFSET] = (unsigned char)form;
    blockB[ROSTER_RECORD_FORM_OFFSET + 1U] = (unsigned char)(form >> 8);
    blockB[ROSTER_RECORD_LEGACY_FORM_OFFSET] &= 0x07U;
    return 1;
}

/* Missing normal/hidden abilities are rejected here, not silently reassigned.
 * Zero is ABILITY_NONE. This proof keeps Trace/Skill Swap state out of storage.
 */
static inline int RosterRecord_ResolveAbility(unsigned int slot,
    const unsigned short abilities[3], unsigned short *result)
{
    if (slot >= 3U || abilities[slot] == 0) {
        return 0;
    }
    *result = abilities[slot];
    return 1;
}

#endif
