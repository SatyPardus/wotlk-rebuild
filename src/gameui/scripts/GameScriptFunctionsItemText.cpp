#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x589C40
static int32_t Script_ItemTextGetItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A480
static int32_t Script_ItemTextGetCreator(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x589C90
static int32_t Script_ItemTextGetMaterial(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x589B50
static int32_t Script_ItemTextGetPage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x589B90
static int32_t Script_ItemTextGetText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x589D70
static int32_t Script_ItemTextHasNextPage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A110
static int32_t Script_ItemTextPrevPage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A150
static int32_t Script_ItemTextNextPage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x58A400
static int32_t Script_CloseItemText(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void ItemTextRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_ITEM_TEXT; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_ItemText[i].name,
            GameScript::s_ScriptFunctions_ItemText[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_ItemText[NUM_SCRIPT_FUNCTIONS_ITEM_TEXT] = {
    { "ItemTextGetItem", &Script_ItemTextGetItem },
    { "ItemTextGetCreator", &Script_ItemTextGetCreator },
    { "ItemTextGetMaterial", &Script_ItemTextGetMaterial },
    { "ItemTextGetPage", &Script_ItemTextGetPage },
    { "ItemTextGetText", &Script_ItemTextGetText },
    { "ItemTextHasNextPage", &Script_ItemTextHasNextPage },
    { "ItemTextPrevPage", &Script_ItemTextPrevPage },
    { "ItemTextNextPage", &Script_ItemTextNextPage },
    { "CloseItemText", &Script_CloseItemText },
};
