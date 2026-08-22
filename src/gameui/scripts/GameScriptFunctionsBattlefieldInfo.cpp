#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x54BAA0
static int32_t Script_GetNumBattlefields(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54D770
static int32_t Script_GetBattlefieldInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54D8F0
static int32_t Script_GetBattlefieldInstanceInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54BAE0
static int32_t Script_IsBattlefieldArena(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549AD0
static int32_t Script_IsActiveBattlefieldArena(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54D990
static int32_t Script_JoinBattlefield(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54BB40
static int32_t Script_SetSelectedBattlefield(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54BBD0
static int32_t Script_GetSelectedBattlefield(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54DA10
static int32_t Script_AcceptBattlefieldPort(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54BC30
static int32_t Script_GetBattlefieldStatus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549B80
static int32_t Script_GetBattlefieldPortExpiration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549C40
static int32_t Script_GetBattlefieldInstanceExpiration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549CD0
static int32_t Script_GetBattlefieldInstanceRunTime(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549D30
static int32_t Script_GetBattlefieldEstimatedWaitTime(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549DD0
static int32_t Script_GetBattlefieldTimeWaited(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549B40
static int32_t Script_CloseBattlefield(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54DCA0
static int32_t Script_RequestBattlefieldScoreData(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549E80
static int32_t Script_GetNumBattlefieldScores(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54BE90
static int32_t Script_GetBattlefieldScore(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549EC0
static int32_t Script_GetBattlefieldWinner(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C120
static int32_t Script_SetBattlefieldScoreFaction(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C250
static int32_t Script_LeaveBattlefield(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549F20
static int32_t Script_GetNumBattlefieldStats(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C170
static int32_t Script_GetBattlefieldStatInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x549F60
static int32_t Script_GetBattlefieldStatData(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54DCB0
static int32_t Script_RequestBattlefieldPositions(lua_State* L) {
    // TODO: sub_54CF60
    return 0;
}

// OFFSET: 0x54A040
static int32_t Script_GetNumBattlefieldPositions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C2E0
static int32_t Script_GetBattlefieldPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A0E0
static int32_t Script_GetNumBattlefieldFlagPositions(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54DCC0
static int32_t Script_GetBattlefieldFlagPosition(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A140
static int32_t Script_GetNumBattlefieldVehicles(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C4D0
static int32_t Script_GetBattlefieldVehicleInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C6E0
static int32_t Script_CanJoinBattlefieldAsGroup(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C740
static int32_t Script_GetBattlefieldMapIconScale(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A180
static int32_t Script_GetBattlefieldTeamInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A280
static int32_t Script_GetBattlefieldArenaFaction(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54DE00
static int32_t Script_SortBattlefieldScoreData(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C7A0
static int32_t Script_HearthAndResurrectFromArea(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C810
static int32_t Script_CanHearthAndResurrectFromArea(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C870
static int32_t Script_GetNumBattlegroundTypes(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54E010
static int32_t Script_GetBattlegroundInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54E6D0
static int32_t Script_RequestBattlegroundInstanceInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A2C0
static int32_t Script_GetNumArenaOpponents(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54E160
static int32_t Script_BattlefieldMgrEntryInviteResponse(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54E1A0
static int32_t Script_BattlefieldMgrQueueRequest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54E1C0
static int32_t Script_BattlefieldMgrQueueInviteResponse(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54E200
static int32_t Script_BattlefieldMgrExitRequest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C8A0
static int32_t Script_GetWorldPVPQueueStatus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A300
static int32_t Script_GetHolidayBGHonorCurrencyBonuses(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54A370
static int32_t Script_GetRandomBGHonorCurrencyBonuses(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x54C9F0
static int32_t Script_SortBGList(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void BattlefieldInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_BATTLEFIELD_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_BattlefieldInfo[i].name,
            GameScript::s_ScriptFunctions_BattlefieldInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_BattlefieldInfo[NUM_SCRIPT_FUNCTIONS_BATTLEFIELD_INFO] = {
    { "GetNumBattlefields", &Script_GetNumBattlefields },
    { "GetBattlefieldInfo", &Script_GetBattlefieldInfo },
    { "GetBattlefieldInstanceInfo", &Script_GetBattlefieldInstanceInfo },
    { "IsBattlefieldArena", &Script_IsBattlefieldArena },
    { "IsActiveBattlefieldArena", &Script_IsActiveBattlefieldArena },
    { "JoinBattlefield", &Script_JoinBattlefield },
    { "SetSelectedBattlefield", &Script_SetSelectedBattlefield },
    { "GetSelectedBattlefield", &Script_GetSelectedBattlefield },
    { "AcceptBattlefieldPort", &Script_AcceptBattlefieldPort },
    { "GetBattlefieldStatus", &Script_GetBattlefieldStatus },
    { "GetBattlefieldPortExpiration", &Script_GetBattlefieldPortExpiration },
    { "GetBattlefieldInstanceExpiration", &Script_GetBattlefieldInstanceExpiration },
    { "GetBattlefieldInstanceRunTime", &Script_GetBattlefieldInstanceRunTime },
    { "GetBattlefieldEstimatedWaitTime", &Script_GetBattlefieldEstimatedWaitTime },
    { "GetBattlefieldTimeWaited", &Script_GetBattlefieldTimeWaited },
    { "CloseBattlefield", &Script_CloseBattlefield },
    { "RequestBattlefieldScoreData", &Script_RequestBattlefieldScoreData },
    { "GetNumBattlefieldScores", &Script_GetNumBattlefieldScores },
    { "GetBattlefieldScore", &Script_GetBattlefieldScore },
    { "GetBattlefieldWinner", &Script_GetBattlefieldWinner },
    { "SetBattlefieldScoreFaction", &Script_SetBattlefieldScoreFaction },
    { "LeaveBattlefield", &Script_LeaveBattlefield },
    { "GetNumBattlefieldStats", &Script_GetNumBattlefieldStats },
    { "GetBattlefieldStatInfo", &Script_GetBattlefieldStatInfo },
    { "GetBattlefieldStatData", &Script_GetBattlefieldStatData },
    { "RequestBattlefieldPositions", &Script_RequestBattlefieldPositions },
    { "GetNumBattlefieldPositions", &Script_GetNumBattlefieldPositions },
    { "GetBattlefieldPosition", &Script_GetBattlefieldPosition },
    { "GetNumBattlefieldFlagPositions", &Script_GetNumBattlefieldFlagPositions },
    { "GetBattlefieldFlagPosition", &Script_GetBattlefieldFlagPosition },
    { "GetNumBattlefieldVehicles", &Script_GetNumBattlefieldVehicles },
    { "GetBattlefieldVehicleInfo", &Script_GetBattlefieldVehicleInfo },
    { "CanJoinBattlefieldAsGroup", &Script_CanJoinBattlefieldAsGroup },
    { "GetBattlefieldMapIconScale", &Script_GetBattlefieldMapIconScale },
    { "GetBattlefieldTeamInfo", &Script_GetBattlefieldTeamInfo },
    { "GetBattlefieldArenaFaction", &Script_GetBattlefieldArenaFaction },
    { "SortBattlefieldScoreData", &Script_SortBattlefieldScoreData },
    { "HearthAndResurrectFromArea", &Script_HearthAndResurrectFromArea },
    { "CanHearthAndResurrectFromArea", &Script_CanHearthAndResurrectFromArea },
    { "GetNumBattlegroundTypes", &Script_GetNumBattlegroundTypes },
    { "GetBattlegroundInfo", &Script_GetBattlegroundInfo },
    { "RequestBattlegroundInstanceInfo", &Script_RequestBattlegroundInstanceInfo },
    { "GetNumArenaOpponents", &Script_GetNumArenaOpponents },
    { "BattlefieldMgrEntryInviteResponse", &Script_BattlefieldMgrEntryInviteResponse },
    { "BattlefieldMgrQueueRequest", &Script_BattlefieldMgrQueueRequest },
    { "BattlefieldMgrQueueInviteResponse", &Script_BattlefieldMgrQueueInviteResponse },
    { "BattlefieldMgrExitRequest", &Script_BattlefieldMgrExitRequest },
    { "GetWorldPVPQueueStatus", &Script_GetWorldPVPQueueStatus },
    { "GetHolidayBGHonorCurrencyBonuses", &Script_GetHolidayBGHonorCurrencyBonuses },
    { "GetRandomBGHonorCurrencyBonuses", &Script_GetRandomBGHonorCurrencyBonuses },
    { "SortBGList", &Script_SortBGList },
};
