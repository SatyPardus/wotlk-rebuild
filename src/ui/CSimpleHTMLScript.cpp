#include "ui/CSimpleHTMLScript.hpp"
#include "ui/CSimpleHTML.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include <cstdint>

// OFFSET: 0x974A10
int32_t CSimpleHTML_SetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974A70
int32_t CSimpleHTML_GetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974AD0
int32_t CSimpleHTML_SetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974B30
int32_t CSimpleHTML_GetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974B90
int32_t CSimpleHTML_SetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974BF0
int32_t CSimpleHTML_GetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974C50
int32_t CSimpleHTML_SetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974CB0
int32_t CSimpleHTML_GetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974D10
int32_t CSimpleHTML_SetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974D70
int32_t CSimpleHTML_GetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974DD0
int32_t CSimpleHTML_SetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974E30
int32_t CSimpleHTML_GetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974E90
int32_t CSimpleHTML_SetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974EF0
int32_t CSimpleHTML_GetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974F50
int32_t CSimpleHTML_SetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x974FB0
int32_t CSimpleHTML_GetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975010
int32_t CSimpleHTML_SetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975070
int32_t CSimpleHTML_GetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9750D0
int32_t CSimpleHTML_SetText(lua_State* L) {
    auto type = CSimpleHTML::GetObjectType();
    auto html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, type));

    auto text = lua_tolstring(L, 2, nullptr);
    html->SetText(text, nullptr);

    return 0;
}

// OFFSET: 0x975120
int32_t CSimpleHTML_SetHyperlinkFormat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9751A0
int32_t CSimpleHTML_GetHyperlinkFormat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9751E0
int32_t CSimpleHTML_SetHyperlinksEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975240
int32_t CSimpleHTML_GetHyperlinksEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

FrameScript_Method SimpleHTMLMethods[NUM_SIMPLE_HTML_SCRIPT_METHODS] = {
    { "SetFontObject",              &CSimpleHTML_SetFontObject },
    { "GetFontObject",              &CSimpleHTML_GetFontObject },
    { "SetFont",                    &CSimpleHTML_SetFont },
    { "GetFont",                    &CSimpleHTML_GetFont },
    { "SetTextColor",               &CSimpleHTML_SetTextColor },
    { "GetTextColor",               &CSimpleHTML_GetTextColor },
    { "SetShadowColor",             &CSimpleHTML_SetShadowColor },
    { "GetShadowColor",             &CSimpleHTML_GetShadowColor },
    { "SetShadowOffset",            &CSimpleHTML_SetShadowOffset },
    { "GetShadowOffset",            &CSimpleHTML_GetShadowOffset },
    { "SetSpacing",                 &CSimpleHTML_SetSpacing },
    { "GetSpacing",                 &CSimpleHTML_GetSpacing },
    { "SetJustifyH",                &CSimpleHTML_SetJustifyH },
    { "GetJustifyH",                &CSimpleHTML_GetJustifyH },
    { "SetJustifyV",                &CSimpleHTML_SetJustifyV },
    { "GetJustifyV",                &CSimpleHTML_GetJustifyV },
    { "SetIndentedWordWrap",        &CSimpleHTML_SetIndentedWordWrap },
    { "GetIndentedWordWrap",        &CSimpleHTML_GetIndentedWordWrap },
    { "SetText",                    &CSimpleHTML_SetText },
    { "SetHyperlinkFormat",         &CSimpleHTML_SetHyperlinkFormat },
    { "GetHyperlinkFormat",         &CSimpleHTML_GetHyperlinkFormat },
    { "SetHyperlinksEnabled",       &CSimpleHTML_SetHyperlinksEnabled },
    { "GetHyperlinksEnabled",       &CSimpleHTML_GetHyperlinksEnabled }
};
