#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x572B40
static int32_t Script_GetNumRaidMembers(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x572B80
static int32_t Script_GetRealNumRaidMembers(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x573690
static int32_t Script_GetRaidRosterInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x572BC0
static int32_t Script_SetRaidRosterSelection(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x572C50
static int32_t Script_GetRaidRosterSelection(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573A60
static int32_t Script_IsRaidLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573AB0
static int32_t Script_IsRealRaidLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573B00
static int32_t Script_IsRaidOfficer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573B50
static int32_t Script_SetRaidSubgroup(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573C90
static int32_t Script_SwapRaidSubgroup(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x574A00
static int32_t Script_ConvertToRaid(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573E10
static int32_t Script_PromoteToLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573EF0
static int32_t Script_PromoteToAssistant(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x573FD0
static int32_t Script_DemoteAssistant(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x574AB0
static int32_t Script_SetRaidTarget(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x572AB0
static int32_t Script_GetRaidTargetIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5740B0
static int32_t Script_DoReadyCheck(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5740C0
static int32_t Script_ConfirmReadyCheck(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x572C80
static int32_t Script_GetReadyCheckTimeLeft(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x574180
static int32_t Script_GetReadyCheckStatus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void RaidInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_RAID_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_RaidInfo[i].name,
            GameScript::s_ScriptFunctions_RaidInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_RaidInfo[NUM_SCRIPT_FUNCTIONS_RAID_INFO] = {
    { "GetNumRaidMembers", &Script_GetNumRaidMembers },
    { "GetRealNumRaidMembers", &Script_GetRealNumRaidMembers },
    { "GetRaidRosterInfo", &Script_GetRaidRosterInfo },
    { "SetRaidRosterSelection", &Script_SetRaidRosterSelection },
    { "GetRaidRosterSelection", &Script_GetRaidRosterSelection },
    { "IsRaidLeader", &Script_IsRaidLeader },
    { "IsRealRaidLeader", &Script_IsRealRaidLeader },
    { "IsRaidOfficer", &Script_IsRaidOfficer },
    { "SetRaidSubgroup", &Script_SetRaidSubgroup },
    { "SwapRaidSubgroup", &Script_SwapRaidSubgroup },
    { "ConvertToRaid", &Script_ConvertToRaid },
    { "PromoteToLeader", &Script_PromoteToLeader },
    { "PromoteToAssistant", &Script_PromoteToAssistant },
    { "DemoteAssistant", &Script_DemoteAssistant },
    { "SetRaidTarget", &Script_SetRaidTarget },
    { "GetRaidTargetIndex", &Script_GetRaidTargetIndex },
    { "DoReadyCheck", &Script_DoReadyCheck },
    { "ConfirmReadyCheck", &Script_ConfirmReadyCheck },
    { "GetReadyCheckTimeLeft", &Script_GetReadyCheckTimeLeft },
    { "GetReadyCheckStatus", &Script_GetReadyCheckStatus },
};
