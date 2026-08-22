#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x564AE0
static int32_t Script_SecureCmdOptionParse(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566E80
static int32_t Script_RunMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566400
static int32_t Script_RunMacroText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x564B90
static int32_t Script_StopMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x568160
static int32_t Script_CreateMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x564BB0
static int32_t Script_GetNumMacros(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566EB0
static int32_t Script_GetMacroInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566F40
static int32_t Script_GetMacroBody(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566F70
static int32_t Script_DeleteMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566FA0
static int32_t Script_EditMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5670D0
static int32_t Script_SetMacroItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x567200
static int32_t Script_GetMacroItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x567320
static int32_t Script_SetMacroSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x567450
static int32_t Script_GetMacroSpell(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x566490
static int32_t Script_GetNumMacroIcons(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5664E0
static int32_t Script_GetNumMacroItemIcons(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x564E90
static int32_t Script_GetMacroIconInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x564F60
static int32_t Script_GetMacroItemIconInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5674F0
static int32_t Script_PickupMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5666A0
static int32_t Script_GetMacroIndexByName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x564C10
static int32_t Script_GetRunningMacro(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x564C70
static int32_t Script_GetRunningMacroButton(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void UIMacrosRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_UIMACROS; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_UIMacros[i].name,
            GameScript::s_ScriptFunctions_UIMacros[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_UIMacros[NUM_SCRIPT_FUNCTIONS_UIMACROS] = {
    { "SecureCmdOptionParse", &Script_SecureCmdOptionParse },
    { "RunMacro", &Script_RunMacro },
    { "RunMacroText", &Script_RunMacroText },
    { "StopMacro", &Script_StopMacro },
    { "CreateMacro", &Script_CreateMacro },
    { "GetNumMacros", &Script_GetNumMacros },
    { "GetMacroInfo", &Script_GetMacroInfo },
    { "GetMacroBody", &Script_GetMacroBody },
    { "DeleteMacro", &Script_DeleteMacro },
    { "EditMacro", &Script_EditMacro },
    { "SetMacroItem", &Script_SetMacroItem },
    { "GetMacroItem", &Script_GetMacroItem },
    { "SetMacroSpell", &Script_SetMacroSpell },
    { "GetMacroSpell", &Script_GetMacroSpell },
    { "GetNumMacroIcons", &Script_GetNumMacroIcons },
    { "GetNumMacroItemIcons", &Script_GetNumMacroItemIcons },
    { "GetMacroIconInfo", &Script_GetMacroIconInfo },
    { "GetMacroItemIconInfo", &Script_GetMacroItemIconInfo },
    { "PickupMacro", &Script_PickupMacro },
    { "GetMacroIndexByName", &Script_GetMacroIndexByName },
    { "GetRunningMacro", &Script_GetRunningMacro },
    { "GetRunningMacroButton", &Script_GetRunningMacroButton },
};
