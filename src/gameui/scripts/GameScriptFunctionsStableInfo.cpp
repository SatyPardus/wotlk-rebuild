#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5A1950
static int32_t Script_ClosePetStables(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A19C0
static int32_t Script_StablePet(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1AC0
static int32_t Script_UnstablePet(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1BD0
static int32_t Script_BuyStableSlot(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A0F60
static int32_t Script_GetNumStablePets(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5A0FA0
static int32_t Script_GetNumStableSlots(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5A1330
static int32_t Script_GetStablePetInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A14D0
static int32_t Script_GetNextStableSlotCost(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1CA0
static int32_t Script_ClickStablePet(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A0FE0
static int32_t Script_PickupStablePet(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1060
static int32_t Script_GetSelectedStablePet(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1530
static int32_t Script_SetPetStablePaperdoll(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A16A0
static int32_t Script_GetStablePetFoodTypes(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5A1090
static int32_t Script_IsAtStableMaster(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void StableInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_STABLE_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_StableInfo[i].name,
            GameScript::s_ScriptFunctions_StableInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_StableInfo[NUM_SCRIPT_FUNCTIONS_STABLE_INFO] = {
    { "ClosePetStables", &Script_ClosePetStables },
    { "StablePet", &Script_StablePet },
    { "UnstablePet", &Script_UnstablePet },
    { "BuyStableSlot", &Script_BuyStableSlot },
    { "GetNumStablePets", &Script_GetNumStablePets },
    { "GetNumStableSlots", &Script_GetNumStableSlots },
    { "GetStablePetInfo", &Script_GetStablePetInfo },
    { "GetNextStableSlotCost", &Script_GetNextStableSlotCost },
    { "ClickStablePet", &Script_ClickStablePet },
    { "PickupStablePet", &Script_PickupStablePet },
    { "GetSelectedStablePet", &Script_GetSelectedStablePet },
    { "SetPetStablePaperdoll", &Script_SetPetStablePaperdoll },
    { "GetStablePetFoodTypes", &Script_GetStablePetFoodTypes },
    { "IsAtStableMaster", &Script_IsAtStableMaster },
};
