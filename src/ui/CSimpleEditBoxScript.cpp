#include "ui/CSimpleEditBoxScript.hpp"
#include "ui/CSimpleEditBox.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include <cstdint>

// OFFSET: 0x975310
int32_t CSimpleEditBox_SetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975370
int32_t CSimpleEditBox_GetFontObject(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9753D0
int32_t CSimpleEditBox_SetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975430
int32_t CSimpleEditBox_GetFont(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975490
int32_t CSimpleEditBox_SetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9754F0
int32_t CSimpleEditBox_GetTextColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975550
int32_t CSimpleEditBox_SetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9755B0
int32_t CSimpleEditBox_GetShadowColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975610
int32_t CSimpleEditBox_SetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975670
int32_t CSimpleEditBox_GetShadowOffset(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9756D0
int32_t CSimpleEditBox_SetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975730
int32_t CSimpleEditBox_GetSpacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975790
int32_t CSimpleEditBox_SetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9757F0
int32_t CSimpleEditBox_GetJustifyH(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975850
int32_t CSimpleEditBox_SetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9758B0
int32_t CSimpleEditBox_GetJustifyV(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975910
int32_t CSimpleEditBox_SetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975970
int32_t CSimpleEditBox_GetIndentedWordWrap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9759D0
int32_t CSimpleEditBox_SetAutoFocus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975A20
int32_t CSimpleEditBox_IsAutoFocus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975A80
int32_t CSimpleEditBox_SetCountInvisibleLetters(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975AD0
int32_t CSimpleEditBox_IsCountInvisibleLetters(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975B30
int32_t CSimpleEditBox_SetMultiLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975B80
int32_t CSimpleEditBox_IsMultiLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975BE0
int32_t CSimpleEditBox_SetNumeric(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975C30
int32_t CSimpleEditBox_IsNumeric(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975C90
int32_t CSimpleEditBox_SetPassword(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975CE0
int32_t CSimpleEditBox_IsPassword(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975D40
int32_t CSimpleEditBox_SetBlinkSpeed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975DC0
int32_t CSimpleEditBox_GetBlinkSpeed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975E10
int32_t CSimpleEditBox_Insert(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x975E80
int32_t CSimpleEditBox_SetText(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    CSimpleEditBox* editBox = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: %s:SetText(\"text\")", editBox->GetDisplayName());
    }

    const char* tainted = nullptr; //*lua_tainted TODO;
    editBox->SetText(lua_tolstring(L, 2, 0), tainted);
    return 0;
}

// OFFSET: 0x975F10
int32_t CSimpleEditBox_GetText(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    CSimpleEditBox* editBox = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    // TODO
    // - taint management
    // if (editBox->m_dwordC && lua_taintexpected && !lua_taintedclosure) {
    //     lua_tainted = editBox->simpleeditbox_dwordC;
    // }

    lua_pushstring(L, editBox->m_text);

    return 1;
}

// OFFSET: 0x975F80
int32_t CSimpleEditBox_SetNumber(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976010
int32_t CSimpleEditBox_GetNumber(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976080
int32_t CSimpleEditBox_HighlightText(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    CSimpleEditBox* editBox = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    int32_t v2 = 0;
    if (lua_isnumber(L, 2))
        v2 = lua_tonumber(L, 2);
    int32_t v3 = -1;
    if (lua_isnumber(L, 3))
        v3 = lua_tonumber(L, 3);
    editBox->HighlightText(v2, v3);
    return 0;
}

// OFFSET: 0x976110
int32_t CSimpleEditBox_AddHistoryLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9761A0
int32_t CSimpleEditBox_ClearHistory(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9761E0
int32_t CSimpleEditBox_SetTextInsets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976330
int32_t CSimpleEditBox_GetTextInsets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976410
int32_t CSimpleEditBox_SetFocus(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    CSimpleEditBox::SetKeyboardFocus(object);
    return 0;
}

// OFFSET: 0x976450
int32_t CSimpleEditBox_ClearFocus(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    CSimpleEditBox::ClearKeyboardFocus(object, true);
    return 0;
}

// OFFSET: 0x976490
int32_t CSimpleEditBox_HasFocus(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    lua_pushboolean(L, object->IsCurrentFocus());
    return 1;
}

// OFFSET: 0x9764E0
int32_t CSimpleEditBox_SetMaxBytes(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "Usage: %s:SetMaxBytes(max)", object->GetDisplayName());
    }

    int32_t n = lua_tonumber(L, 2);
    if (n <= 0)
        object->m_textLengthMax = -1;
    else
        object->m_textLengthMax = n - 1;
    return 0;
}

// OFFSET: 0x976580
int32_t CSimpleEditBox_GetMaxBytes(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    lua_pushnumber(L, object->m_textLengthMax + 1);
    return 1;
}

// OFFSET: 0x9765D0
int32_t CSimpleEditBox_SetMaxLetters(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "Usage: %s:SetMaxLetters(max)", object->GetDisplayName());
    }

    object->m_textLettersMax = lua_tonumber(L, 2);
    return 0;
}

// OFFSET: 0x976650
int32_t CSimpleEditBox_GetMaxLetters(lua_State* L) {
    int32_t type = CSimpleEditBox::GetObjectType();
    auto object = static_cast<CSimpleEditBox*>(FrameScript_GetObjectThis(L, type));

    lua_pushnumber(L, object->m_textLettersMax);
    return 1;
}

// OFFSET: 0x9766A0
int32_t CSimpleEditBox_GetNumLetters(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976720
int32_t CSimpleEditBox_GetHistoryLines(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976770
int32_t CSimpleEditBox_SetHistoryLines(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976800
int32_t CSimpleEditBox_GetInputLanguage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976850
int32_t CSimpleEditBox_ToggleInputLanguage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976890
int32_t CSimpleEditBox_SetAltArrowKeyMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9768E0
int32_t CSimpleEditBox_GetAltArrowKeyMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976940
int32_t CSimpleEditBox_IsInIMECompositionMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x9769A0
int32_t CSimpleEditBox_SetCursorPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976A20
int32_t CSimpleEditBox_GetCursorPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x976A70
int32_t CSimpleEditBox_GetUTF8CursorPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

FrameScript_Method SimpleEditBoxMethods[NUM_SIMPLE_EDIT_BOX_SCRIPT_METHODS] = {
    { "SetFontObject",                  &CSimpleEditBox_SetFontObject },
    { "GetFontObject",                  &CSimpleEditBox_GetFontObject },
    { "SetFont",                        &CSimpleEditBox_SetFont },
    { "GetFont",                        &CSimpleEditBox_GetFont },
    { "SetTextColor",                   &CSimpleEditBox_SetTextColor },
    { "GetTextColor",                   &CSimpleEditBox_GetTextColor },
    { "SetShadowColor",                 &CSimpleEditBox_SetShadowColor },
    { "GetShadowColor",                 &CSimpleEditBox_GetShadowColor },
    { "SetShadowOffset",                &CSimpleEditBox_SetShadowOffset },
    { "GetShadowOffset",                &CSimpleEditBox_GetShadowOffset },
    { "SetSpacing",                     &CSimpleEditBox_SetSpacing },
    { "GetSpacing",                     &CSimpleEditBox_GetSpacing },
    { "SetJustifyH",                    &CSimpleEditBox_SetJustifyH },
    { "GetJustifyH",                    &CSimpleEditBox_GetJustifyH },
    { "SetJustifyV",                    &CSimpleEditBox_SetJustifyV },
    { "GetJustifyV",                    &CSimpleEditBox_GetJustifyV },
    { "SetIndentedWordWrap",            &CSimpleEditBox_SetIndentedWordWrap },
    { "GetIndentedWordWrap",            &CSimpleEditBox_GetIndentedWordWrap },
    { "SetAutoFocus",                   &CSimpleEditBox_SetAutoFocus },
    { "IsAutoFocus",                    &CSimpleEditBox_IsAutoFocus },
    { "SetCountInvisibleLetters",       &CSimpleEditBox_SetCountInvisibleLetters },
    { "IsCountInvisibleLetters",        &CSimpleEditBox_IsCountInvisibleLetters },
    { "SetMultiLine",                   &CSimpleEditBox_SetMultiLine },
    { "IsMultiLine",                    &CSimpleEditBox_IsMultiLine },
    { "SetNumeric",                     &CSimpleEditBox_SetNumeric },
    { "IsNumeric",                      &CSimpleEditBox_IsNumeric },
    { "SetPassword",                    &CSimpleEditBox_SetPassword },
    { "IsPassword",                     &CSimpleEditBox_IsPassword },
    { "SetBlinkSpeed",                  &CSimpleEditBox_SetBlinkSpeed },
    { "GetBlinkSpeed",                  &CSimpleEditBox_GetBlinkSpeed },
    { "Insert",                         &CSimpleEditBox_Insert },
    { "SetText",                        &CSimpleEditBox_SetText },
    { "GetText",                        &CSimpleEditBox_GetText },
    { "SetNumber",                      &CSimpleEditBox_SetNumber },
    { "GetNumber",                      &CSimpleEditBox_GetNumber },
    { "HighlightText",                  &CSimpleEditBox_HighlightText },
    { "AddHistoryLine",                 &CSimpleEditBox_AddHistoryLine },
    { "ClearHistory",                   &CSimpleEditBox_ClearHistory },
    { "SetTextInsets",                  &CSimpleEditBox_SetTextInsets },
    { "GetTextInsets",                  &CSimpleEditBox_GetTextInsets },
    { "SetFocus",                       &CSimpleEditBox_SetFocus },
    { "ClearFocus",                     &CSimpleEditBox_ClearFocus },
    { "HasFocus",                       &CSimpleEditBox_HasFocus },
    { "SetMaxBytes",                    &CSimpleEditBox_SetMaxBytes },
    { "GetMaxBytes",                    &CSimpleEditBox_GetMaxBytes },
    { "SetMaxLetters",                  &CSimpleEditBox_SetMaxLetters },
    { "GetMaxLetters",                  &CSimpleEditBox_GetMaxLetters },
    { "GetNumLetters",                  &CSimpleEditBox_GetNumLetters },
    { "GetHistoryLines",                &CSimpleEditBox_GetHistoryLines },
    { "SetHistoryLines",                &CSimpleEditBox_SetHistoryLines },
    { "GetInputLanguage",               &CSimpleEditBox_GetInputLanguage },
    { "ToggleInputLanguage",            &CSimpleEditBox_ToggleInputLanguage },
    { "SetAltArrowKeyMode",             &CSimpleEditBox_SetAltArrowKeyMode },
    { "GetAltArrowKeyMode",             &CSimpleEditBox_GetAltArrowKeyMode },
    { "IsInIMECompositionMode",         &CSimpleEditBox_IsInIMECompositionMode },
    { "SetCursorPosition",              &CSimpleEditBox_SetCursorPosition },
    { "GetCursorPosition",              &CSimpleEditBox_GetCursorPosition },
    { "GetUTF8CursorPosition",          &CSimpleEditBox_GetUTF8CursorPosition }
};
