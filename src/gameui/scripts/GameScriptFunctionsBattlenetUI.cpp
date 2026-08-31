#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"


// OFFSET: 0x5343F0
static int32_t Script_BNGetInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x534590
static int32_t Script_BNGetNumFriends(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x539BF0
static int32_t Script_BNGetFriendInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x539CC0
static int32_t Script_BNGetFriendInfoByID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5349F0
static int32_t Script_BNGetNumFriendToons(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x539D70
static int32_t Script_BNGetFriendToonInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x539F90
static int32_t Script_BNGetToonInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x534ED0
static int32_t Script_BNRemoveFriend(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x534F80
static int32_t Script_BNSetFriendNote(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537510
static int32_t Script_BNSetSelectedFriend(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537600
static int32_t Script_BNGetSelectedFriend(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535080
static int32_t Script_BNGetNumFriendInvites(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x535180
static int32_t Script_BNGetFriendInviteInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535380
static int32_t Script_BNSendFriendInvite(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535490
static int32_t Script_BNSendFriendInviteByID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5355C0
static int32_t Script_BNAcceptFriendInvite(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535660
static int32_t Script_BNDeclineFriendInvite(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535700
static int32_t Script_BNReportFriendInvite(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5357A0
static int32_t Script_BNSetAFK(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535860
static int32_t Script_BNSetDND(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535920
static int32_t Script_BNSetCustomMessage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535AA0
static int32_t Script_BNGetCustomMessageTable(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535C60
static int32_t Script_BNSetFocus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53A030
static int32_t Script_BNSendWhisper(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535CE0
static int32_t Script_BNCreateConversation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x535EB0
static int32_t Script_BNInviteToConversation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536030
static int32_t Script_BNLeaveConversation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536110
static int32_t Script_BNSendConversationMessage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536220
static int32_t Script_BNGetNumConversationMembers(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53A150
static int32_t Script_BNGetConversationMemberInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536330
static int32_t Script_BNGetConversationInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53A300
static int32_t Script_BNListConversation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536400
static int32_t Script_BNGetNumBlocked(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x53A540
static int32_t Script_BNGetBlockedInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5364E0
static int32_t Script_BNIsBlocked(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5365B0
static int32_t Script_BNSetBlocked(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5366A0
static int32_t Script_BNSetSelectedBlock(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536790
static int32_t Script_BNGetSelectedBlock(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536890
static int32_t Script_BNGetNumBlockedToons(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536970
static int32_t Script_BNGetBlockedToonInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536A90
static int32_t Script_BNIsToonBlocked(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536B60
static int32_t Script_BNSetToonBlocked(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536C50
static int32_t Script_BNSetSelectedToonBlock(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536D40
static int32_t Script_BNGetSelectedToonBlock(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x536E40
static int32_t Script_BNReportPlayer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x530EC0
static int32_t Script_BNConnected(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537010
static int32_t Script_BNFeaturesEnabledAndConnected(lua_State* L) {
    // TODO
    // ##############
    bool v2 = false;
    // ############
    //NetClientPtr = GetNetClientPtr();
    //if (NetClientPtr && NetClientPtr->GetLoginServerType(NetClientPtr) == 1 && !(NetClientPtr->IsTrialAccount)(NetClientPtr) && BattlenetAPI__IsRIDEnabled())
    //    v2 = maybe_BattlenetUI__IsConnected(NetClientPtr);
    //else
    //    v2 = 0;
    lua_pushboolean(L, v2);
    return 1;
}

// OFFSET: 0x530F20
static int32_t Script_IsBNLogin(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537070
static int32_t Script_BNFeaturesEnabled(lua_State* L) {
    // TODO
    // ##############
    bool v2 = false;
    //############
    //NetClientPtr = GetNetClientPtr();
    //v2 = NetClientPtr && NetClientPtr->GetLoginServerType(NetClientPtr) == 1 && !(NetClientPtr->IsTrialAccount)(NetClientPtr) && BattlenetAPI__IsRIDEnabled();
    lua_pushboolean(L, v2);
    return 1;
}

// OFFSET: 0x53A660
static int32_t Script_BNRequestFOFInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5370D0
static int32_t Script_BNGetNumFOF(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537240
static int32_t Script_BNGetFOFInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5376C0
static int32_t Script_BNSetMatureLanguageFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5377C0
static int32_t Script_BNGetMatureLanguageFilter(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x5378A0
static int32_t Script_BNIsSelf(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537950
static int32_t Script_BNIsFriend(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x537A00
static int32_t Script_BNGetMaxPlayersInConversation(lua_State* L) {
    // TODO: LoginConnection
    lua_pushnumber(L, 12.0);
    return 1;
}

void BattlenetUIRegisterScriptFunctions() {
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_BATTLENET_UI; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_BattlenetUI[i].name,
            GameScript::s_ScriptFunctions_BattlenetUI[i].method);
    }
}

FrameScript_Method GameScript::s_ScriptFunctions_BattlenetUI[NUM_SCRIPT_FUNCTIONS_BATTLENET_UI] = {
    { "BNGetInfo", &Script_BNGetInfo },
    { "BNGetNumFriends", &Script_BNGetNumFriends },
    { "BNGetFriendInfo", &Script_BNGetFriendInfo },
    { "BNGetFriendInfoByID", &Script_BNGetFriendInfoByID },
    { "BNGetNumFriendToons", &Script_BNGetNumFriendToons },
    { "BNGetFriendToonInfo", &Script_BNGetFriendToonInfo },
    { "BNGetToonInfo", &Script_BNGetToonInfo },
    { "BNRemoveFriend", &Script_BNRemoveFriend },
    { "BNSetFriendNote", &Script_BNSetFriendNote },
    { "BNSetSelectedFriend", &Script_BNSetSelectedFriend },
    { "BNGetSelectedFriend", &Script_BNGetSelectedFriend },
    { "BNGetNumFriendInvites", &Script_BNGetNumFriendInvites },
    { "BNGetFriendInviteInfo", &Script_BNGetFriendInviteInfo },
    { "BNSendFriendInvite", &Script_BNSendFriendInvite },
    { "BNSendFriendInviteByID", &Script_BNSendFriendInviteByID },
    { "BNAcceptFriendInvite", &Script_BNAcceptFriendInvite },
    { "BNDeclineFriendInvite", &Script_BNDeclineFriendInvite },
    { "BNReportFriendInvite", &Script_BNReportFriendInvite },
    { "BNSetAFK", &Script_BNSetAFK },
    { "BNSetDND", &Script_BNSetDND },
    { "BNSetCustomMessage", &Script_BNSetCustomMessage },
    { "BNGetCustomMessageTable", &Script_BNGetCustomMessageTable },
    { "BNSetFocus", &Script_BNSetFocus },
    { "BNSendWhisper", &Script_BNSendWhisper },
    { "BNCreateConversation", &Script_BNCreateConversation },
    { "BNInviteToConversation", &Script_BNInviteToConversation },
    { "BNLeaveConversation", &Script_BNLeaveConversation },
    { "BNSendConversationMessage", &Script_BNSendConversationMessage },
    { "BNGetNumConversationMembers", &Script_BNGetNumConversationMembers },
    { "BNGetConversationMemberInfo", &Script_BNGetConversationMemberInfo },
    { "BNGetConversationInfo", &Script_BNGetConversationInfo },
    { "BNListConversation", &Script_BNListConversation },
    { "BNGetNumBlocked", &Script_BNGetNumBlocked },
    { "BNGetBlockedInfo", &Script_BNGetBlockedInfo },
    { "BNIsBlocked", &Script_BNIsBlocked },
    { "BNSetBlocked", &Script_BNSetBlocked },
    { "BNSetSelectedBlock", &Script_BNSetSelectedBlock },
    { "BNGetSelectedBlock", &Script_BNGetSelectedBlock },
    { "BNGetNumBlockedToons", &Script_BNGetNumBlockedToons },
    { "BNGetBlockedToonInfo", &Script_BNGetBlockedToonInfo },
    { "BNIsToonBlocked", &Script_BNIsToonBlocked },
    { "BNSetToonBlocked", &Script_BNSetToonBlocked },
    { "BNSetSelectedToonBlock", &Script_BNSetSelectedToonBlock },
    { "BNGetSelectedToonBlock", &Script_BNGetSelectedToonBlock },
    { "BNReportPlayer", &Script_BNReportPlayer },
    { "BNConnected", &Script_BNConnected },
    { "BNFeaturesEnabledAndConnected", &Script_BNFeaturesEnabledAndConnected },
    { "IsBNLogin", &Script_IsBNLogin },
    { "BNFeaturesEnabled", &Script_BNFeaturesEnabled },
    { "BNRequestFOFInfo", &Script_BNRequestFOFInfo },
    { "BNGetNumFOF", &Script_BNGetNumFOF },
    { "BNGetFOFInfo", &Script_BNGetFOFInfo },
    { "BNSetMatureLanguageFilter", &Script_BNSetMatureLanguageFilter },
    { "BNGetMatureLanguageFilter", &Script_BNGetMatureLanguageFilter },
    { "BNIsSelf", &Script_BNIsSelf },
    { "BNIsFriend", &Script_BNIsFriend },
    { "BNGetMaxPlayersInConversation", &Script_BNGetMaxPlayersInConversation },
};
