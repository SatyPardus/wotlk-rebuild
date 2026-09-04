#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x548720
static int32_t Script_GetNumWorldStateUI(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x548D40
static int32_t Script_GetWorldStateUIInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void WorldStateInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_WORLD_STATE_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_WorldStateInfo[i].name,
            GameScript::s_ScriptFunctions_WorldStateInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_WorldStateInfo[NUM_SCRIPT_FUNCTIONS_WORLD_STATE_INFO] = {
    { "GetNumWorldStateUI", &Script_GetNumWorldStateUI },
    { "GetWorldStateUIInfo", &Script_GetWorldStateUIInfo },
};
