# Portable record identity contract - limited extraction

Baseline: upstream/main fe1ea418a7364c93daf0d07ae55e195452d8803c. Provenance: 17a8139c4d3d04327266494098eff20e6a83cd8e. include/roster_record.h is the existing independent, header-only encoding/resolution primitive. It uses the caller's decrypted logical32-byte A/B blocks: slot at A+0x0D, u16 form at B+0x1A, legacy five form bits cleared while low three flags survive. Slot0-2 and form0-65535 are validated atomically; missing abilities reject without assigning another slot. Wide ability resolution is u16 and caller-provided.

No font/name/catalog/debug/profile prerequisite. Caller supplies non-null blocks/three-entry ability table/result and validates species/form registry membership and available slots. Never apply this to encrypted bytes or activate it against vanilla getters/constructors in isolation.

This is a reusable **identity contract**, not an installed wider-record/save feature. Native Pokemon structs, getters, saves, nickname lengths and external DTOs remain unchanged on this branch. Wiring the compact identity contract into constructors/copies/crypto/ability consumers and a compatible guarded schema remains staged in the historical storage foundation. It must not be advertised as standalone full three-slot gameplay support.

The host test covers196608 slot/form combinations, u16 ability IDs, reserved-bit preservation, rejection atomicity and missing abilities. It uses synthetic arrays only; no real save/ROM or runtime acceptance is involved.
