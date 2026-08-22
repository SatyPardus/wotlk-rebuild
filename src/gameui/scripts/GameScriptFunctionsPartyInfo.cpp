#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x52C110
static int32_t Script_GetNumPartyMembers(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x52C190
static int32_t Script_GetRealNumPartyMembers(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x52C1D0
static int32_t Script_GetPartyMember(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52C270
static int32_t Script_GetPartyLeaderIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52CCD0
static int32_t Script_IsPartyLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52CD30
static int32_t Script_IsRealPartyLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52D990
static int32_t Script_LeaveParty(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52CD90
static int32_t Script_GetLootMethod(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52DC20
static int32_t Script_SetLootMethod(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52C2A0
static int32_t Script_GetLootThreshold(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52DE60
static int32_t Script_SetLootThreshold(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52E1B0
static int32_t Script_SetPartyAssignment(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52E400
static int32_t Script_ClearPartyAssignment(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52CF60
static int32_t Script_GetPartyAssignment(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52D9C0
static int32_t Script_SilenceMember(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52DAF0
static int32_t Script_UnSilenceMember(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52CF00
static int32_t Script_SetOptOutOfLoot(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52C2D0
static int32_t Script_GetOptOutOfLoot(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52D000
static int32_t Script_CanChangePlayerDifficulty(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52E420
static int32_t Script_ChangePlayerDifficulty(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52C310
static int32_t Script_IsPartyLFG(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x52C350
static int32_t Script_HasLFGRestrictions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void PartyInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_PARTY_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_PartyInfo[i].name,
            GameScript::s_ScriptFunctions_PartyInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_PartyInfo[NUM_SCRIPT_FUNCTIONS_PARTY_INFO] = {
    { "GetNumPartyMembers", &Script_GetNumPartyMembers },
    { "GetRealNumPartyMembers", &Script_GetRealNumPartyMembers },
    { "GetPartyMember", &Script_GetPartyMember },
    { "GetPartyLeaderIndex", &Script_GetPartyLeaderIndex },
    { "IsPartyLeader", &Script_IsPartyLeader },
    { "IsRealPartyLeader", &Script_IsRealPartyLeader },
    { "LeaveParty", &Script_LeaveParty },
    { "GetLootMethod", &Script_GetLootMethod },
    { "SetLootMethod", &Script_SetLootMethod },
    { "GetLootThreshold", &Script_GetLootThreshold },
    { "SetLootThreshold", &Script_SetLootThreshold },
    { "SetPartyAssignment", &Script_SetPartyAssignment },
    { "ClearPartyAssignment", &Script_ClearPartyAssignment },
    { "GetPartyAssignment", &Script_GetPartyAssignment },
    { "SilenceMember", &Script_SilenceMember },
    { "UnSilenceMember", &Script_UnSilenceMember },
    { "SetOptOutOfLoot", &Script_SetOptOutOfLoot },
    { "GetOptOutOfLoot", &Script_GetOptOutOfLoot },
    { "CanChangePlayerDifficulty", &Script_CanChangePlayerDifficulty },
    { "ChangePlayerDifficulty", &Script_ChangePlayerDifficulty },
    { "IsPartyLFG", &Script_IsPartyLFG },
    { "HasLFGRestrictions", &Script_HasLFGRestrictions },
};
