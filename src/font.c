#include "font.h"

#include <nitro.h>
#include <string.h>

#include "constants/charcode.h"
#include "constants/heap.h"
#include "constants/narc.h"

#include "charcode.h"
#include "font_manager.h"
#include "graphics.h"
#include "heap.h"
#include "palette.h"
#include "render_text.h"
#include "string_gf.h"
#include "text.h"

#include "res/fonts/pl_font.naix"

typedef struct {
    TextGlyph curGlyph;
    void *data[FONT_MAX];
    FontManager *fontManagers[FONT_MAX];
} FontWork;

static const struct {
    u16 arcFileIdx;
    u16 isMonospace;
} sFontArchiveData[FONT_MAX] = {
    [FONT_SYSTEM] = {
        .arcFileIdx = font_system_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_MESSAGE] = {
        .arcFileIdx = font_message_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_SUBSCREEN] = {
        .arcFileIdx = font_subscreen_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_UNOWN] = {
        .arcFileIdx = font_unown_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_NARROW] = {
        .arcFileIdx = font_narrow_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_NARROWER] = {
        .arcFileIdx = font_narrower_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_SHORT] = {
        .arcFileIdx = font_short_NFGR,
        .isMonospace = FALSE,
    },
    [FONT_SHORTER] = {
        .arcFileIdx = font_shorter_NFGR,
        .isMonospace = FALSE,
    },

};

static const FontAttributes sFontAttributes[FONT_MAX + 1] = {
    [FONT_SYSTEM] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_MESSAGE] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_SUBSCREEN] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_UNOWN] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_NARROW] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_NARROWER] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_SHORT] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },
    [FONT_SHORTER] = {
        .maxLetterWidth = 11,
        .maxLetterHeight = 16,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 2,
    },

};

static FontWork work;
static FontWork *sFontWork = NULL;

void Fonts_Init(void)
{
    sFontWork = &work;

    for (int i = 0; i < NELEMS(sFontArchiveData); i++) {
        sFontWork->data[i] = NULL;
        sFontWork->fontManagers[i] = NULL;
    }

    Text_SetFontAttributesPtr(sFontAttributes);
}

void Font_InitManager(enum Font font, enum HeapID heapID)
{
    sFontWork->fontManagers[font] = FontManager_New(NARC_INDEX_GRAPHIC__PL_FONT,
        sFontArchiveData[font].arcFileIdx,
        GLYPH_ACCESS_MODE_LAZY,
        sFontArchiveData[font].isMonospace,
        heapID);
}

void Font_UseImmediateGlyphAccess(enum Font font, enum HeapID heapID)
{
    GF_ASSERT(font < FONT_MAX);
    GF_ASSERT(sFontWork->fontManagers[font]);

    FontManager_SwitchGlyphAccessMode(sFontWork->fontManagers[font], GLYPH_ACCESS_MODE_IMMEDIATE, heapID);
}

void Font_UseLazyGlyphAccess(enum Font font)
{
    GF_ASSERT(font < FONT_MAX);
    GF_ASSERT(sFontWork->fontManagers[font]);

    FontManager_SwitchGlyphAccessMode(sFontWork->fontManagers[font], GLYPH_ACCESS_MODE_LAZY, HEAP_ID_SYSTEM);
}

void Font_Free(enum Font font)
{
    GF_ASSERT(font < FONT_MAX);

    if (sFontWork->data[font] != NULL) {
        int i;
        for (i = 0; i < NELEMS(sFontArchiveData); i++) {
            if (i != font
                && sFontArchiveData[i].arcFileIdx == sFontArchiveData[font].arcFileIdx
                && sFontWork->fontManagers[i] != NULL) {
                sFontWork->data[i] = sFontWork->data[font];
                break;
            }
        }

        if (i == NELEMS(sFontArchiveData)) {
            Heap_Free(sFontWork->data[font]);
            sFontWork->data[font] = NULL;
        }
    }

    if (sFontWork->fontManagers[font] != NULL) {
        FontManager_Delete(sFontWork->fontManagers[font]);
        sFontWork->fontManagers[font] = NULL;
    }
}

const TextGlyph *Font_TryLoadGlyph(enum Font font, charcode_t c)
{
    FontManager_TryLoadGlyph(sFontWork->fontManagers[font], c, &sFontWork->curGlyph);
    return &sFontWork->curGlyph;
}

enum RenderResult Font_RenderText(enum Font font, TextPrinter *printer)
{
    TextPrinterSubstruct *substruct = (TextPrinterSubstruct *)printer->substruct;

    if (!substruct->fontIDSet) {
        substruct->fontID = font;
        substruct->fontIDSet = TRUE;
    }

    return RenderText(printer);
}

u32 Font_CalcCharArrayWidth(enum Font font, const charcode_t *str, u32 letterSpacing)
{
    GF_ASSERT(sFontWork->fontManagers[font] != NULL);
    return FontManager_CalcStringWidth(sFontWork->fontManagers[font], str, letterSpacing);
}

u32 Font_CalcStringWidth(enum Font font, const String *string, u32 letterSpacing)
{
    GF_ASSERT(sFontWork->fontManagers[font] != NULL);
    return FontManager_CalcStringWidth(sFontWork->fontManagers[font], String_GetData(string), letterSpacing);
}

u32 Font_AreAllCharsValid(enum Font font, String *string, String *tmpbuf)
{
    GF_ASSERT(sFontWork->fontManagers[font] != NULL);

    String_Clear(tmpbuf);
    String_ConcatTrainerName(tmpbuf, string);

    return FontManager_AreAllCharsValid(sFontWork->fontManagers[font], String_GetData(tmpbuf));
}

u8 Font_GetAttribute(u8 font, u8 attribute)
{
    u8 result = 0;

    switch (attribute) {
    case FONTATTR_MAX_LETTER_WIDTH:
        result = sFontAttributes[font].maxLetterWidth;
        break;

    case FONTATTR_MAX_LETTER_HEIGHT:
        result = sFontAttributes[font].maxLetterHeight;
        break;

    case FONTATTR_LETTER_SPACING:
        result = sFontAttributes[font].letterSpacing;
        break;

    case FONTATTR_LINE_SPACING:
        result = sFontAttributes[font].lineSpacing;
        break;

    case FONTATTR_DUMMY:
        result = sFontAttributes[font].dummy;
        break;

    case FONTATTR_FG_COLOR:
        result = sFontAttributes[font].fgColor;
        break;

    case FONTATTR_BG_COLOR:
        result = sFontAttributes[font].bgColor;
        break;

    case FONTATTR_SHADOW_COLOR:
        result = sFontAttributes[font].shadowColor;
        break;
    }

    return result;
}

void Font_LoadTextPalette(enum PaletteLoadLocation palLocation, u32 palSlotOffset, enum HeapID heapID)
{
    Graphics_LoadPalette(NARC_INDEX_GRAPHIC__PL_FONT,
        font_NCLR,
        palLocation,
        palSlotOffset,
        PALETTE_SIZE_BYTES,
        heapID);
}

void Font_LoadScreenIndicatorsPalette(enum PaletteLoadLocation palLocation, u32 palSlotOffset, enum HeapID heapID)
{
    Graphics_LoadPalette(NARC_INDEX_GRAPHIC__PL_FONT,
        screen_indicators_NCLR,
        palLocation,
        palSlotOffset,
        PALETTE_SIZE_BYTES,
        heapID);
}

u32 Font_CalcMaxLineWidth(enum Font font, const String *string, u32 letterSpacing)
{
    GF_ASSERT(sFontWork->fontManagers[font] != NULL);
    return FontManager_CalcMaxLineWidth(sFontWork->fontManagers[font], String_GetData(string), letterSpacing);
}

u32 Font_CalcCenterAlignment(enum Font font, const String *string, u32 letterSpacing, u32 windowWidth)
{
    u32 width = Font_CalcStringWidth(font, string, letterSpacing);

    return width < windowWidth ? (windowWidth - width) / 2 : 0;
}

u32 Font_CalcStringWidthWithCursorControl(enum Font font, const String *string)
{
    GF_ASSERT(sFontWork->fontManagers[font] != NULL);
    return FontManager_CalcStringWidthWithCursorControl(sFontWork->fontManagers[font], String_GetData(string));
}

BOOL Font_TryFitName(enum Font preferred, const String *string, u32 usableWidth, enum Font *selected, u32 *measuredWidth)
{
    // Preserve the requested native style first, then try the approved family.
    // Prefer full-height narrowing over shorter forms, and NARROWER last.
    const enum Font alternatives[] = { FONT_SYSTEM, FONT_NARROW, FONT_SHORT, FONT_SHORTER, FONT_NARROWER };
    if (selected == NULL || measuredWidth == NULL) {
        return FALSE;
    }
    *selected = preferred;
    *measuredWidth = 0;
    if (string == NULL || preferred < 0 || preferred >= FONT_MAX || sFontWork == NULL) {
        return FALSE;
    }
    const charcode_t *chars = String_GetData(string);
    u32 length = String_Length(string);
    for (u32 candidate = 0; candidate <= NELEMS(alternatives); candidate++) {
        enum Font font = candidate == 0 ? preferred : alternatives[candidate - 1];
        FontManager *manager = sFontWork->fontManagers[font];
        if (manager == NULL) {
            continue;
        }
        BOOL valid = TRUE;
        for (u32 i = 0; i < length; i++) {
            // Plain resolved names only; reject controls, EOS and unmapped codes.
            if (chars[i] == 0 || chars[i] > manager->header.numGlyphs) {
                valid = FALSE;
                break;
            }
        }
        if (!valid) {
            continue;
        }
        u32 width = Font_CalcStringWidth(font, string, 0);
        if (width <= usableWidth) {
            *selected = font;
            *measuredWidth = width;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL Font_PrintName(Window *window, enum Font preferred, const String *string, u32 x, u32 y, u32 usableWidth, BOOL centered, u32 color)
{
    if (window == NULL) {
        return FALSE;
    }
    u32 windowWidth = Window_GetWidth(window) * 8;
    u32 windowHeight = Window_GetHeight(window) * 8;
    if (x >= windowWidth || y >= windowHeight) {
        return FALSE;
    }
    if (usableWidth > windowWidth - x) {
        usableWidth = windowWidth - x;
    }
    enum Font font;
    u32 width;
    // All reviewed atlases retain 16-row cells and unshortened fallback glyphs.
    // Keep the original origin; never infer a shorter line box from Latin caps.
    if (windowHeight - y >= MAX_LETTER_HEIGHT
        && Font_TryFitName(preferred, string, usableWidth, &font, &width)) {
        u32 drawX = centered ? x + (usableWidth - width) / 2 : x;
        Text_AddPrinterWithParamsAndColor(window, font, string, drawX, y, TEXT_SPEED_NO_TRANSFER, color, NULL);
        return TRUE;
    }
    // Explicit development overflow marker: no clipped/truncated name prefix.
    // Callers retain the complete String and must resolve this field before release.
    if (usableWidth >= 7 && windowHeight - y >= 7) {
        u8 foreground = GET_TEXT_FG_COLOR(color);
        for (u32 i = 0; i < 7; i++) {
            Window_FillRectWithColor(window, foreground, x + i, y + i, 1, 1);
            Window_FillRectWithColor(window, foreground, x + 6 - i, y + i, 1, 1);
        }
    }
    return FALSE;
}

// The admitted decoration is COLOR with one palette-index argument (0..6).
// Validate lengths before examining parameters; never pass arbitrary controls
// to the name selector. The original string is retained for actual rendering.
BOOL Font_TryFitNameWithColor(enum Font preferred, const String *string, u32 usableWidth, enum Font *selected, u32 *measuredWidth, u32 heapID)
{
    if (string == NULL || selected == NULL || measuredWidth == NULL) {
        return FALSE;
    }
    *selected = preferred;
    *measuredWidth = 0;
    u32 length = String_Length(string);
    const charcode_t *chars = String_GetData(string);
    String *plain = String_Init(length + 1, heapID);
    if (plain == NULL) {
        return FALSE;
    }
    BOOL valid = TRUE;
    for (u32 i = 0; i < length; i++) {
        if (chars[i] == CHAR_FORMAT_ARG) {
            if (length - i < 4 || chars[i + 1] != CHAR_CONTROL_SET_COLOR
                || chars[i + 2] != 1 || chars[i + 3] > 6) {
                valid = FALSE;
                break;
            }
            i += 3;
        } else {
            String_AppendChar(plain, chars[i]);
        }
    }
    BOOL fits = valid && Font_TryFitName(preferred, plain, usableWidth, selected, measuredWidth);
    String_Free(plain);
    return fits;
}

BOOL Font_PrintNameWithColor(Window *window, enum Font preferred, const String *string, u32 x, u32 y, u32 usableWidth, u32 color, u32 heapID)
{
    if (window == NULL || x >= Window_GetWidth(window) * 8 || y >= Window_GetHeight(window) * 8) {
        return FALSE;
    }
    u32 remaining = Window_GetWidth(window) * 8 - x;
    if (usableWidth > remaining) {
        usableWidth = remaining;
    }
    enum Font selected;
    u32 measured;
    if (Window_GetHeight(window) * 8 - y >= MAX_LETTER_HEIGHT
        && Font_TryFitNameWithColor(preferred, string, usableWidth, &selected, &measured, heapID)) {
        Text_AddPrinterWithParamsAndColor(window, selected, string, x, y, TEXT_SPEED_NO_TRANSFER, color, NULL);
        return TRUE;
    }
    // Use the shared bounded marker, including allocation/malformed-input failure.
    return Font_PrintName(window, preferred, NULL, x, y, usableWidth, FALSE, color);
}
