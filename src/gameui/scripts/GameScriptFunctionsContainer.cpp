#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5D6F60
static int32_t Script_ContainerIDToInventoryID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D74A0
static int32_t Script_GetContainerNumSlots(lua_State* L) {
    // TODO
    lua_pushnumber(L, 16.0);
    return 1;
}

// OFFSET: 0x5D7A90
static int32_t Script_GetContainerItemInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7D00
static int32_t Script_GetContainerItemID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7C80
static int32_t Script_GetContainerItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7D90
static int32_t Script_GetContainerItemCooldown(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7FF0
static int32_t Script_PickupContainerItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D84F0
static int32_t Script_SplitContainerItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D8650
static int32_t Script_UseContainerItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D8B10
static int32_t Script_SocketContainerItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D8BD0
static int32_t Script_ShowContainerSellCursor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7180
static int32_t Script_SetBagPortraitTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D8C70
static int32_t Script_GetBagName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7EF0
static int32_t Script_GetContainerItemDurability(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7590
static int32_t Script_GetContainerNumFreeSlots(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D7820
static int32_t Script_GetContainerFreeSlots(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D8D80
static int32_t Script_GetContainerItemPurchaseInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D8F70
static int32_t Script_GetContainerItemPurchaseItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D91B0
static int32_t Script_ContainerRefundItemPurchase(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D6FF0
static int32_t Script_GetMaxArenaCurrency(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D9300
static int32_t Script_GetContainerItemGems(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5D9400
static int32_t Script_GetContainerItemQuestInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void ContainerRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_CONTAINER; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_Container[i].name,
            GameScript::s_ScriptFunctions_Container[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_Container[NUM_SCRIPT_FUNCTIONS_CONTAINER] = {
    { "ContainerIDToInventoryID", &Script_ContainerIDToInventoryID },
    { "GetContainerNumSlots", &Script_GetContainerNumSlots },
    { "GetContainerItemInfo", &Script_GetContainerItemInfo },
    { "GetContainerItemID", &Script_GetContainerItemID },
    { "GetContainerItemLink", &Script_GetContainerItemLink },
    { "GetContainerItemCooldown", &Script_GetContainerItemCooldown },
    { "PickupContainerItem", &Script_PickupContainerItem },
    { "SplitContainerItem", &Script_SplitContainerItem },
    { "UseContainerItem", &Script_UseContainerItem },
    { "SocketContainerItem", &Script_SocketContainerItem },
    { "ShowContainerSellCursor", &Script_ShowContainerSellCursor },
    { "SetBagPortraitTexture", &Script_SetBagPortraitTexture },
    { "GetBagName", &Script_GetBagName },
    { "GetContainerItemDurability", &Script_GetContainerItemDurability },
    { "GetContainerNumFreeSlots", &Script_GetContainerNumFreeSlots },
    { "GetContainerFreeSlots", &Script_GetContainerFreeSlots },
    { "GetContainerItemPurchaseInfo", &Script_GetContainerItemPurchaseInfo },
    { "GetContainerItemPurchaseItem", &Script_GetContainerItemPurchaseItem },
    { "ContainerRefundItemPurchase", &Script_ContainerRefundItemPurchase },
    { "GetMaxArenaCurrency", &Script_GetMaxArenaCurrency },
    { "GetContainerItemGems", &Script_GetContainerItemGems },
    { "GetContainerItemQuestInfo", &Script_GetContainerItemQuestInfo },
};
