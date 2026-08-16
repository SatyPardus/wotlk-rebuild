#include "console/DebugScreen.hpp"
#include "console/Screen.hpp"
#include "console/Text.hpp"
#include "console/Types.hpp"
#include "gx/Buffer.hpp"
#include "gx/Coordinate.hpp"
#include "gx/Device.hpp"
#include "gx/Draw.hpp"
#include "gx/Font.hpp"
#include "gx/Gx.hpp"
#include "gx/RenderState.hpp"
#include "gx/Screen.hpp"
#include <bc/Debug.hpp>
#include <cstdarg>
#include <storm/String.hpp>

struct DEBUGSCREENLINE {
    char label[DEBUG_SCREEN_MAX_LABEL];
    char buffer[DEBUG_SCREEN_MAX_TEXT];
    CGxString* fontPointer;
    COLOR_T colorType;
    int32_t used;
};

static HLAYER s_layerDebug;

// RECTF is { left, bottom, right, top } and OnPaint only calls a layer's
// paintfunc when visible.top > visible.bottom, so bottom must be the SMALLER
// value.  A full-screen layer is { 0, 0, 1, 1 }.
static RECTF s_debugRect = { 0.0f, 0.0f, 1.0f, 1.0f };

static DEBUGSCREENLINE s_debugLines[DEBUG_SCREEN_MAX_LINES];
static int32_t s_debugLineCount = 0;

static CGxStringBatch* s_debugBatch = nullptr;
static char s_debugFontName[STORM_MAX_PATH];
static HTEXTFONT s_debugTextFont;

// The layer is drawn through GxuXformCreateOrtho(left, right, bottom, top),
// so y increases UPWARD and y = 1.0 is the top of the screen.  s_debugScreenTop
// is the baseline of the first line; each line after it steps DOWN.
float s_debugScreenFontHeight = 0.02f;
float s_debugScreenLeft = 0.008f;
float s_debugScreenTop = 0.975f;

static const float s_debugPanelPad = 0.004f;

DEBUGSCREENLINE* FindDebugLine(const char* label) {
    for (auto i = 0; i < s_debugLineCount; i++) {
        if (s_debugLines[i].used && !SStrCmp(s_debugLines[i].label, label, DEBUG_SCREEN_MAX_LABEL)) {
            return &s_debugLines[i];
        }
    }

    return nullptr;
}

DEBUGSCREENLINE* AcquireDebugLine(const char* label) {
    auto line = FindDebugLine(label);

    if (line) {
        return line;
    }

    for (auto i = 0; i < DEBUG_SCREEN_MAX_LINES; i++) {
        if (!s_debugLines[i].used) {
            line = &s_debugLines[i];

            SStrCopy(line->label, label, sizeof(line->label));
            line->buffer[0] = '\0';
            line->fontPointer = nullptr;
            line->colorType = DEFAULT_COLOR;
            line->used = 1;

            if (i >= s_debugLineCount) {
                s_debugLineCount = i + 1;
            }

            return line;
        }
    }

    return nullptr;
}

void SetDebugLineText(DEBUGSCREENLINE* line, COLOR_T colorType, const char* text) {
    char buffer[DEBUG_SCREEN_MAX_TEXT];
    SStrPrintf(buffer, sizeof(buffer), "%s: %s", line->label, text);

    if (line->colorType == colorType && !SStrCmp(line->buffer, buffer, sizeof(buffer))) {
        return;
    }

    SStrCopy(line->buffer, buffer, sizeof(line->buffer));
    line->colorType = colorType;

    if (line->fontPointer) {
        GxuFontDestroyString(line->fontPointer);
        line->fontPointer = nullptr;
    }
}

void GenerateDebugLineString(DEBUGSCREENLINE* line) {
    auto font = TextBlockGetFontPtr(s_debugTextFont);

    if (font && line->buffer[0] != '\0') {
        if (line->fontPointer) {
            GxuFontDestroyString(line->fontPointer);
        }

        C3Vector pos = {
            0.0f, 0.0f, 1.0f
        };

        GxuFontCreateString(
            font,
            line->buffer,
            s_debugScreenFontHeight,
            pos,
            1.0f,
            s_debugScreenFontHeight,
            0.0f,
            line->fontPointer,
            GxVJ_Middle, GxHJ_Left,
            s_baseTextFlags,
            s_colorArray[line->colorType],
            s_charSpacing,
            1.0f);

        STORM_ASSERT(line->fontPointer);
    }
}

float MeasureDebugPanelWidth() {
    auto font = TextBlockGetFontPtr(s_debugTextFont);

    if (!font) {
        return 0.0f;
    }

    auto widest = 0.0f;

    for (auto i = 0; i < s_debugLineCount; i++) {
        auto line = &s_debugLines[i];

        if (!line->used || line->buffer[0] == '\0') {
            continue;
        }

        auto width = 0.0f;

        GxuFontGetTextExtent(font, line->buffer, SStrLen(line->buffer), s_debugScreenFontHeight, &width, 0.0f, 1.0f, s_charSpacing, s_baseTextFlags);

        widest = width > widest ? width : widest;
    }

    return widest;
}

int32_t CountDebugLines() {
    auto count = 0;

    for (auto i = 0; i < s_debugLineCount; i++) {
        if (s_debugLines[i].used && s_debugLines[i].buffer[0] != '\0') {
            count++;
        }
    }

    return count;
}

void DrawDebugPanel(float width, int32_t lines) {
    uint16_t indices[] = {
        0, 1, 2, 3
    };

    float minX = s_debugScreenLeft - s_debugPanelPad;
    float maxX = s_debugScreenLeft + width + s_debugPanelPad;

    // Lines run downward from s_debugScreenTop, and GxVJ_Middle centres each
    // one on its baseline, so the block is half a line tall either side.
    float maxY = s_debugScreenTop + (s_debugScreenFontHeight * 0.5f) + s_debugPanelPad;
    float minY = s_debugScreenTop - (s_debugScreenFontHeight * (lines - 0.5f)) - s_debugPanelPad;

    C3Vector position[] = {
        { minX, minY, 0.0f },
        { maxX, minY, 0.0f },
        { minX, maxY, 0.0f },
        { maxX, maxY, 0.0f }
    };

    GxRsPush();

    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_PolygonOffset, 0.0f);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxRsSet(GxRs_AlphaRef, CGxDevice::s_alphaRef[GxBlend_Alpha]);

    GxPrimLockVertexPtrs(4, position, sizeof(C3Vector), nullptr, 0, &s_colorArray[BACKGROUND_COLOR], 0, nullptr, 0, nullptr, 0, nullptr, 0);
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, indices);
    GxPrimUnlockVertexPtrs();

    GxRsPop();
}

void PaintDebugScreen(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    auto lines = CountDebugLines();

    if (!lines) {
        return;
    }

    DrawDebugPanel(MeasureDebugPanelWidth(), lines);

    C3Vector pos = {
        s_debugScreenLeft,
        s_debugScreenTop - s_debugPanelPad * 2,
        1.0f
    };

    GxuFontClearBatch(s_debugBatch);

    for (auto i = 0; i < s_debugLineCount; i++) {
        auto line = &s_debugLines[i];

        if (!line->used || line->buffer[0] == '\0') {
            continue;
        }

        if (line->fontPointer == nullptr) {
            GenerateDebugLineString(line);
        }

        if (line->fontPointer) {
            GxuFontSetStringPosition(line->fontPointer, pos);
            GxuFontAddToBatch(s_debugBatch, line->fontPointer);
        }

        pos.y -= s_debugScreenFontHeight;
    }

    GxuFontRenderBatch(s_debugBatch);
}

void DebugScreenSet(const char* label, const char* format, ...) {
    auto line = AcquireDebugLine(label);

    if (!line) {
        return;
    }

    char text[DEBUG_SCREEN_MAX_TEXT];

    va_list args;
    va_start(args, format);
    SStrVPrintf(text, sizeof(text), format, args);
    va_end(args);

    SetDebugLineText(line, line->colorType, text);
}

void DebugScreenSetColored(COLOR_T colorType, const char* label, const char* format, ...) {
    auto line = AcquireDebugLine(label);

    if (!line) {
        return;
    }

    char text[DEBUG_SCREEN_MAX_TEXT];

    va_list args;
    va_start(args, format);
    SStrVPrintf(text, sizeof(text), format, args);
    va_end(args);

    SetDebugLineText(line, colorType, text);
}

void DebugScreenRemove(const char* label) {
    auto line = FindDebugLine(label);

    if (!line) {
        return;
    }

    if (line->fontPointer) {
        GxuFontDestroyString(line->fontPointer);
        line->fontPointer = nullptr;
    }

    line->label[0] = '\0';
    line->buffer[0] = '\0';
    line->used = 0;
}

void DebugScreenClear() {
    for (auto i = 0; i < s_debugLineCount; i++) {
        auto line = &s_debugLines[i];

        if (line->fontPointer) {
            GxuFontDestroyString(line->fontPointer);
            line->fontPointer = nullptr;
        }

        line->label[0] = '\0';
        line->buffer[0] = '\0';
        line->used = 0;
    }

    s_debugLineCount = 0;
}

void DebugScreenInitialize() {
    SStrCopy(s_debugFontName, "Fonts\\ARIALN.ttf", sizeof(s_debugFontName));
    s_debugTextFont = TextBlockGenerateFont(s_debugFontName, 0, NDCToDDCHeight(s_debugScreenFontHeight));

    ScrnLayerCreate(&s_debugRect, 8.0f, 0x1 | 0x2, nullptr, PaintDebugScreen, &s_layerDebug);

    s_debugBatch = GxuFontCreateBatch(false, false);
}
