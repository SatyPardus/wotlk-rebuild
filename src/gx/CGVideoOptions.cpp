#include "gx/CGVideoOptions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Unimplemented.hpp"
#include <tempest/vector/C2iVector.hpp>
#include "console/CVar.hpp"
#include "gx/CGxMonitorMode.hpp"
#include "gx/Device.hpp"
#include "util/Lua.hpp"
#include <os/System.hpp>

static TSGrowableArray<C2iVector> s_resolutions;

void SetupFormats() {
    if (s_resolutions.Count())
        return;

    auto widescreen = CVar::Lookup("widescreen");
    if (!widescreen || !widescreen->GetInt()) {
        C2iVector resolutions[4] = {
            C2iVector(800, 600),
            C2iVector(1024, 768),
            C2iVector(1280, 1024),
            C2iVector(1600, 1200)
        };
        s_resolutions.Add(4, resolutions);
        return;
    }

    TSGrowableArray<CGxMonitorMode> modes;
    GxAdapterMonitorModes(modes);
    int32_t x = 0;
    int32_t y = 0;
    for (int32_t i = 0; i < modes.Count(); i++) {
        float aspect = (float)modes[i].size.x / modes[i].size.y;
        if (aspect >= 1.248f && modes[i].size.x > 640 && modes[i].size.y > 480 && (modes[i].size.x != x || modes[i].size.y != y)) {
            s_resolutions.Add(1, &modes[i].size);
            x = modes[i].size.x;
            y = modes[i].size.y;
        }
    }

    bool v5 = true;
    if (s_resolutions.Count())
        v5 = false;
    modes.Clear();

    if (v5) {
        C2iVector resolutions[4] = {
            C2iVector(800, 600),
            C2iVector(1024, 768),
            C2iVector(1280, 1024),
            C2iVector(1600, 1200)
        };
        s_resolutions.Add(4, resolutions);
    }
}

int32_t Script_GetScreenResolutions(lua_State* L) {
    SetupFormats();
    lua_checkstack(L, s_resolutions.Count());
    char res[64];
    for (int32_t i = 0; i < s_resolutions.Count(); i++) {
        SStrPrintf(res, 64, "%dx%d", s_resolutions[i].x, s_resolutions[i].y);
        lua_pushstring(L, res);
    }
    return s_resolutions.Count();
}

int32_t Script_GetCurrentResolution(lua_State* L) {
    SetupFormats();
    auto v1 = CVar::Lookup("gxResolution");
    if (!v1 || !v1->GetString() || !s_resolutions.Count()) {
        lua_pushnumber(L, 1);
        return 1;
    }

    int32_t x;
    char v8;
    int32_t y;
    if (sscanf(v1->GetString(), "%d%c%d", &x, &v8, &y)) {
        lua_pushnumber(L, 1);
        return 1;
    }
    for (int32_t i = 0; i < s_resolutions.Count(); i++) {
        if (s_resolutions[i].x == x && s_resolutions[i].y == y) {
            lua_pushnumber(L, i + 1);
            return 1;
        }
    }

    lua_pushnumber(L, 1);
    return 1;
}

int32_t Script_SetScreenResolution(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Script_GetRefreshRates(lua_State* L) {
    // TODO
    lua_pushnumber(L, 60);
    return 1;
}

int32_t Script_SetupFullscreenScale(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Script_GetMultisampleFormats(lua_State* L) {
    // TODO
    return 0;
}

int32_t Script_GetCurrentMultisampleFormat(lua_State* L) {
    // TODO
    lua_pushnumber(L, 1.0);
    return 1;
}

int32_t Script_SetMultisampleFormat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Script_GetVideoCaps(lua_State* L) {
    auto v1 = g_theGxDevicePtr->Caps();
    if (v1.m_texFilterAnisotropic)
        lua_pushnumber(L, 1.0);
    else
        lua_pushnil(L);
    if (v1.m_shaderTargets[4])
        lua_pushnumber(L, 1.0);
    else
        lua_pushnil(L);
    if (v1.m_shaderTargets[0])
        lua_pushnumber(L, 1.0);
    else
        lua_pushnil(L);
    if (v1.m_texFilterTrilinear)
        lua_pushnumber(L, 1.0);
    else
        lua_pushnil(L);
    lua_pushnumber(L, v1.unk18);
    if (v1.m_maxTexAnisotropy)
        lua_pushnumber(L, v1.m_maxTexAnisotropy);
    else
        lua_pushnil(L);
    if (v1.m_hwCursor)
        lua_pushnumber(L, 1.0);
    else
        lua_pushnil(L);
    return 7;
}

int32_t Script_GetGamma(lua_State* L) {
    auto v1 = CVar::Lookup("gamma");
    lua_pushnumber(L, 1.0f - v1->m_floatValue);
    return 1;
}

int32_t Script_SetGamma(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGamma(value)");
    }
    auto v1 = CVar::Lookup("gamma");
    float value = lua_tonumber(L, 1);
    char valueString[0x10];
    SStrPrintf(valueString, 0x10, "%f", 1.0f - value);
    v1->Set(valueString, 1, 0, 0, 1);
    return 0;
}

int32_t Script_GetTerrainMip(lua_State* L) {
    auto v1 = CVar::Lookup("shadowLevel");
    lua_pushnumber(L, 1.0f - v1->GetInt());
    return 1;
}

int32_t Script_SetTerrainMip(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

int32_t Script_IsStereoVideoAvailable(lua_State* L) {
    if (g_theGxDevicePtr->Caps().m_stereoAvailable)
        lua_pushnumber(L, 1.0);
    else
        lua_pushnil(L);
    return 1;
}

int32_t Script_IsPlayerResolutionAvailable(lua_State* L) {
    if (OsGetNumberOfProcessors() <= 1)
        lua_pushnil(L);
    else
        lua_pushnumber(L, 1.0);
    return 1;
}

FrameScript_Method CGVideoOptions::s_ScriptFunctions[] = {
    { "GetScreenResolutions", &Script_GetScreenResolutions },
    { "GetCurrentResolution", &Script_GetCurrentResolution },
    { "SetScreenResolution", &Script_SetScreenResolution },
    { "GetRefreshRates", &Script_GetRefreshRates },
    { "SetupFullscreenScale", &Script_SetupFullscreenScale },
    { "GetMultisampleFormats", &Script_GetMultisampleFormats },
    { "GetCurrentMultisampleFormat", &Script_GetCurrentMultisampleFormat },
    { "SetMultisampleFormat", &Script_SetMultisampleFormat },
    { "GetVideoCaps", &Script_GetVideoCaps },
    { "GetGamma", &Script_GetGamma },
    { "SetGamma", &Script_SetGamma },
    { "GetTerrainMip", &Script_GetTerrainMip },
    { "SetTerrainMip", &Script_SetTerrainMip },
    { "IsStereoVideoAvailable", &Script_IsStereoVideoAvailable },
    { "IsPlayerResolutionAvailable", &Script_IsPlayerResolutionAvailable },
};

size_t CGVideoOptions::s_NumScriptFunctions = sizeof(CGVideoOptions::s_ScriptFunctions) / sizeof(FrameScript_Method);

void CGVideoOptions::RegisterScriptFunctions() {
    for (int32_t i = 0; i < CGVideoOptions::s_NumScriptFunctions; i++) {
        auto item = &s_ScriptFunctions[i];
        FrameScript_RegisterFunction(item->name, item->method);
    }
}
