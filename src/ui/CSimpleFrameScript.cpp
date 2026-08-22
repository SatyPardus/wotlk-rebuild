#include "ui/CSimpleFrameScript.hpp"
#include "ui/CSimpleFrame.hpp"
#include "ui/CSimpleTexture.hpp"
#include "gx/Coordinate.hpp"
#include "ui/FrameScript.hpp"
#include "ui/FrameXML.hpp"
#include "ui/CBackdropGenerator.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include "util/StringTo.hpp"
#include <bc/Memory.hpp>
#include <algorithm>
#include <cstdint>
#include <limits>


// OFFSET: 0x49E5B0
int32_t CSimpleFrame_GetTitleRegion(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49E630
int32_t CSimpleFrame_CreateTitleRegion(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A2010
int32_t CSimpleFrame_CreateTexture(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    const char* name = lua_isstring(L, 2) ? lua_tolstring(L, 2, nullptr) : nullptr;

    int32_t drawLayer = 2;
    if (lua_isstring(L, 3)) {
        StringToDrawLayer(lua_tolstring(L, 3, nullptr), drawLayer);
    }

    XMLNode* frameNode = nullptr;

    if (lua_type(L, 4) == LUA_TSTRING) {
        const char* tainted;
        bool locked;

        const char* inheritName = lua_tolstring(L, 4, nullptr);
        const char* frameName = frame->GetName();
        if (!frameName) {
            frameName = "<unnamed>";
        }

        frameNode = FrameXML_AcquireHashNode(inheritName, tainted, locked);
        if (!frameNode) {
            luaL_error(L, "%s:CreateTexture(): Couldn't find inherited node \"%s\"", frameName, inheritName);
        }

        if (locked) {
            luaL_error(L, "%s:CreateTexture(): Recursively inherited node \"%s\"", frameName, inheritName);
        }
    }

    auto texture = NEW(CSimpleTexture, frame, drawLayer, 1);
    if (name && *name) {
        texture->SetName(name);
    }

    if (frameNode) {
        CStatus status;
        texture->PostLoadXML(frameNode, &status);
        FrameXML_ReleaseHashNode(lua_tolstring(L, 4, nullptr));
    }

    // TODO

    if (!texture->lua_registered) {
        texture->RegisterScriptObject(nullptr);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, texture->lua_objectRef);
    return 1;
}

// OFFSET: 0x4A2240
int32_t CSimpleFrame_CreateFontString(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49E700
int32_t CSimpleFrame_GetBoundsRect(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    CRect bounds = {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        0.0f,
        0.0f
    };

    if (!frame->GetBoundsRect(bounds)) {
        return 0;
    }

    float ooScale = 1.0f / frame->m_layoutScale;

    float ddcTop = CoordinateGetAspectCompensation() * 1024.0f * ooScale * bounds.minX;
    float ndcTop = DDCToNDCWidth(ddcTop);
    lua_pushnumber(L, ndcTop);

    float ddcLeft = CoordinateGetAspectCompensation() * 1024.0f * ooScale * bounds.minY;
    float ndcLeft = DDCToNDCWidth(ddcLeft);
    lua_pushnumber(L, ndcLeft);

    float ddcWidth = CoordinateGetAspectCompensation() * 1024.0f * ooScale * (bounds.maxX - bounds.minX);
    float ndcWidth = DDCToNDCWidth(ddcWidth);
    lua_pushnumber(L, ndcWidth);

    float ddcHeight = CoordinateGetAspectCompensation() * 1024.0f * ooScale * (bounds.maxY - bounds.minY);
    float ndcHeight = DDCToNDCWidth(ddcHeight);
    lua_pushnumber(L, ndcHeight);

    return 4;
}

// OFFSET: 0x4A1E80
int32_t CSimpleFrame_GetNumRegions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1F10
int32_t CSimpleFrame_GetRegions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A2490
int32_t CSimpleFrame_GetNumChildren(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A2510
int32_t CSimpleFrame_GetChildren(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49E830
int32_t CSimpleFrame_GetFrameStrata(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49E880
int32_t CSimpleFrame_SetFrameStrata(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49E980
int32_t CSimpleFrame_GetFrameLevel(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    lua_pushnumber(L, frame->m_level);

    return 1;
}

// OFFSET: 0x49E9D0
int32_t CSimpleFrame_SetFrameLevel(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!frame->ProtectedFunctionsAllowed()) {
        // TODO
        // - disallowed logic

        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        return luaL_error(L, "Usage: %s:SetFrameLevel(level)", frame->GetDisplayName());
    }

    int32_t level = lua_tonumber(L, 2);

    if (level < 0) {
        return luaL_error(L, "%s:SetFrameLevel(): Passed negative frame level: %d", frame->GetDisplayName(), level);
    }

    frame->SetFrameLevel(level, 1);

    return 0;
}

// OFFSET: 0x49EAB0
int32_t CSimpleFrame_HasScript(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49EB70
int32_t CSimpleFrame_GetScript(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!lua_isstring(L, 2)) {
        return luaL_error(L, "Usage: %s:GetScript(\"type\")", frame->GetDisplayName());
    }

    auto scriptName = lua_tolstring(L, 2, nullptr);

    FrameScript_Object::ScriptData scriptData;
    auto script = frame->GetScriptByName(scriptName, scriptData);
    if (!script) {
        return luaL_error(L, "%s doesn't have a \"%s\" script", frame->GetDisplayName(), scriptName);
    }

    // TODO: if (script->unk && lua_taintexpected && !lua_taintedclosure )

    if (script->luaRef <= 0) {
        lua_pushnil(L);
    } else {
        lua_rawgeti(L, LUA_REGISTRYINDEX, script->luaRef);
    }
    return 1;
}

// OFFSET: 0x49EC80
int32_t CSimpleFrame_SetScript(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!lua_isstring(L, 2) || (lua_type(L, 3) != LUA_TFUNCTION && lua_type(L, 3) != LUA_TNIL)) {
        return luaL_error(L, "Usage: %s:SetScript(\"type\", function)", frame->GetDisplayName());
    }

    auto scriptName = lua_tolstring(L, 2, nullptr);

    FrameScript_Object::ScriptData scriptData;
    auto script = frame->GetScriptByName(scriptName, scriptData);
    if (!script) {
        return luaL_error(L, "%s doesn't have a \"%s\" script", frame->GetDisplayName(), scriptName);
    }

    if (script->luaRef) {
        luaL_unref(L, LUA_REGISTRYINDEX, script->luaRef);
    }

    auto ref = luaL_ref(L, LUA_REGISTRYINDEX);
    script->luaRef = ref <= 0 ? 0 : ref;
    // TODO: script->unk = lua_tainted;
    return 0;
}

// OFFSET: 0x49EDB0
int32_t CSimpleFrame_HookScript(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49EFE0
int32_t CSimpleFrame_RegisterEvent(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!lua_isstring(L, 2)) {
        return luaL_error(L, "Usage: %s:RegisterEvent(\"event\")", frame->GetDisplayName());
    }

    const char* event = lua_tolstring(L, 2, 0);

    frame->RegisterScriptEvent(event);

    return 0;
}

// OFFSET: 0x49F060
int32_t CSimpleFrame_UnregisterEvent(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!lua_isstring(L, 2)) {
        return luaL_error(L, "Usage: %s:UnregisterEvent(\"event\")", frame->GetDisplayName());
    }
    frame->UnregisterScriptEvent(lua_tolstring(L, 2, 0));
    return 0;
}

// OFFSET: 0x49F0E0
int32_t CSimpleFrame_RegisterAllEvents(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F120
int32_t CSimpleFrame_UnregisterAllEvents(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F160
int32_t CSimpleFrame_IsEventRegistered(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F210
int32_t CSimpleFrame_AllowAttributeChanges(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F260
int32_t CSimpleFrame_CanChangeAttributes(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F2D0
int32_t CSimpleFrame_GetAttribute(lua_State* L) {
    auto type = CSimpleFrame::GetObjectType();
    auto frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (lua_gettop(L) == 4 && lua_isstring(L, 3)) {
        size_t prefixLength;
        size_t nameLength;
        size_t suffixLength;
        auto prefix = lua_tolstring(L, 2, &prefixLength);
        auto name = lua_tolstring(L, 3, &nameLength);
        auto suffix = lua_tolstring(L, 4, &suffixLength);

        int32_t luaRef = -1;
        char fullName[256];

        size_t offset = 0;
        offset += SStrNCopy(&fullName[offset], prefix, prefixLength, sizeof(fullName) - offset);
        offset += SStrNCopy(&fullName[offset], name, nameLength, sizeof(fullName) - offset);
        offset += SStrNCopy(&fullName[offset], suffix, suffixLength, sizeof(fullName) - offset);

        if (frame->GetAttribute(fullName, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        offset = 0;
        offset += SStrNCopy(&fullName[offset], prefix, prefixLength, sizeof(fullName) - offset - 1);
        offset += SStrNCopy(&fullName[offset], name, nameLength, sizeof(fullName) - offset - 1);
        fullName[offset++] = '*';
        fullName[offset++] = '\0';

        if (frame->GetAttribute(fullName, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        offset = 0;
        fullName[offset++] = '*';
        offset += SStrNCopy(&fullName[offset], name, nameLength, sizeof(fullName) - offset - 1);
        fullName[offset++] = '*';
        fullName[offset++] = '\0';

        if (frame->GetAttribute(fullName, luaRef) || frame->GetAttribute(name, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        lua_pushnil(L);
    } else {
        if (!lua_isstring(L, 2)) {
            return luaL_error(L, "Usage: %s:GetAttribute(\"name\")", frame->GetDisplayName());
        }

        auto name = lua_tolstring(L, 2, nullptr);
        int32_t luaRef = -1;
        if (frame->GetAttribute(name, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
        } else {
            lua_pushnil(L);
        }
    }
    return 1;
}

// OFFSET: 0x49F610
int32_t CSimpleFrame_SetAttribute(lua_State* L) {
    auto type = CSimpleFrame::GetObjectType();
    auto frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!frame->ProtectedFunctionsAllowed() && !frame->AttributeChangesAllowed()) {
        // TODO: CSimpleTop::s_instance->dword1254(); // fptr call
        return 0;
    }

    lua_settop(L, 3);
    if (!lua_isstring(L, 2) || lua_type(L, 3) == LUA_TNONE) {
        return luaL_error(L, "Usage: %s:SetAttribute(\"name\", value)", frame->GetDisplayName());
    }

    int32_t luaRef = -1;

    auto name = lua_tolstring(L, 2, nullptr);
    if (frame->GetAttribute(name, luaRef)) {
        luaL_unref(L, LUA_REGISTRYINDEX, luaRef);
    }

    // TODO: LUA Tainted
    luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
    frame->SetAttribute(name, luaRef);

    return 0;
}

// OFFSET: 0x49F790
int32_t CSimpleFrame_GetEffectiveScale(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F7D0
int32_t CSimpleFrame_GetScale(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F820
int32_t CSimpleFrame_SetScale(lua_State* L) {
    auto type = CSimpleFrame::GetObjectType();
    auto frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!frame->ProtectedFunctionsAllowed()) {
        // TODO
        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: %s:SetScale(scale)", frame->GetDisplayName());
    }

    float scale = lua_tonumber(L, 2);
    if (scale <= 0.0f) {
        luaL_error(L, "%s:SetScale(): Scale must be > 0", frame->GetDisplayName());
    }

    frame->SetFrameScale(scale, false);
    return 0;
}

// OFFSET: 0x49F900
int32_t CSimpleFrame_GetEffectiveAlpha(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F980
int32_t CSimpleFrame_GetAlpha(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49F9E0
int32_t CSimpleFrame_SetAlpha(lua_State* L) {
    auto type = CSimpleFrame::GetObjectType();
    auto frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: %s:SetAlpha(alpha 0 to 1)", frame->GetDisplayName());
    }

    float alpha = lua_tonumber(L, 2);
    alpha = std::max(std::min(alpha, 1.0f), 0.0f);
    frame->SetFrameAlpha(static_cast<uint8_t>(alpha * 255.0f));

    return 0;
}

// OFFSET: 0x49FAB0
int32_t CSimpleFrame_GetID(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    lua_pushnumber(L, frame->m_id);

    return 1;
}

// OFFSET: 0x49FB00
int32_t CSimpleFrame_SetID(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!frame->ProtectedFunctionsAllowed()) {
        // TODO
        // - disallowed logic

        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        return luaL_error(L, "Usage: %s:SetID(ID)", frame->GetDisplayName());
    }

    frame->m_id = lua_tonumber(L, 2);

    return 0;
}

// OFFSET: 0x49FBB0
int32_t CSimpleFrame_SetToplevel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49FC30
int32_t CSimpleFrame_IsToplevel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49FC90
int32_t CSimpleFrame_EnableDrawLayer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49FD00
int32_t CSimpleFrame_DisableDrawLayer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49FD70
int32_t CSimpleFrame_Show(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (frame->ProtectedFunctionsAllowed()) {
        frame->Show();
    } else {
        // TODO
        // - disallowed logic
    }

    return 0;
}

// OFFSET: 0x49FDD0
int32_t CSimpleFrame_Hide(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (frame->ProtectedFunctionsAllowed()) {
        frame->Hide();
    } else {
        // TODO
        // - disallowed logic
    }

    return 0;
}

// OFFSET: 0x49FE30
int32_t CSimpleFrame_IsVisible(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (frame->m_visible) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x49FE90
int32_t CSimpleFrame_IsShown(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (frame->m_shown) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x49FEF0
int32_t CSimpleFrame_Raise(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!frame->ProtectedFunctionsAllowed()) {
        // TODO
        // - disallowed logic

        return 0;
    }

    frame->Raise();

    return 0;
}

// OFFSET: 0x49FF50
int32_t CSimpleFrame_Lower(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x49FFB0
int32_t CSimpleFrame_GetHitRectInsets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0090
int32_t CSimpleFrame_SetHitRectInsets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0230
int32_t CSimpleFrame_GetClampRectInsets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0310
int32_t CSimpleFrame_SetClampRectInsets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0480
int32_t CSimpleFrame_GetMinResize(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0520
int32_t CSimpleFrame_SetMinResize(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0640
int32_t CSimpleFrame_GetMaxResize(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A06E0
int32_t CSimpleFrame_SetMaxResize(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0800
int32_t CSimpleFrame_SetMovable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0850
int32_t CSimpleFrame_IsMovable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A08C0
int32_t CSimpleFrame_SetDontSavePosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0910
int32_t CSimpleFrame_GetDontSavePosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0980
int32_t CSimpleFrame_SetResizable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A09D0
int32_t CSimpleFrame_IsResizable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0A40
int32_t CSimpleFrame_StartMoving(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0B10
int32_t CSimpleFrame_StartSizing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0C20
int32_t CSimpleFrame_StopMovingOrSizing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0C70
int32_t CSimpleFrame_SetUserPlaced(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0D00
int32_t CSimpleFrame_IsUserPlaced(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0D70
int32_t CSimpleFrame_SetClampedToScreen(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0DC0
int32_t CSimpleFrame_IsClampedToScreen(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0E20
int32_t CSimpleFrame_RegisterForDrag(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0EC0
int32_t CSimpleFrame_EnableKeyboard(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    if (!frame->ProtectedFunctionsAllowed()) {
        // TODO
        // - disallowed logic

        return 0;
    }

    if (StringToBOOL(L, 2, 1)) {
        frame->EnableEvent(SIMPLE_EVENT_KEY, -1);
        frame->EnableEvent(SIMPLE_EVENT_CHAR, -1);
    } else {
        frame->DisableEvent(SIMPLE_EVENT_KEY);
        frame->DisableEvent(SIMPLE_EVENT_CHAR);
    }
}

// OFFSET: 0x4A0F60
int32_t CSimpleFrame_IsKeyboardEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A0FD0
int32_t CSimpleFrame_EnableMouse(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1060
int32_t CSimpleFrame_IsMouseEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A10D0
int32_t CSimpleFrame_EnableMouseWheel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1160
int32_t CSimpleFrame_IsMouseWheelEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A11D0
int32_t CSimpleFrame_EnableJoystick(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1260
int32_t CSimpleFrame_IsJoystickEnabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A12D0
int32_t CSimpleFrame_GetBackdrop(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A15A0
int32_t CSimpleFrame_SetBackdrop(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A19A0
int32_t CSimpleFrame_GetBackdropColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1A80
int32_t CSimpleFrame_SetBackdropColor(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    auto object = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    CImVector color = { 0 };
    FrameScript_GetColor(L, 2, color);

    if (object->m_backdrop) {
        object->m_backdrop->SetVertexColor(color);
    }

    return 0;
}

// OFFSET: 0x4A1AF0
int32_t CSimpleFrame_GetBackdropBorderColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1BD0
int32_t CSimpleFrame_SetBackdropBorderColor(lua_State* L) {
    int32_t type = CSimpleFrame::GetObjectType();
    auto object = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, type));

    CImVector color = { 0 };
    FrameScript_GetColor(L, 2, color);

    if (object->m_backdrop) {
        object->m_backdrop->SetBorderVertexColor(color);
    }

    return 0;
}

// OFFSET: 0x4A1C40
int32_t CSimpleFrame_SetDepth(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1CC0
int32_t CSimpleFrame_GetDepth(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1D20
int32_t CSimpleFrame_GetEffectiveDepth(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1D80
int32_t CSimpleFrame_IgnoreDepth(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x4A1E00
int32_t CSimpleFrame_IsIgnoringDepth(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

FrameScript_Method SimpleFrameMethods[NUM_SIMPLE_FRAME_SCRIPT_METHODS] = {
    { "GetTitleRegion",             &CSimpleFrame_GetTitleRegion },
    { "CreateTitleRegion",          &CSimpleFrame_CreateTitleRegion },
    { "CreateTexture",              &CSimpleFrame_CreateTexture },
    { "CreateFontString",           &CSimpleFrame_CreateFontString },
    { "GetBoundsRect",              &CSimpleFrame_GetBoundsRect },
    { "GetNumRegions",              &CSimpleFrame_GetNumRegions },
    { "GetRegions",                 &CSimpleFrame_GetRegions },
    { "GetNumChildren",             &CSimpleFrame_GetNumChildren },
    { "GetChildren",                &CSimpleFrame_GetChildren },
    { "GetFrameStrata",             &CSimpleFrame_GetFrameStrata },
    { "SetFrameStrata",             &CSimpleFrame_SetFrameStrata },
    { "GetFrameLevel",              &CSimpleFrame_GetFrameLevel },
    { "SetFrameLevel",              &CSimpleFrame_SetFrameLevel },
    { "HasScript",                  &CSimpleFrame_HasScript },
    { "GetScript",                  &CSimpleFrame_GetScript },
    { "SetScript",                  &CSimpleFrame_SetScript },
    { "HookScript",                 &CSimpleFrame_HookScript },
    { "RegisterEvent",              &CSimpleFrame_RegisterEvent },
    { "UnregisterEvent",            &CSimpleFrame_UnregisterEvent },
    { "RegisterAllEvents",          &CSimpleFrame_RegisterAllEvents },
    { "UnregisterAllEvents",        &CSimpleFrame_UnregisterAllEvents },
    { "IsEventRegistered",          &CSimpleFrame_IsEventRegistered },
    { "AllowAttributeChanges",      &CSimpleFrame_AllowAttributeChanges },
    { "CanChangeAttribute",         &CSimpleFrame_CanChangeAttributes },
    { "GetAttribute",               &CSimpleFrame_GetAttribute },
    { "SetAttribute",               &CSimpleFrame_SetAttribute },
    { "GetEffectiveScale",          &CSimpleFrame_GetEffectiveScale },
    { "GetScale",                   &CSimpleFrame_GetScale },
    { "SetScale",                   &CSimpleFrame_SetScale },
    { "GetEffectiveAlpha",          &CSimpleFrame_GetEffectiveAlpha },
    { "GetAlpha",                   &CSimpleFrame_GetAlpha },
    { "SetAlpha",                   &CSimpleFrame_SetAlpha },
    { "GetID",                      &CSimpleFrame_GetID },
    { "SetID",                      &CSimpleFrame_SetID },
    { "SetToplevel",                &CSimpleFrame_SetToplevel },
    { "IsToplevel",                 &CSimpleFrame_IsToplevel },
    { "EnableDrawLayer",            &CSimpleFrame_EnableDrawLayer },
    { "DisableDrawLayer",           &CSimpleFrame_DisableDrawLayer },
    { "Show",                       &CSimpleFrame_Show },
    { "Hide",                       &CSimpleFrame_Hide },
    { "IsVisible",                  &CSimpleFrame_IsVisible },
    { "IsShown",                    &CSimpleFrame_IsShown },
    { "Raise",                      &CSimpleFrame_Raise },
    { "Lower",                      &CSimpleFrame_Lower },
    { "GetHitRectInsets",           &CSimpleFrame_GetHitRectInsets },
    { "SetHitRectInsets",           &CSimpleFrame_SetHitRectInsets },
    { "GetClampRectInsets",         &CSimpleFrame_GetClampRectInsets },
    { "SetClampRectInsets",         &CSimpleFrame_SetClampRectInsets },
    { "GetMinResize",               &CSimpleFrame_GetMinResize },
    { "SetMinResize",               &CSimpleFrame_SetMinResize },
    { "GetMaxResize",               &CSimpleFrame_GetMaxResize },
    { "SetMaxResize",               &CSimpleFrame_SetMaxResize },
    { "SetMovable",                 &CSimpleFrame_SetMovable },
    { "IsMovable",                  &CSimpleFrame_IsMovable },
    { "SetDontSavePosition",        &CSimpleFrame_SetDontSavePosition },
    { "GetDontSavePosition",        &CSimpleFrame_GetDontSavePosition },
    { "SetResizable",               &CSimpleFrame_SetResizable },
    { "IsResizable",                &CSimpleFrame_IsResizable },
    { "StartMoving",                &CSimpleFrame_StartMoving },
    { "StartSizing",                &CSimpleFrame_StartSizing },
    { "StopMovingOrSizing",         &CSimpleFrame_StopMovingOrSizing },
    { "SetUserPlaced",              &CSimpleFrame_SetUserPlaced },
    { "IsUserPlaced",               &CSimpleFrame_IsUserPlaced },
    { "SetClampedToScreen",         &CSimpleFrame_SetClampedToScreen },
    { "IsClampedToScreen",          &CSimpleFrame_IsClampedToScreen },
    { "RegisterForDrag",            &CSimpleFrame_RegisterForDrag },
    { "EnableKeyboard",             &CSimpleFrame_EnableKeyboard },
    { "IsKeyboardEnabled",          &CSimpleFrame_IsKeyboardEnabled },
    { "EnableMouse",                &CSimpleFrame_EnableMouse },
    { "IsMouseEnabled",             &CSimpleFrame_IsMouseEnabled },
    { "EnableMouseWheel",           &CSimpleFrame_EnableMouseWheel },
    { "IsMouseWheelEnabled",        &CSimpleFrame_IsMouseWheelEnabled },
    { "EnableJoystick",             &CSimpleFrame_EnableJoystick },
    { "IsJoystickEnabled",          &CSimpleFrame_IsJoystickEnabled },
    { "GetBackdrop",                &CSimpleFrame_GetBackdrop },
    { "SetBackdrop",                &CSimpleFrame_SetBackdrop },
    { "GetBackdropColor",           &CSimpleFrame_GetBackdropColor },
    { "SetBackdropColor",           &CSimpleFrame_SetBackdropColor },
    { "GetBackdropBorderColor",     &CSimpleFrame_GetBackdropBorderColor },
    { "SetBackdropBorderColor",     &CSimpleFrame_SetBackdropBorderColor },
    { "SetDepth",                   &CSimpleFrame_SetDepth },
    { "GetDepth",                   &CSimpleFrame_GetDepth },
    { "GetEffectiveDepth",          &CSimpleFrame_GetEffectiveDepth },
    { "IgnoreDepth",                &CSimpleFrame_IgnoreDepth },
    { "IsIgnoringDepth",            &CSimpleFrame_IsIgnoringDepth }
};
