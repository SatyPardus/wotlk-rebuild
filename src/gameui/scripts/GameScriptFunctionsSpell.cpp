#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x7FDCD0
static int32_t Script_SpellIsTargeting(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x7FDD10
static int32_t Script_SpellCanTargetItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x8007E0
static int32_t Script_SpellTargetItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x804190
static int32_t Script_SpellCanTargetUnit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x80DC00
static int32_t Script_SpellTargetUnit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x7FDD60
static int32_t Script_SpellCanTargetGlyph(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x809E30
static int32_t Script_SpellStopTargeting(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x809EA0
static int32_t Script_SpellStopCasting(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x804220
static int32_t Script_CancelUnitBuff(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x8021D0
static int32_t Script_CancelItemTempEnchantment(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x802270
static int32_t Script_CannotBeResurrected(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void SpellRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_SPELL; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_Spell[i].name,
            GameScript::s_ScriptFunctions_Spell[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_Spell[NUM_SCRIPT_FUNCTIONS_SPELL] = {
    { "SpellIsTargeting", &Script_SpellIsTargeting },
    { "SpellCanTargetItem", &Script_SpellCanTargetItem },
    { "SpellTargetItem", &Script_SpellTargetItem },
    { "SpellCanTargetUnit", &Script_SpellCanTargetUnit },
    { "SpellTargetUnit", &Script_SpellTargetUnit },
    { "SpellCanTargetGlyph", &Script_SpellCanTargetGlyph },
    { "SpellStopTargeting", &Script_SpellStopTargeting },
    { "SpellStopCasting", &Script_SpellStopCasting },
    { "CancelUnitBuff", &Script_CancelUnitBuff },
    { "CancelItemTempEnchantment", &Script_CancelItemTempEnchantment },
    { "CannotBeResurrected", &Script_CannotBeResurrected },
};
