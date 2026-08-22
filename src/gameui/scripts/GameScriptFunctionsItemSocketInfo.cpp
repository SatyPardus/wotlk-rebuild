#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5C49C0
static int32_t Script_CloseSocketInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C49D0
static int32_t Script_GetSocketItemInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C4AF0
static int32_t Script_GetNumSockets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C5160
static int32_t Script_GetExistingSocketInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C5340
static int32_t Script_GetExistingSocketLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C4B60
static int32_t Script_GetNewSocketInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C4D10
static int32_t Script_GetNewSocketLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C4DC0
static int32_t Script_ClickSocketButton(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C5460
static int32_t Script_AcceptSockets(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C4550
static int32_t Script_GetSocketTypes(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C5470
static int32_t Script_GetSocketItemRefundable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5C4EA0
static int32_t Script_GetSocketItemBoundTradeable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void ItemSocketInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_ITEM_SOCKET_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_ItemSocketInfo[i].name,
            GameScript::s_ScriptFunctions_ItemSocketInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_ItemSocketInfo[NUM_SCRIPT_FUNCTIONS_ITEM_SOCKET_INFO] = {
    { "CloseSocketInfo", &Script_CloseSocketInfo },
    { "GetSocketItemInfo", &Script_GetSocketItemInfo },
    { "GetNumSockets", &Script_GetNumSockets },
    { "GetExistingSocketInfo", &Script_GetExistingSocketInfo },
    { "GetExistingSocketLink", &Script_GetExistingSocketLink },
    { "GetNewSocketInfo", &Script_GetNewSocketInfo },
    { "GetNewSocketLink", &Script_GetNewSocketLink },
    { "ClickSocketButton", &Script_ClickSocketButton },
    { "AcceptSockets", &Script_AcceptSockets },
    { "GetSocketTypes", &Script_GetSocketTypes },
    { "GetSocketItemRefundable", &Script_GetSocketItemRefundable },
    { "GetSocketItemBoundTradeable", &Script_GetSocketItemBoundTradeable },
};
