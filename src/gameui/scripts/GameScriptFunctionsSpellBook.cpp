#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x53B5C0
static int32_t Script_GetNumSpellTabs(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x53BE70
static int32_t Script_GetSpellTabInfo(lua_State* L) {
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(6);
}

// OFFSET: 0x5407F0
static int32_t Script_GetSpellName(lua_State* L) {
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    WHOA_UNIMPLEMENTED(2);
}

// OFFSET: 0x5408E0
static int32_t Script_GetSpellLink(lua_State* L) {
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    WHOA_UNIMPLEMENTED(2);
}

// OFFSET: 0x540A30
static int32_t Script_GetSpellInfo(lua_State* L) {
    lua_pushstring(L, "");
    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(9);
}

// OFFSET: 0x540D70
static int32_t Script_GetSpellTexture(lua_State* L) {
    lua_pushstring(L, "");
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x540DF0
static int32_t Script_GetSpellCount(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x540E80
static int32_t Script_GetSpellCooldown(lua_State* L) {
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(3);
}

// OFFSET: 0x541010
static int32_t Script_GetSpellAutocast(lua_State* L) {
    lua_pushnil(L);
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(2);
}

// OFFSET: 0x5410E0
static int32_t Script_ToggleSpellAutocast(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541140
static int32_t Script_EnableSpellAutocast(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5411A0
static int32_t Script_DisableSpellAutocast(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541200
static int32_t Script_PickupSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541250
static int32_t Script_CastSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5412C0
static int32_t Script_IsSelectedSpell(lua_State* L) {
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x541340
static int32_t Script_IsPassiveSpell(lua_State* L) {
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x541420
static int32_t Script_IsAttackSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541500
static int32_t Script_IsCurrentSpell(lua_State* L) {
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x5415D0
static int32_t Script_IsAutoRepeatSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541680
static int32_t Script_IsUsableSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541800
static int32_t Script_IsHelpfulSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5418F0
static int32_t Script_IsHarmfulSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5419F0
static int32_t Script_IsConsumableSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541AF0
static int32_t Script_SpellHasRange(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x541C60
static int32_t Script_IsSpellInRange(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53DD10
static int32_t Script_UpdateSpells(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53DD30
static int32_t Script_HasPetSpells(lua_State* L) {
    lua_pushnil(L);
    lua_pushstring(L, "");
    WHOA_UNIMPLEMENTED(2);
}

// OFFSET: 0x53C0B0
static int32_t Script_GetNumShapeshiftForms(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x53DE10
static int32_t Script_GetShapeshiftForm(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53DE60
static int32_t Script_CancelShapeshiftForm(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53DE90
static int32_t Script_GetShapeshiftFormInfo(lua_State* L) {
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    lua_pushnil(L);
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(4);
}

// OFFSET: 0x53C0F0
static int32_t Script_CastShapeshiftForm(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53C180
static int32_t Script_GetShapeshiftFormCooldown(lua_State* L) {
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(3);
}

// OFFSET: 0x540310
static int32_t Script_CastSpellByName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53E060
static int32_t Script_CastSpellByID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53C2A0
static int32_t Script_GetNumCompanions(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x53E2C0
static int32_t Script_GetCompanionInfo(lua_State* L) {
    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(5);
}

// OFFSET: 0x53E490
static int32_t Script_GetCompanionCooldown(lua_State* L) {
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    WHOA_UNIMPLEMENTED(3);
}

// OFFSET: 0x53C310
static int32_t Script_PickupCompanion(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53E5E0
static int32_t Script_CallCompanion(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53E6B0
static int32_t Script_DismissCompanion(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53B650
static int32_t Script_GetKnownSlotFromHighestRankSlot(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x53C3A0
static int32_t Script_IsSpellKnown(lua_State* L) {
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(1);
}

// OFFSET: 0x53B6C0
static int32_t Script_FindSpellBookSlotByID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53E7E0
static int32_t Script_SummonRandomCritter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void SpellBookRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_SPELL_BOOK; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_SpellBook[i].name,
            GameScript::s_ScriptFunctions_SpellBook[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_SpellBook[NUM_SCRIPT_FUNCTIONS_SPELL_BOOK] = {
    { "GetNumSpellTabs", &Script_GetNumSpellTabs },
    { "GetSpellTabInfo", &Script_GetSpellTabInfo },
    { "GetSpellName", &Script_GetSpellName },
    { "GetSpellLink", &Script_GetSpellLink },
    { "GetSpellInfo", &Script_GetSpellInfo },
    { "GetSpellTexture", &Script_GetSpellTexture },
    { "GetSpellCount", &Script_GetSpellCount },
    { "GetSpellCooldown", &Script_GetSpellCooldown },
    { "GetSpellAutocast", &Script_GetSpellAutocast },
    { "ToggleSpellAutocast", &Script_ToggleSpellAutocast },
    { "EnableSpellAutocast", &Script_EnableSpellAutocast },
    { "DisableSpellAutocast", &Script_DisableSpellAutocast },
    { "PickupSpell", &Script_PickupSpell },
    { "CastSpell", &Script_CastSpell },
    { "IsSelectedSpell", &Script_IsSelectedSpell },
    { "IsPassiveSpell", &Script_IsPassiveSpell },
    { "IsAttackSpell", &Script_IsAttackSpell },
    { "IsCurrentSpell", &Script_IsCurrentSpell },
    { "IsAutoRepeatSpell", &Script_IsAutoRepeatSpell },
    { "IsUsableSpell", &Script_IsUsableSpell },
    { "IsHelpfulSpell", &Script_IsHelpfulSpell },
    { "IsHarmfulSpell", &Script_IsHarmfulSpell },
    { "IsConsumableSpell", &Script_IsConsumableSpell },
    { "SpellHasRange", &Script_SpellHasRange },
    { "IsSpellInRange", &Script_IsSpellInRange },
    { "UpdateSpells", &Script_UpdateSpells },
    { "HasPetSpells", &Script_HasPetSpells },
    { "GetNumShapeshiftForms", &Script_GetNumShapeshiftForms },
    { "GetShapeshiftForm", &Script_GetShapeshiftForm },
    { "CancelShapeshiftForm", &Script_CancelShapeshiftForm },
    { "GetShapeshiftFormInfo", &Script_GetShapeshiftFormInfo },
    { "CastShapeshiftForm", &Script_CastShapeshiftForm },
    { "GetShapeshiftFormCooldown", &Script_GetShapeshiftFormCooldown },
    { "CastSpellByName", &Script_CastSpellByName },
    { "CastSpellByID", &Script_CastSpellByID },
    { "GetNumCompanions", &Script_GetNumCompanions },
    { "GetCompanionInfo", &Script_GetCompanionInfo },
    { "GetCompanionCooldown", &Script_GetCompanionCooldown },
    { "PickupCompanion", &Script_PickupCompanion },
    { "CallCompanion", &Script_CallCompanion },
    { "DismissCompanion", &Script_DismissCompanion },
    { "GetKnownSlotFromHighestRankSlot", &Script_GetKnownSlotFromHighestRankSlot },
    { "IsSpellKnown", &Script_IsSpellKnown },
    { "FindSpellBookSlotByID", &Script_FindSpellBookSlotByID },
    { "SummonRandomCritter", &Script_SummonRandomCritter },
};
