#include "ui/CSimpleMessageFrameScript.hpp"
#include "ui/CSimpleMessageFrame.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"

// OFFSET: 0x973D70
static int32_t Script_SetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973DD0
static int32_t Script_GetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973E30
static int32_t Script_SetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973EA0
static int32_t Script_GetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973F00
static int32_t Script_SetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973F60
static int32_t Script_GetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973FC0
static int32_t Script_SetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974020
static int32_t Script_GetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974080
static int32_t Script_SetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9740E0
static int32_t Script_GetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974140
static int32_t Script_SetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9741A0
static int32_t Script_GetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974200
static int32_t Script_SetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974260
static int32_t Script_GetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9742C0
static int32_t Script_SetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974320
static int32_t Script_GetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974380
static int32_t Script_SetInsertMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974440
static int32_t Script_GetInsertMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9744B0
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974510
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974570
static int32_t Script_SetFading(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9745C0
static int32_t Script_GetFading(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974620
static int32_t Script_SetTimeVisible(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9746A0
static int32_t Script_GetTimeVisible(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9746F0
static int32_t Script_SetFadeDuration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974770
static int32_t Script_GetFadeDuration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9747C0
static int32_t Script_AddMessage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974890
static int32_t Script_Clear(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}


FrameScript_Method SimpleMessageFrameMethods[NUM_SIMPLE_MESSAGE_FRAME_SCRIPT_METHODS] = {
    { "SetFontObject", &Script_SetFontObject },
    { "GetFontObject", &Script_GetFontObject },
    { "SetFont", &Script_SetFont },
    { "GetFont", &Script_GetFont },
    { "SetTextColor", &Script_SetTextColor },
    { "GetTextColor", &Script_GetTextColor },
    { "SetShadowColor", &Script_SetShadowColor },
    { "GetShadowColor", &Script_GetShadowColor },
    { "SetShadowOffset", &Script_SetShadowOffset },
    { "GetShadowOffset", &Script_GetShadowOffset },
    { "SetSpacing", &Script_SetSpacing },
    { "GetSpacing", &Script_GetSpacing },
    { "SetJustifyH", &Script_SetJustifyH },
    { "GetJustifyH", &Script_GetJustifyH },
    { "SetJustifyV", &Script_SetJustifyV },
    { "GetJustifyV", &Script_GetJustifyV },
    { "SetInsertMode", &Script_SetInsertMode },
    { "GetInsertMode", &Script_GetInsertMode },
    { "SetIndentedWordWrap", &Script_SetIndentedWordWrap },
    { "GetIndentedWordWrap", &Script_GetIndentedWordWrap },
    { "SetFading", &Script_SetFading },
    { "GetFading", &Script_GetFading },
    { "SetTimeVisible", &Script_SetTimeVisible },
    { "GetTimeVisible", &Script_GetTimeVisible },
    { "SetFadeDuration", &Script_SetFadeDuration },
    { "GetFadeDuration", &Script_GetFadeDuration },
    { "AddMessage", &Script_AddMessage },
    { "Clear", &Script_Clear }
};
