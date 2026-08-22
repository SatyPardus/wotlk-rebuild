#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x58A900
static int32_t Script_GetGossipText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A920
static int32_t Script_GetNumGossipOptions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A960
static int32_t Script_GetNumGossipAvailableQuests(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A9A0
static int32_t Script_GetNumGossipActiveQuests(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A9E0
static int32_t Script_GetGossipOptions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58B3A0
static int32_t Script_GetGossipAvailableQuests(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58B490
static int32_t Script_GetGossipActiveQuests(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58B590
static int32_t Script_SelectGossipOption(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58B600
static int32_t Script_SelectGossipAvailableQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58B670
static int32_t Script_SelectGossipActiveQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58AA40
static int32_t Script_CloseGossip(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58AB50
static int32_t Script_ForceGossip(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void GossipInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_GOSSIP_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_GossipInfo[i].name,
            GameScript::s_ScriptFunctions_GossipInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_GossipInfo[NUM_SCRIPT_FUNCTIONS_GOSSIP_INFO] = {
    { "GetGossipText", &Script_GetGossipText },
    { "GetNumGossipOptions", &Script_GetNumGossipOptions },
    { "GetNumGossipAvailableQuests", &Script_GetNumGossipAvailableQuests },
    { "GetNumGossipActiveQuests", &Script_GetNumGossipActiveQuests },
    { "GetGossipOptions", &Script_GetGossipOptions },
    { "GetGossipAvailableQuests", &Script_GetGossipAvailableQuests },
    { "GetGossipActiveQuests", &Script_GetGossipActiveQuests },
    { "SelectGossipOption", &Script_SelectGossipOption },
    { "SelectGossipAvailableQuest", &Script_SelectGossipAvailableQuest },
    { "SelectGossipActiveQuest", &Script_SelectGossipActiveQuest },
    { "CloseGossip", &Script_CloseGossip },
    { "ForceGossip", &Script_ForceGossip },
};
