#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5A24F0
static int32_t Script_ClosePetitionVendor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1F70
static int32_t Script_GetNumPetitionItems(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5A1FB0
static int32_t Script_GetPetitionItemInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A2530
static int32_t Script_BuyPetition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A2200
static int32_t Script_ClickPetitionButton(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A2600
static int32_t Script_TurnInPetition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A26D0
static int32_t Script_TurnInArenaPetition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A2820
static int32_t Script_HasFilledPetition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void PetitionVendorRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_PETITION_VENDOR; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_PetitionVendor[i].name,
            GameScript::s_ScriptFunctions_PetitionVendor[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_PetitionVendor[NUM_SCRIPT_FUNCTIONS_PETITION_VENDOR] = {
    { "ClosePetitionVendor", &Script_ClosePetitionVendor },
    { "GetNumPetitionItems", &Script_GetNumPetitionItems },
    { "GetPetitionItemInfo", &Script_GetPetitionItemInfo },
    { "BuyPetition", &Script_BuyPetition },
    { "ClickPetitionButton", &Script_ClickPetitionButton },
    { "TurnInPetition", &Script_TurnInPetition },
    { "TurnInArenaPetition", &Script_TurnInArenaPetition },
    { "HasFilledPetition", &Script_HasFilledPetition },
};
