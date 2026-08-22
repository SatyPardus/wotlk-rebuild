#include "ui/CSimpleFont.hpp"
#include "ui/CSimpleFontString.hpp"
#include "util/Unimplemented.hpp"
#include <cstdint>
#include "ui/CSimpleFontScript.hpp"

// OFFSET: 0x4A4120
int32_t CSimpleFont_GetObjectType(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4170
int32_t CSimpleFont_IsObjectType(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4220
int32_t CSimpleFont_GetName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4280
int32_t CSimpleFont_SetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A42D0
int32_t CSimpleFont_GetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4350
int32_t CSimpleFont_CopyFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A43A0
int32_t CSimpleFont_SetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A43F0
int32_t CSimpleFont_GetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4440
int32_t CSimpleFont_SetAlpha(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4490
int32_t CSimpleFont_GetAlpha(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4500
int32_t CSimpleFont_SetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4590
int32_t CSimpleFont_GetTextColor(lua_State* L) {
    int32_t type = CSimpleFont::GetObjectType();
    auto font = static_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, type));

    return CSimpleFont::GetTextColor(font->GetDisplayName(), font, L);
}

// OFFSET: 0x4A45E0
int32_t CSimpleFont_SetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4630
int32_t CSimpleFont_GetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4680
int32_t CSimpleFont_SetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A46D0
int32_t CSimpleFont_GetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4720
int32_t CSimpleFont_SetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4770
int32_t CSimpleFont_GetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4AB0
int32_t CSimpleFont_SetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A47E0
int32_t CSimpleFont_GetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4B00
int32_t CSimpleFont_SetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4840
int32_t CSimpleFont_GetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A4B50
int32_t CSimpleFont_SetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A48A0
int32_t CSimpleFont_GetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

FrameScript_Method SimpleFontMethods[NUM_SIMPLE_FONT_SCRIPT_METHODS] = {
    { "GetObjectType",          &CSimpleFont_GetObjectType },
    { "IsObjectType",           &CSimpleFont_IsObjectType },
    { "GetName",                &CSimpleFont_GetName },
    { "SetFontObject",          &CSimpleFont_SetFontObject },
    { "GetFontObject",          &CSimpleFont_GetFontObject },
    { "CopyFontObject",         &CSimpleFont_CopyFontObject },
    { "SetFont",                &CSimpleFont_SetFont },
    { "GetFont",                &CSimpleFont_GetFont },
    { "SetAlpha",               &CSimpleFont_SetAlpha },
    { "GetAlpha",               &CSimpleFont_GetAlpha },
    { "SetTextColor",           &CSimpleFont_SetTextColor },
    { "GetTextColor",           &CSimpleFont_GetTextColor },
    { "SetShadowColor",         &CSimpleFont_SetShadowColor },
    { "GetShadowColor",         &CSimpleFont_GetShadowColor },
    { "SetShadowOffset",        &CSimpleFont_SetShadowOffset },
    { "GetShadowOffset",        &CSimpleFont_GetShadowOffset },
    { "SetSpacing",             &CSimpleFont_SetSpacing },
    { "GetSpacing",             &CSimpleFont_GetSpacing },
    { "SetJustifyH",            &CSimpleFont_SetJustifyH },
    { "GetJustifyH",            &CSimpleFont_GetJustifyH },
    { "SetJustifyV",            &CSimpleFont_SetJustifyV },
    { "GetJustifyV",            &CSimpleFont_GetJustifyV },
    { "SetIndentedWordWrap",    &CSimpleFont_SetIndentedWordWrap },
    { "GetIndentedWordWrap",    &CSimpleFont_GetIndentedWordWrap }
};
