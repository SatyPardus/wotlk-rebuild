#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x57B3A0
static int32_t Script_GetAutoCompleteResults(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x57B500
static int32_t Script_GetAutoCompletePresenceID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void AutoCompleteRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_AUTO_COMPLETE; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_AutoComplete[i].name,
            GameScript::s_ScriptFunctions_AutoComplete[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_AutoComplete[NUM_SCRIPT_FUNCTIONS_AUTO_COMPLETE] = {
    { "GetAutoCompleteResults", &Script_GetAutoCompleteResults },
    { "GetAutoCompletePresenceID", &Script_GetAutoCompletePresenceID },
};
