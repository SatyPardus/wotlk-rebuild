#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5DA8B0
static int32_t Script_CloseTradeSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA120
static int32_t Script_GetNumTradeSkills(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5DB2A0
static int32_t Script_GetTradeSkillInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA900
static int32_t Script_SelectTradeSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA970
static int32_t Script_GetTradeSkillSelectionIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA9A0
static int32_t Script_GetTradeSkillCooldown(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5DB550
static int32_t Script_GetTradeSkillIcon(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DB6E0
static int32_t Script_GetTradeSkillNumMade(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DB810
static int32_t Script_GetTradeSkillLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DBA70
static int32_t Script_GetTradeSkillItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD070
static int32_t Script_SetTradeSkillItemNameFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA150
static int32_t Script_GetTradeSkillItemNameFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD090
static int32_t Script_SetTradeSkillItemLevelFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA170
static int32_t Script_GetTradeSkillItemLevelFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DBBA0
static int32_t Script_GetTradeSkillNumReagents(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DE400
static int32_t Script_GetTradeSkillReagentInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DBCC0
static int32_t Script_GetTradeSkillReagentItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DE620
static int32_t Script_GetTradeSkillTools(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DBE10
static int32_t Script_GetTradeSkillDescription(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DAA70
static int32_t Script_GetTradeSkillSubClasses(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DBF10
static int32_t Script_GetTradeSkillInvSlots(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD0F0
static int32_t Script_SetTradeSkillSubClassFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA1B0
static int32_t Script_GetTradeSkillSubClassFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD230
static int32_t Script_SetTradeSkillInvSlotFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DBF90
static int32_t Script_GetTradeSkillInvSlotFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD3E0
static int32_t Script_TradeSkillOnlyShowMakeable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD420
static int32_t Script_TradeSkillOnlyShowSkillUps(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD460
static int32_t Script_CollapseTradeSkillSubClass(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DD500
static int32_t Script_ExpandTradeSkillSubClass(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DAB70
static int32_t Script_GetFirstTradeSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA270
static int32_t Script_GetTradeskillRepeatCount(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DAC00
static int32_t Script_DoTradeSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DB920
static int32_t Script_GetTradeSkillRecipeLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA2E0
static int32_t Script_StopTradeSkillRepeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DC0B0
static int32_t Script_GetTradeSkillListLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5DA310
static int32_t Script_IsTradeSkillLinked(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void TradeSkillRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_TRADE_SKILL; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_TradeSkill[i].name,
            GameScript::s_ScriptFunctions_TradeSkill[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_TradeSkill[NUM_SCRIPT_FUNCTIONS_TRADE_SKILL] = {
    { "CloseTradeSkill", &Script_CloseTradeSkill },
    { "GetNumTradeSkills", &Script_GetNumTradeSkills },
    { "GetTradeSkillInfo", &Script_GetTradeSkillInfo },
    { "SelectTradeSkill", &Script_SelectTradeSkill },
    { "GetTradeSkillSelectionIndex", &Script_GetTradeSkillSelectionIndex },
    { "GetTradeSkillCooldown", &Script_GetTradeSkillCooldown },
    { "GetTradeSkillIcon", &Script_GetTradeSkillIcon },
    { "GetTradeSkillNumMade", &Script_GetTradeSkillNumMade },
    { "GetTradeSkillLine", &Script_GetTradeSkillLine },
    { "GetTradeSkillItemLink", &Script_GetTradeSkillItemLink },
    { "SetTradeSkillItemNameFilter", &Script_SetTradeSkillItemNameFilter },
    { "GetTradeSkillItemNameFilter", &Script_GetTradeSkillItemNameFilter },
    { "SetTradeSkillItemLevelFilter", &Script_SetTradeSkillItemLevelFilter },
    { "GetTradeSkillItemLevelFilter", &Script_GetTradeSkillItemLevelFilter },
    { "GetTradeSkillNumReagents", &Script_GetTradeSkillNumReagents },
    { "GetTradeSkillReagentInfo", &Script_GetTradeSkillReagentInfo },
    { "GetTradeSkillReagentItemLink", &Script_GetTradeSkillReagentItemLink },
    { "GetTradeSkillTools", &Script_GetTradeSkillTools },
    { "GetTradeSkillDescription", &Script_GetTradeSkillDescription },
    { "GetTradeSkillSubClasses", &Script_GetTradeSkillSubClasses },
    { "GetTradeSkillInvSlots", &Script_GetTradeSkillInvSlots },
    { "SetTradeSkillSubClassFilter", &Script_SetTradeSkillSubClassFilter },
    { "GetTradeSkillSubClassFilter", &Script_GetTradeSkillSubClassFilter },
    { "SetTradeSkillInvSlotFilter", &Script_SetTradeSkillInvSlotFilter },
    { "GetTradeSkillInvSlotFilter", &Script_GetTradeSkillInvSlotFilter },
    { "TradeSkillOnlyShowMakeable", &Script_TradeSkillOnlyShowMakeable },
    { "TradeSkillOnlyShowSkillUps", &Script_TradeSkillOnlyShowSkillUps },
    { "CollapseTradeSkillSubClass", &Script_CollapseTradeSkillSubClass },
    { "ExpandTradeSkillSubClass", &Script_ExpandTradeSkillSubClass },
    { "GetFirstTradeSkill", &Script_GetFirstTradeSkill },
    { "GetTradeskillRepeatCount", &Script_GetTradeskillRepeatCount },
    { "DoTradeSkill", &Script_DoTradeSkill },
    { "GetTradeSkillRecipeLink", &Script_GetTradeSkillRecipeLink },
    { "StopTradeSkillRepeat", &Script_StopTradeSkillRepeat },
    { "GetTradeSkillListLink", &Script_GetTradeSkillListLink },
    { "IsTradeSkillLinked", &Script_IsTradeSkillLinked },
};
