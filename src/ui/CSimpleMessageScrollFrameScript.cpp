#include "ui/CSimpleMessageScrollFrameScript.hpp"
#include "ui/CSimpleMessageScrollFrame.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"

// OFFSET: 0x972860
static int32_t Script_SetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9728C0
static int32_t Script_GetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972920
static int32_t Script_SetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972980
static int32_t Script_GetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9729E0
static int32_t Script_SetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972A40
static int32_t Script_GetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972AA0
static int32_t Script_SetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972B00
static int32_t Script_GetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972B60
static int32_t Script_SetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972BC0
static int32_t Script_GetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972C20
static int32_t Script_SetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972C80
static int32_t Script_GetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972CE0
static int32_t Script_SetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972D40
static int32_t Script_GetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972DA0
static int32_t Script_SetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972E00
static int32_t Script_GetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972F20
static int32_t Script_AddMessage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973150
static int32_t Script_GetMessageInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973270
static int32_t Script_RemoveMessagesByAccessID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9732F0
static int32_t Script_ScrollUp(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973330
static int32_t Script_ScrollDown(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973370
static int32_t Script_PageUp(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9733B0
static int32_t Script_PageDown(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9733F0
static int32_t Script_ScrollToTop(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973430
static int32_t Script_ScrollToBottom(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973470
static int32_t Script_SetScrollOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9734D0
static int32_t Script_AtTop(lua_State* L) {
    auto type = CSimpleMessageScrollFrame::GetObjectType();
    auto frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, type));
    if (frame->m_atTop) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

// OFFSET: 0x973530
static int32_t Script_AtBottom(lua_State* L) {
    auto type = CSimpleMessageScrollFrame::GetObjectType();
    auto frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, type));
    if (frame->m_atBottom) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

// OFFSET: 0x973590
static int32_t Script_UpdateColorByID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973640
static int32_t Script_GetNumMessages(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9736A0
static int32_t Script_GetNumLinesDisplayed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9736F0
static int32_t Script_GetCurrentScroll(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973750
static int32_t Script_GetCurrentLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9737A0
static int32_t Script_GetMaxLines(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9737F0
static int32_t Script_SetMaxLines(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973880
static int32_t Script_SetFading(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9738D0
static int32_t Script_GetFading(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973930
static int32_t Script_SetTimeVisible(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9739B0
static int32_t Script_GetTimeVisible(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973A00
static int32_t Script_SetFadeDuration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973A80
static int32_t Script_GetFadeDuration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973AD0
static int32_t Script_Clear(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973B10
static int32_t Script_SetInsertMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973BD0
static int32_t Script_GetInsertMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972E60
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x972EC0
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973C40
static int32_t Script_SetHyperlinksEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x973CA0
static int32_t Script_GetHyperlinksEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}


FrameScript_Method SimpleMessageScrollFrameMethods[NUM_SIMPLE_MESSAGE_SCROLL_FRAME_SCRIPT_METHODS] = {
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
    { "AddMessage", &Script_AddMessage },
    { "GetMessageInfo", &Script_GetMessageInfo },
    { "RemoveMessagesByAccessID", &Script_RemoveMessagesByAccessID },
    { "ScrollUp", &Script_ScrollUp },
    { "ScrollDown", &Script_ScrollDown },
    { "PageUp", &Script_PageUp },
    { "PageDown", &Script_PageDown },
    { "ScrollToTop", &Script_ScrollToTop },
    { "ScrollToBottom", &Script_ScrollToBottom },
    { "SetScrollOffset", &Script_SetScrollOffset },
    { "AtTop", &Script_AtTop },
    { "AtBottom", &Script_AtBottom },
    { "UpdateColorByID", &Script_UpdateColorByID },
    { "GetNumMessages", &Script_GetNumMessages },
    { "GetNumLinesDisplayed", &Script_GetNumLinesDisplayed },
    { "GetCurrentScroll", &Script_GetCurrentScroll },
    { "GetCurrentLine", &Script_GetCurrentLine },
    { "GetMaxLines", &Script_GetMaxLines },
    { "SetMaxLines", &Script_SetMaxLines },
    { "SetFading", &Script_SetFading },
    { "GetFading", &Script_GetFading },
    { "SetTimeVisible", &Script_SetTimeVisible },
    { "GetTimeVisible", &Script_GetTimeVisible },
    { "SetFadeDuration", &Script_SetFadeDuration },
    { "GetFadeDuration", &Script_GetFadeDuration },
    { "Clear", &Script_Clear },
    { "SetInsertMode", &Script_SetInsertMode },
    { "GetInsertMode", &Script_GetInsertMode },
    { "SetIndentedWordWrap", &Script_SetIndentedWordWrap },
    { "GetIndentedWordWrap", &Script_GetIndentedWordWrap },
    { "SetHyperlinksEnabled", &Script_SetHyperlinksEnabled },
    { "GetHyperlinksEnabled", &Script_GetHyperlinksEnabled }
};
