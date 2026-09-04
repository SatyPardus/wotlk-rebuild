#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x595040
static int32_t Script_OpenTrainer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x594370
static int32_t Script_CloseTrainer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x593D10
static int32_t Script_GetNumTrainerServices(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x595090
static int32_t Script_GetTrainerServiceInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5943A0
static int32_t Script_SelectTrainerService(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x593D50
static int32_t Script_IsTradeskillTrainer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x594430
static int32_t Script_GetTrainerSelectionIndex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x593D90
static int32_t Script_GetTrainerGreetingText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x595150
static int32_t Script_GetTrainerServiceIcon(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5952F0
static int32_t Script_GetTrainerServiceSkillLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x594480
static int32_t Script_GetTrainerServiceCost(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x594530
static int32_t Script_GetTrainerServiceLevelReq(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x595470
static int32_t Script_GetTrainerServiceSkillReq(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5945B0
static int32_t Script_GetTrainerServiceNumAbilityReq(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5955E0
static int32_t Script_GetTrainerServiceAbilityReq(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5957D0
static int32_t Script_GetTrainerServiceStepReq(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5959D0
static int32_t Script_GetTrainerServiceDescription(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x595B60
static int32_t Script_IsTrainerServiceSkillStep(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x595C40
static int32_t Script_GetTrainerServiceStepIncrease(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x595E60
static int32_t Script_BuyTrainerService(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x595EC0
static int32_t Script_SetTrainerServiceTypeFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x596010
static int32_t Script_SetTrainerSkillLineFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x593E10
static int32_t Script_GetTrainerServiceTypeFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x593EB0
static int32_t Script_GetTrainerSkillLineFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x594650
static int32_t Script_GetTrainerSkillLines(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x596150
static int32_t Script_CollapseTrainerSkillLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5961F0
static int32_t Script_ExpandTrainerSkillLine(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x596290
static int32_t Script_GetTrainerServiceItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void ClassTrainerRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_CLASS_TRAINER; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_ClassTrainer[i].name,
            GameScript::s_ScriptFunctions_ClassTrainer[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_ClassTrainer[NUM_SCRIPT_FUNCTIONS_CLASS_TRAINER] = {
    { "OpenTrainer", &Script_OpenTrainer },
    { "CloseTrainer", &Script_CloseTrainer },
    { "GetNumTrainerServices", &Script_GetNumTrainerServices },
    { "GetTrainerServiceInfo", &Script_GetTrainerServiceInfo },
    { "SelectTrainerService", &Script_SelectTrainerService },
    { "IsTradeskillTrainer", &Script_IsTradeskillTrainer },
    { "GetTrainerSelectionIndex", &Script_GetTrainerSelectionIndex },
    { "GetTrainerGreetingText", &Script_GetTrainerGreetingText },
    { "GetTrainerServiceIcon", &Script_GetTrainerServiceIcon },
    { "GetTrainerServiceSkillLine", &Script_GetTrainerServiceSkillLine },
    { "GetTrainerServiceCost", &Script_GetTrainerServiceCost },
    { "GetTrainerServiceLevelReq", &Script_GetTrainerServiceLevelReq },
    { "GetTrainerServiceSkillReq", &Script_GetTrainerServiceSkillReq },
    { "GetTrainerServiceNumAbilityReq", &Script_GetTrainerServiceNumAbilityReq },
    { "GetTrainerServiceAbilityReq", &Script_GetTrainerServiceAbilityReq },
    { "GetTrainerServiceStepReq", &Script_GetTrainerServiceStepReq },
    { "GetTrainerServiceDescription", &Script_GetTrainerServiceDescription },
    { "IsTrainerServiceSkillStep", &Script_IsTrainerServiceSkillStep },
    { "GetTrainerServiceStepIncrease", &Script_GetTrainerServiceStepIncrease },
    { "BuyTrainerService", &Script_BuyTrainerService },
    { "SetTrainerServiceTypeFilter", &Script_SetTrainerServiceTypeFilter },
    { "SetTrainerSkillLineFilter", &Script_SetTrainerSkillLineFilter },
    { "GetTrainerServiceTypeFilter", &Script_GetTrainerServiceTypeFilter },
    { "GetTrainerSkillLineFilter", &Script_GetTrainerSkillLineFilter },
    { "GetTrainerSkillLines", &Script_GetTrainerSkillLines },
    { "CollapseTrainerSkillLine", &Script_CollapseTrainerSkillLine },
    { "ExpandTrainerSkillLine", &Script_ExpandTrainerSkillLine },
    { "GetTrainerServiceItemLink", &Script_GetTrainerServiceItemLink },
};
