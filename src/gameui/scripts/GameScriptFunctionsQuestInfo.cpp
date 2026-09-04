#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x58D350
static int32_t Script_CloseQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BD10
static int32_t Script_GetTitleText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BD30
static int32_t Script_GetGreetingText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BD50
static int32_t Script_GetQuestText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BD70
static int32_t Script_GetObjectiveText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BD90
static int32_t Script_GetProgressText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BDB0
static int32_t Script_GetRewardText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BDD0
static int32_t Script_GetNumAvailableQuests(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x58BE00
static int32_t Script_GetNumActiveQuests(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x58BE30
static int32_t Script_GetAvailableTitle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BED0
static int32_t Script_GetActiveTitle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58BF70
static int32_t Script_GetAvailableLevel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C010
static int32_t Script_GetActiveLevel(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D370
static int32_t Script_IsAvailableQuestTrivial(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D420
static int32_t Script_IsActiveQuestTrivial(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D4D0
static int32_t Script_SelectAvailableQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D540
static int32_t Script_SelectActiveQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D5B0
static int32_t Script_AcceptQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D5C0
static int32_t Script_DeclineQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D5D0
static int32_t Script_IsQuestCompletable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D610
static int32_t Script_CompleteQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D620
static int32_t Script_GetQuestReward(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C0B0
static int32_t Script_GetRewardMoney(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C0E0
static int32_t Script_GetRewardXP(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C110
static int32_t Script_GetRewardHonor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D670
static int32_t Script_GetRewardSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C160
static int32_t Script_GetQuestMoneyToGet(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C190
static int32_t Script_GetNumQuestRewards(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x58C1F0
static int32_t Script_GetNumQuestChoices(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x58C250
static int32_t Script_GetNumQuestItems(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x58D980
static int32_t Script_GetQuestItemInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C2B0
static int32_t Script_GetQuestItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58DAB0
static int32_t Script_GetQuestSpellLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C3A0
static int32_t Script_QuestChooseRewardError(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C910
static int32_t Script_ConfirmAcceptQuest(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C990
static int32_t Script_GetQuestBackgroundMaterial(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C3B0
static int32_t Script_GetSuggestedGroupNum(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C3E0
static int32_t Script_QuestFlagsPVP(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C430
static int32_t Script_QuestGetAutoAccept(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58DB30
static int32_t Script_GetDailyQuestsCompleted(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C470
static int32_t Script_GetMaxDailyQuests(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58D810
static int32_t Script_GetRewardTitle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C140
static int32_t Script_GetRewardTalents(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C4A0
static int32_t Script_GetRewardArenaPoints(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58DBC0
static int32_t Script_GetAvailableQuestInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C4D0
static int32_t Script_QuestIsDaily(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58C510
static int32_t Script_QuestIsWeekly(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void QuestInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_QUEST_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_QuestInfo[i].name,
            GameScript::s_ScriptFunctions_QuestInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_QuestInfo[NUM_SCRIPT_FUNCTIONS_QUEST_INFO] = {
    { "CloseQuest", &Script_CloseQuest },
    { "GetTitleText", &Script_GetTitleText },
    { "GetGreetingText", &Script_GetGreetingText },
    { "GetQuestText", &Script_GetQuestText },
    { "GetObjectiveText", &Script_GetObjectiveText },
    { "GetProgressText", &Script_GetProgressText },
    { "GetRewardText", &Script_GetRewardText },
    { "GetNumAvailableQuests", &Script_GetNumAvailableQuests },
    { "GetNumActiveQuests", &Script_GetNumActiveQuests },
    { "GetAvailableTitle", &Script_GetAvailableTitle },
    { "GetActiveTitle", &Script_GetActiveTitle },
    { "GetAvailableLevel", &Script_GetAvailableLevel },
    { "GetActiveLevel", &Script_GetActiveLevel },
    { "IsAvailableQuestTrivial", &Script_IsAvailableQuestTrivial },
    { "IsActiveQuestTrivial", &Script_IsActiveQuestTrivial },
    { "SelectAvailableQuest", &Script_SelectAvailableQuest },
    { "SelectActiveQuest", &Script_SelectActiveQuest },
    { "AcceptQuest", &Script_AcceptQuest },
    { "DeclineQuest", &Script_DeclineQuest },
    { "IsQuestCompletable", &Script_IsQuestCompletable },
    { "CompleteQuest", &Script_CompleteQuest },
    { "GetQuestReward", &Script_GetQuestReward },
    { "GetRewardMoney", &Script_GetRewardMoney },
    { "GetRewardXP", &Script_GetRewardXP },
    { "GetRewardHonor", &Script_GetRewardHonor },
    { "GetRewardSpell", &Script_GetRewardSpell },
    { "GetQuestMoneyToGet", &Script_GetQuestMoneyToGet },
    { "GetNumQuestRewards", &Script_GetNumQuestRewards },
    { "GetNumQuestChoices", &Script_GetNumQuestChoices },
    { "GetNumQuestItems", &Script_GetNumQuestItems },
    { "GetQuestItemInfo", &Script_GetQuestItemInfo },
    { "GetQuestItemLink", &Script_GetQuestItemLink },
    { "GetQuestSpellLink", &Script_GetQuestSpellLink },
    { "QuestChooseRewardError", &Script_QuestChooseRewardError },
    { "ConfirmAcceptQuest", &Script_ConfirmAcceptQuest },
    { "GetQuestBackgroundMaterial", &Script_GetQuestBackgroundMaterial },
    { "GetSuggestedGroupNum", &Script_GetSuggestedGroupNum },
    { "QuestFlagsPVP", &Script_QuestFlagsPVP },
    { "QuestGetAutoAccept", &Script_QuestGetAutoAccept },
    { "GetDailyQuestsCompleted", &Script_GetDailyQuestsCompleted },
    { "GetMaxDailyQuests", &Script_GetMaxDailyQuests },
    { "GetRewardTitle", &Script_GetRewardTitle },
    { "GetRewardTalents", &Script_GetRewardTalents },
    { "GetRewardArenaPoints", &Script_GetRewardArenaPoints },
    { "GetAvailableQuestInfo", &Script_GetAvailableQuestInfo },
    { "QuestIsDaily", &Script_QuestIsDaily },
    { "QuestIsWeekly", &Script_QuestIsWeekly },
};
