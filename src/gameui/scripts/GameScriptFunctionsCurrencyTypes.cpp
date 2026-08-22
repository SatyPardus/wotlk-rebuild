#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5AFD10
static int32_t Script_GetCurrencyListSize(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B0680
static int32_t Script_GetCurrencyListInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B0940
static int32_t Script_ExpandCurrencyList(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B09B0
static int32_t Script_SetCurrencyUnused(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B0A20
static int32_t Script_SetCurrencyBackpack(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5B0A90
static int32_t Script_GetBackpackCurrencyInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void CurrencyTypesRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_CURRENCY_TYPES; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_CurrencyTypes[i].name,
            GameScript::s_ScriptFunctions_CurrencyTypes[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_CurrencyTypes[NUM_SCRIPT_FUNCTIONS_CURRENCY_TYPES] = {
    { "GetCurrencyListSize", &Script_GetCurrencyListSize },
    { "GetCurrencyListInfo", &Script_GetCurrencyListInfo },
    { "ExpandCurrencyList", &Script_ExpandCurrencyList },
    { "SetCurrencyUnused", &Script_SetCurrencyUnused },
    { "SetCurrencyBackpack", &Script_SetCurrencyBackpack },
    { "GetBackpackCurrencyInfo", &Script_GetBackpackCurrencyInfo },
};
