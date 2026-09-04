#include "gameui/CGQuestPOIFrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"

// OFFSET: 0x58FE50
static int32_t Script_SetFillTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E660
static int32_t Script_SetFillAlpha(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58FEA0
static int32_t Script_SetBorderTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E6F0
static int32_t Script_SetBorderAlpha(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E780
static int32_t Script_SetBorderScalar(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5905F0
static int32_t Script_DrawQuestBlob(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E800
static int32_t Script_EnableSmoothing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E850
static int32_t Script_EnableMerging(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E8A0
static int32_t Script_SetMergeThreshold(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E920
static int32_t Script_SetNumSplinePoints(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E990
static int32_t Script_UpdateQuestPOI(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58E9C0
static int32_t Script_UpdateMouseOverTooltip(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58EA50
static int32_t Script_GetTooltipIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58EAC0
static int32_t Script_GetNumTooltips(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}


FrameScript_Method CGQuestPOIFrameMethods[NUM_CGQUEST_POI_FRAME_SCRIPT_METHODS] = {
    { "SetFillTexture", &Script_SetFillTexture },
    { "SetFillAlpha", &Script_SetFillAlpha },
    { "SetBorderTexture", &Script_SetBorderTexture },
    { "SetBorderAlpha", &Script_SetBorderAlpha },
    { "SetBorderScalar", &Script_SetBorderScalar },
    { "DrawQuestBlob", &Script_DrawQuestBlob },
    { "EnableSmoothing", &Script_EnableSmoothing },
    { "EnableMerging", &Script_EnableMerging },
    { "SetMergeThreshold", &Script_SetMergeThreshold },
    { "SetNumSplinePoints", &Script_SetNumSplinePoints },
    { "UpdateQuestPOI", &Script_UpdateQuestPOI },
    { "UpdateMouseOverTooltip", &Script_UpdateMouseOverTooltip },
    { "GetTooltipIndex", &Script_GetTooltipIndex },
    { "GetNumTooltips", &Script_GetNumTooltips }
};
