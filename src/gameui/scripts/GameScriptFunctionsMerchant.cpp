#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5846D0
static int32_t Script_CloseMerchant(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5841D0
static int32_t Script_GetMerchantNumItems(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x584E10
static int32_t Script_GetMerchantItemInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5846E0
static int32_t Script_GetMerchantItemCostInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x584820
static int32_t Script_GetMerchantItemCostItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x585070
static int32_t Script_GetBuybackItemInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5852C0
static int32_t Script_GetBuybackItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x584200
static int32_t Script_GetMerchantItemLink(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5842D0
static int32_t Script_GetMerchantItemMaxStack(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5853A0
static int32_t Script_PickupMerchantItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5854C0
static int32_t Script_BuyMerchantItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5855C0
static int32_t Script_BuybackItem(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5849F0
static int32_t Script_CanMerchantRepair(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5856D0
static int32_t Script_ShowMerchantSellCursor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x585890
static int32_t Script_ShowBuybackSellCursor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x584A60
static int32_t Script_ShowRepairCursor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x584390
static int32_t Script_HideRepairCursor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5843B0
static int32_t Script_InRepairMode(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x585990
static int32_t Script_GetRepairAllCost(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x585C90
static int32_t Script_RepairAllItems(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5843F0
static int32_t Script_GetNumBuybackItems(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

void MerchantRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_MERCHANT; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_Merchant[i].name,
            GameScript::s_ScriptFunctions_Merchant[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_Merchant[NUM_SCRIPT_FUNCTIONS_MERCHANT] = {
    { "CloseMerchant", &Script_CloseMerchant },
    { "GetMerchantNumItems", &Script_GetMerchantNumItems },
    { "GetMerchantItemInfo", &Script_GetMerchantItemInfo },
    { "GetMerchantItemCostInfo", &Script_GetMerchantItemCostInfo },
    { "GetMerchantItemCostItem", &Script_GetMerchantItemCostItem },
    { "GetBuybackItemInfo", &Script_GetBuybackItemInfo },
    { "GetBuybackItemLink", &Script_GetBuybackItemLink },
    { "GetMerchantItemLink", &Script_GetMerchantItemLink },
    { "GetMerchantItemMaxStack", &Script_GetMerchantItemMaxStack },
    { "PickupMerchantItem", &Script_PickupMerchantItem },
    { "BuyMerchantItem", &Script_BuyMerchantItem },
    { "BuybackItem", &Script_BuybackItem },
    { "CanMerchantRepair", &Script_CanMerchantRepair },
    { "ShowMerchantSellCursor", &Script_ShowMerchantSellCursor },
    { "ShowBuybackSellCursor", &Script_ShowBuybackSellCursor },
    { "ShowRepairCursor", &Script_ShowRepairCursor },
    { "HideRepairCursor", &Script_HideRepairCursor },
    { "InRepairMode", &Script_InRepairMode },
    { "GetRepairAllCost", &Script_GetRepairAllCost },
    { "RepairAllItems", &Script_RepairAllItems },
    { "GetNumBuybackItems", &Script_GetNumBuybackItems },
};
