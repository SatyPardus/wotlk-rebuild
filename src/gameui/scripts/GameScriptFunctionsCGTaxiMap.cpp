#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x590D10
static int32_t Script_SetTaxiMap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590710
static int32_t Script_NumTaxiNodes(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590D60
static int32_t Script_TaxiNodeName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590E00
static int32_t Script_TaxiNodePosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5915E0
static int32_t Script_TaxiNodeCost(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x591680
static int32_t Script_TakeTaxiNode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590EC0
static int32_t Script_CloseTaxiMap(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590ED0
static int32_t Script_TaxiNodeGetType(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x591E60
static int32_t Script_TaxiNodeSetCurrent(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590F40
static int32_t Script_TaxiGetSrcX(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x590FE0
static int32_t Script_TaxiGetSrcY(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x591080
static int32_t Script_TaxiGetDestX(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x591120
static int32_t Script_TaxiGetDestY(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5911C0
static int32_t Script_GetNumRoutes(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

void CGTaxiMapRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_CGTAXI_MAP; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_CGTaxiMap[i].name,
            GameScript::s_ScriptFunctions_CGTaxiMap[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_CGTaxiMap[NUM_SCRIPT_FUNCTIONS_CGTAXI_MAP] = {
    { "SetTaxiMap", &Script_SetTaxiMap },
    { "NumTaxiNodes", &Script_NumTaxiNodes },
    { "TaxiNodeName", &Script_TaxiNodeName },
    { "TaxiNodePosition", &Script_TaxiNodePosition },
    { "TaxiNodeCost", &Script_TaxiNodeCost },
    { "TakeTaxiNode", &Script_TakeTaxiNode },
    { "CloseTaxiMap", &Script_CloseTaxiMap },
    { "TaxiNodeGetType", &Script_TaxiNodeGetType },
    { "TaxiNodeSetCurrent", &Script_TaxiNodeSetCurrent },
    { "TaxiGetSrcX", &Script_TaxiGetSrcX },
    { "TaxiGetSrcY", &Script_TaxiGetSrcY },
    { "TaxiGetDestX", &Script_TaxiGetDestX },
    { "TaxiGetDestY", &Script_TaxiGetDestY },
    { "GetNumRoutes", &Script_GetNumRoutes },
};
