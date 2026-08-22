#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5CD1B0
static int32_t Script_GetNumSkillLines(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CDE20
static int32_t Script_GetSkillLineInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE2C0
static int32_t Script_AbandonSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE3A0
static int32_t Script_CollapseSkillHeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE400
static int32_t Script_ExpandSkillHeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE460
static int32_t Script_AddSkillUp(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE4D0
static int32_t Script_RemoveSkillUp(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE540
static int32_t Script_GetAdjustedSkillPoints(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE5D0
static int32_t Script_AcceptSkillUps(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CD820
static int32_t Script_CancelSkillUps(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CE6F0
static int32_t Script_BuySkillTier(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CD860
static int32_t Script_SetSelectedSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5CD8F0
static int32_t Script_GetSelectedSkill(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void SkillInfoRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_SKILL_INFO; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_SkillInfo[i].name,
            GameScript::s_ScriptFunctions_SkillInfo[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_SkillInfo[NUM_SCRIPT_FUNCTIONS_SKILL_INFO] = {
    { "GetNumSkillLines", &Script_GetNumSkillLines },
    { "GetSkillLineInfo", &Script_GetSkillLineInfo },
    { "AbandonSkill", &Script_AbandonSkill },
    { "CollapseSkillHeader", &Script_CollapseSkillHeader },
    { "ExpandSkillHeader", &Script_ExpandSkillHeader },
    { "AddSkillUp", &Script_AddSkillUp },
    { "RemoveSkillUp", &Script_RemoveSkillUp },
    { "GetAdjustedSkillPoints", &Script_GetAdjustedSkillPoints },
    { "AcceptSkillUps", &Script_AcceptSkillUps },
    { "CancelSkillUps", &Script_CancelSkillUps },
    { "BuySkillTier", &Script_BuySkillTier },
    { "SetSelectedSkill", &Script_SetSelectedSkill },
    { "GetSelectedSkill", &Script_GetSelectedSkill },
};
