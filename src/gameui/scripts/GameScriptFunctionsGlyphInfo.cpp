#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5B71E0
static int32_t Script_GetNumGlyphSockets(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5B7260
static int32_t Script_GetGlyphSocketInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B7410
static int32_t Script_GlyphMatchesSocket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B75A0
static int32_t Script_PlaceGlyphInSocket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B7660
static int32_t Script_RemoveGlyphFromSocket(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B7740
static int32_t Script_GetGlyphLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void GlyphInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_GLYPH_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_GlyphInfo[i].name,
            GameScript::s_ScriptFunctions_GlyphInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_GlyphInfo[NUM_SCRIPT_FUNCTIONS_GLYPH_INFO] = {
    { "GetNumGlyphSockets", &Script_GetNumGlyphSockets },
    { "GetGlyphSocketInfo", &Script_GetGlyphSocketInfo },
    { "GlyphMatchesSocket", &Script_GlyphMatchesSocket },
    { "PlaceGlyphInSocket", &Script_PlaceGlyphInSocket },
    { "RemoveGlyphFromSocket", &Script_RemoveGlyphFromSocket },
    { "GetGlyphLink", &Script_GetGlyphLink },
};
