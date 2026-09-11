#include "gameui/GameCVars.hpp"
#include "gameui/CGGameUI.hpp"
#include "gameui/camera/CameraCVars.hpp"
#include "util/Unimplemented.hpp"
#include <storm/String.hpp>
#include <clientobject/PlayerName.hpp>

CVar* g_minimapPortalMaxCVar;
CVar* g_showToastBroadcastCVar;
CVar* g_showToastConversationCVar;
CVar* g_ShowClassColorInNameplateCVar;
CVar* g_assistAttackCVar;
CVar* g_autoClearAFKCVar;
CVar* g_autoDismountCVar;
CVar* g_autoDismountFlyingCVar;
CVar* g_autoLootDefaultCVar;
CVar* g_autoSelfCastCVar;
CVar* g_autoStandCVar;
CVar* g_autoUnshiftCVar;
CVar* g_autojoinBGVoiceCVar;
CVar* g_autojoinPartyVoiceCVar;
CVar* g_blockTradesCVar;
CVar* g_buffDurationsCVar;
CVar* g_chatBubblesCVar;
CVar* g_chatBubblesPartyCVar;
CVar* g_combatDamageCVar;
CVar* g_combatHealingCVar;
CVar* g_combatLogRetentionTimeCVar;
CVar* g_currencyTokensBackpack1CVar;
CVar* g_currencyTokensBackpack2CVar;
CVar* g_currencyTokensUnused1CVar;
CVar* g_currencyTokensUnused2CVar;
CVar* g_cvAutoInteract;
CVar* g_deselectOnClickCVar;
CVar* g_displayFreeBagSlotsCVar;
CVar* g_displayWorldPVPObjectivesCVar;
CVar* g_enablePVPNotifyAFKCVar;
CVar* g_guildMemberNotifyCVar;
CVar* g_guildRecruitmentChannelCVar;
CVar* g_guildShowOfflineCVar;
CVar* g_lootUnderMouseCVar;
CVar* g_minimapInsideZoomCVar;
CVar* g_minimapZoomCVar;
CVar* g_nameplateShowEnemiesCVar;
CVar* g_nameplateShowEnemyGuardiansCVar;
CVar* g_nameplateShowEnemyPetsCVar;
CVar* g_nameplateShowEnemyTotemsCVar;
CVar* g_nameplateShowFriendlyGuardiansCVar;
CVar* g_nameplateShowFriendlyPetsCVar;
CVar* g_nameplateShowFriendlyTotemsCVar;
CVar* g_nameplateShowFriendsCVar;
CVar* g_petMeleeDamageCVar;
CVar* g_petSpellDamageCVar;
CVar* g_predictedHealthCVar;
CVar* g_predictedPowerCVar;
CVar* g_previewTalentsCVar;
CVar* g_profanityFilterCVar;
CVar* g_questPOICVar;
CVar* g_removeChatDelayCVar;
CVar* g_rotateMinimapCVar;
CVar* g_screenEdgeFlashCVar;
CVar* g_scriptErrorsCVar;
CVar* g_secureAbilityToggleCVar;
CVar* g_serviceTypeFilterCVar;
CVar* g_showLootSpamCVar;
CVar* g_showTargetCastbarCVar;
CVar* g_showTargetOfTargetCVar;
CVar* g_showVKeyCastbarCVar;
CVar* g_spamFilterCvar;
CVar* g_stopAutoAttackOnTargetChangeCVar;
CVar* g_targetOfTargetModeCVar;
CVar* g_threatWarningCVar;
CVar* g_threatWorldTextCVar;
CVar* g_trackerSortingCVar;
CVar* g_unitHighlightsCVar;
CVar* s_cvAutoFilledMultiCastSlots;
CVar* s_cvAutoRangedCombat;
CVar* s_cvCalendarShowBattlegrounds;
CVar* s_cvCalendarShowDarkmoon;
CVar* s_cvCalendarShowLockouts;
CVar* s_cvCalendarShowResets;
CVar* s_cvCalendarShowWeeklyHolidays;
CVar* s_cvCombatLogPeriodicSpells;
CVar* s_cvLfdCollapsedHeaders;
CVar* s_cvLfdSelectedDungeons;
CVar* s_cvLfgSelectedRoles;
CVar* s_cvNameplateAllowOverlap;
CVar* s_cvQuestLogCollapseFilter;
CVar* s_cvShowAllSpellRanks;
CVar* s_cvShowToastFriendRequest;
CVar* s_cvShowToastOffline;
CVar* s_cvShowToastOnline;
CVar* s_cvThreatPlaySounds;
CVar* s_cvThreatShowNumeric;

// OFFSET: 0x512700
bool AutoInteractStateChangedCallback(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x512830
bool CVarHandler_autoCompleteResortNamesOnRecency(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x512870
bool CVarHandler_autoCompleteUseContext(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x512850
bool CVarHandler_autoCompleteWhenEditingFromCenter(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x512750
bool CVarHandler_GuildRecruitmentMode(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x512730
bool GuildShowOfflineCallback(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5127A0
bool SetCalendarFilterCVarCallback(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x512770
bool ShowClassColorInNameplateCallback(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5127D0
bool SignalPreviewTalentPointsChanged(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x518BD0
bool UnitHighlightsCVarCallback(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x51D990
bool CVarHandler_ThreatWarning(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x51D9B0
void CGGameUI::RegisterGameCVars() {
    g_deselectOnClickCVar = CVar::Register("deselectOnClick", "Clear the target when clicking on terrain", 16, "1", nullptr, 4, 0, nullptr, 0);
    const char* autoInteractDefault = "1";
    // if (GetLocaleIndex(<current locale>) != 1) {
    //     autoInteractDefault = "0";
    // }
    g_cvAutoInteract = CVar::Register("autoInteract", "Toggles auto-move to interact target", 16, autoInteractDefault, &AutoInteractStateChangedCallback, 4, 0, nullptr, 0);
    g_autoStandCVar = CVar::Register("autoStand", "Automatically stand when needed", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_autoDismountCVar = CVar::Register("autoDismount", "Automatically dismount when needed", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_autoDismountFlyingCVar = CVar::Register("autoDismountFlying", "If enabled, your character will automatically dismount before casting while flying", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_autoUnshiftCVar = CVar::Register("autoUnshift", "Automatically leave shapeshift form when needed", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_autoClearAFKCVar = CVar::Register("autoClearAFK", "Automatically clear AFK when moving or chatting", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_blockTradesCVar = CVar::Register("blockTrades", "Whether to automatically block trade requests", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_lootUnderMouseCVar = CVar::Register("lootUnderMouse", "Whether the loot window should open under the mouse", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_autoLootDefaultCVar = CVar::Register("autoLootDefault", "Automatically loot items when the loot window opens", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("alwaysCompareItems", "Always show item comparison tooltips", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("equipmentManager", "Enables the equipment management UI", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_assistAttackCVar = CVar::Register("assistAttack", "Whether to start attacking after an assist", 32, "0", nullptr, 4, 0, nullptr, 0);
    s_cvAutoRangedCombat = CVar::Register("autoRangedCombat", "Whether your character will automatically switch between auto attack and auto shot", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_autoSelfCastCVar = CVar::Register("autoSelfCast", "Whether spells should automatically be cast on you if you don't have a valid target", 32, "1", nullptr, 4, 0, nullptr, 0);
    g_stopAutoAttackOnTargetChangeCVar = CVar::Register("stopAutoAttackOnTargetChange", "Whether to stop attacking when changing targets", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_showTargetOfTargetCVar = CVar::Register("showTargetOfTarget", "Whether the target of target frame should be shown", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_targetOfTargetModeCVar = CVar::Register("targetOfTargetMode", "The conditions under which target of target should be shown", 16, "5", nullptr, 4, 0, nullptr, 0);
    g_showTargetCastbarCVar = CVar::Register("showTargetCastbar", "Show the spell your current target is casting", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_showVKeyCastbarCVar = CVar::Register("showVKeyCastbar", "If the V key display is up for your current target, show the enemy cast bar with the target's health bar in the game field", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_rotateMinimapCVar = CVar::Register("rotateMinimap", "Whether to rotate the entire minimap instead of the player arrow", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_minimapZoomCVar = CVar::Register("minimapZoom", "The current outdoor minimap zoom level", 32, "3", nullptr, 4, 0, nullptr, 0);
    g_minimapInsideZoomCVar = CVar::Register("minimapInsideZoom", "The current indoor minimap zoom level", 32, "3", nullptr, 4, 0, nullptr, 0);
    g_minimapPortalMaxCVar = CVar::Register("minimapPortalMax", "Max Number of Portals to traverse for minimap", 32, "99", nullptr, 4, 0, nullptr, 0);
    g_scriptErrorsCVar = CVar::Register("scriptErrors", "Whether or not the UI shows Lua errors", 16, "0", 0, 5, 0, 0, 0);
    g_screenEdgeFlashCVar = CVar::Register("screenEdgeFlash", "Whether to show a red flash while you are in combat with the world map up", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_showLootSpamCVar = CVar::Register("showLootSpam", "Whether to show verbose loot rolls", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_displayFreeBagSlotsCVar = CVar::Register("displayFreeBagSlots", "Whether or not the backpack button should indicate how many inventory slots you've got free", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_displayWorldPVPObjectivesCVar = CVar::Register("displayWorldPVPObjectives", "Whether to show world PvP objectives", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showClock", "Whether to display the time manager's clock button", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("colorblindMode", "Enables colorblind accessibility features in the game", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("questFadingDisable", "Whether to disable quest text slowly fading in", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("autoQuestWatch", "Whether to automatically watch all quests when you obtain them", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("autoQuestProgress", "Whether to automatically watch all quests when they are updated", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showQuestTrackingTooltips", "Displays quest tracking information in unit and object tooltips", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("mapQuestDifficulty", "Whether to color quest titles by difficulty in the World Map", 32, "0", nullptr, 4, 0, nullptr, 0);
    s_cvQuestLogCollapseFilter = CVar::Register("questLogCollapseFilter", "bit filed for saving off the state of the headers in Quest Log", 32, "0", nullptr, 4, 0, nullptr, 0);
    s_cvQuestLogCollapseFilter->Set(-1, true, false, false, true);
    CVar::Register("advancedWatchFrame", "Enables advanced Objectives tracking features", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("watchFrameIgnoreCursor", "Disables Objectives frame mouseover and title dropdown.", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("watchFrameBaseAlpha", "Objectives frame opacity.", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("watchFrameState", "Stores Objectives frame locked and collapsed states", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showQuestObjectivesOnMap", "Shows quest POIs on the main map.", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("trackedQuests", "Internal cvar for saving tracked quests in order", 288, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("trackedAchievements", "Internal cvar for saving tracked achievements in order", 288, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("flaggedTutorials", "Internal cvar for saving compleated tutorials in order", 272, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("advancedWorldMap", "Enables advanced World Map features", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("worldMapOpacity", "Opacity for the world map when sized down", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("watchFrameWidth", "Controls objectives frame width", 0, "0", nullptr, 4, 0, nullptr, 0);
    g_trackerSortingCVar = CVar::Register("trackerSorting", "sorting option for the objectives tracker", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("trackerFilter", "filter option for the objectives tracker", 32, "7", nullptr, 4, 0, nullptr, 0);
    g_profanityFilterCVar = CVar::Register("profanityFilter", "Whether to enable mature language filtering", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_spamFilterCvar = CVar::Register("spamFilter", "Whether to enable spam filtering", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_chatBubblesCVar = CVar::Register("chatBubbles", "Whether to show in-game chat bubbles", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_chatBubblesPartyCVar = CVar::Register("chatBubblesParty", "Whether to show in-game chat bubbles for party chat", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_removeChatDelayCVar = CVar::Register("removeChatDelay", "Remove Chat Hover Delay", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_guildShowOfflineCVar = CVar::Register("guildShowOffline", "Show offline guild members in the guild UI", 16, "1", &GuildShowOfflineCallback, 4, 0, nullptr, 0);
    g_guildMemberNotifyCVar = CVar::Register("guildMemberNotify", "Receive notification when guild members log on/off", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_guildRecruitmentChannelCVar = CVar::Register("guildRecruitmentChannel", "Whether to automatically join the guild recruitment channel when not in a guild", 16, "1", &CVarHandler_GuildRecruitmentMode, 4, 0, nullptr, 0);
    CVar::Register("lfgAutoFill", "Whether to automatically add party members while looking for a group", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("lfgAutoJoin", "Whether to automatically join a party while looking for a group", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("friendsViewButtons", "Whether to show the friends list view buttons", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("friendsSmallView", "Whether to use smaller buttons in the friends list", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("chatStyle", "The style of Edit Boxes for the ChatFrame. Valid values: \"classic\", \"im\"", 16, "im", nullptr, 4, 0, nullptr, 0);
    CVar::Register("wholeChatWindowClickable", "Whether the user may click anywhere on a chat window to change EditBox focus (only works in IM style)", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("conversationMode", "The action new Real ID Conversations take by default: \"popout\", \"inline\"", 16, "popout", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showTimestamps", "The format of timestamps in chat or \"none\"", 16, "none", nullptr, 4, 0, nullptr, 0);
    CVar::Register("chatMouseScroll", "Whether the user can use the mouse wheel to scroll through chat", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("lockActionBars", "Whether the action bars should be locked, preventing changes", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("alwaysShowActionBars", "Whether to always show the action bar grid", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_secureAbilityToggleCVar = CVar::Register("secureAbilityToggle", "Whether you should be protected against accidentally double-clicking an aura", 16, "1", nullptr, 4, 0, nullptr, 0);
    PlayerNameRegisterCVars();
    g_combatDamageCVar = CVar::Register("CombatDamage", "Display damage numbers over hostile creatures when damaged", 16, "1", nullptr, 4, 0, nullptr, 0);
    s_cvCombatLogPeriodicSpells = CVar::Register("CombatLogPeriodicSpells", "Display damage caused by periodic effects", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_petMeleeDamageCVar = CVar::Register("PetMeleeDamage", "Display pet melee damage in the world", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_petSpellDamageCVar = CVar::Register("PetSpellDamage", "Display pet spell damage in the world", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_combatHealingCVar = CVar::Register("CombatHealing", "Display amount of healing you did to the target", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("enableCombatText", "Whether to show floating combat text", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("combatTextFloatMode", "The combat text float mode", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctCombatState", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctDodgeParryMiss", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctDamageReduction", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctRepChanges", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctReactives", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctFriendlyHealers", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctComboPoints", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctLowManaHealth", nullptr, 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctEnergyGains", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctPeriodicEnergyGains", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctHonorGains", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctAuras", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctAllSpellMechanics", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctSpellMechanics", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fctSpellMechanicsOther", nullptr, 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("xpBarText", "Whether the XP bar shows the numeric experience value", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("playerStatusText", "Whether the player portrait shows numeric health/mana values", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("petStatusText", "Whether the pet portrait shows numeric health/mana values", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("partyStatusText", "Whether the party portraits shows numeric health/mana values", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("targetStatusText", "Whether the target portrait shows numeric health/mana values", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("statusTextPercentage", "Whether numeric health/mana values are shown as raw values or percentages", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showPartyBackground", "Show a background behind party members", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("partyBackgroundOpacity", "The opacity of the party background", 16, "0.5", nullptr, 4, 0, nullptr, 0);
    CVar::Register("hidePartyInRaid", "Whether to hide the party UI while in a raid", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showPartyPets", "Whether to show pets in the party UI", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showRaidRange", "Show range indicator in raid UI", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showArenaEnemyFrames", "Show arena enemy frames while in an Arena", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showArenaEnemyCastbar", "Show the spell enemies are casting on the Arena Enemy frames", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showArenaEnemyPets", "Show the enemy team's pets on the ArenaEnemy frames", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("fullSizeFocusFrame", "Increases the size of the focus frame to that of the target frame", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_buffDurationsCVar = CVar::Register("buffDurations", "Whether to show buff durations", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showDispelDebuffs", "Show only Debuffs that the player can dispel.  Only applies to raids.", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showCastableBuffs", "Show only Buffs the player can cast.  Only applies to raids.", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("consolidateBuffs", "Consolidates buffs displayed for the player.", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showCastableDebuffs", "Show only debuffs the player can apply.", 32, "0", nullptr, 4, 0, nullptr, 0);
    s_cvShowToastOnline = CVar::Register("showToastOnline", "Whether to show Battle.net message for friend coming online", 16, "1", nullptr, 4, 0, nullptr, 0);
    s_cvShowToastOffline = CVar::Register("showToastOffline", "Whether to show Battle.net message for friend going offline", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_showToastBroadcastCVar = CVar::Register("showToastBroadcast", "Whether to show Battle.net message for broadcasts", 16, "1", nullptr, 4, 0, nullptr, 0);
    s_cvShowToastFriendRequest = CVar::Register("showToastFriendRequest", "Whether to show Battle.net message for friend requests", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_showToastConversationCVar = CVar::Register("showToastConversation", "Whether to show Battle.net message for conversations", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showToastWindow", "Whether to show Battle.net system messages in a toast window", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("toastDuration", "How long to display Battle.net toast windows, in seconds", 16, "4", nullptr, 4, 0, nullptr, 0);
    CameraRegisterCVars();
    CVar::Register("showNewbieTips", "Show beginner tooltips", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("UberTooltips", "Show verbose tooltips", 16, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showItemLevel", "Show item level in the tooltip", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showTutorials", "display tutorials", 16, "1", nullptr, 4, 0, nullptr, 0);
    s_cvCalendarShowWeeklyHolidays = CVar::Register("calendarShowWeeklyHolidays", "Whether weekly holidays should appear in the calendar", 32, "1", &SetCalendarFilterCVarCallback, 4, 0, nullptr, 0);
    s_cvCalendarShowDarkmoon = CVar::Register("calendarShowDarkmoon", "Whether Darkmoon Faire holidays should appear in the calendar", 32, "1", &SetCalendarFilterCVarCallback, 4, 0, nullptr, 0);
    s_cvCalendarShowBattlegrounds = CVar::Register("calendarShowBattlegrounds", "Whether Battleground holidays should appear in the calendar", 32, "0", &SetCalendarFilterCVarCallback, 4, 0, nullptr, 0);
    s_cvCalendarShowLockouts = CVar::Register("calendarShowLockouts", "Whether raid lockouts should appear in the calendar", 32, "1", &SetCalendarFilterCVarCallback, 4, 0, nullptr, 0);
    s_cvCalendarShowResets = CVar::Register("calendarShowResets", "Whether raid resets should appear in the calendar", 32, "0", &SetCalendarFilterCVarCallback, 4, 0, nullptr, 0);
    g_nameplateShowEnemiesCVar = CVar::Register("nameplateShowEnemies", nullptr, 32, "0", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowEnemyPetsCVar = CVar::Register("nameplateShowEnemyPets", nullptr, 32, "1", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowEnemyGuardiansCVar = CVar::Register("nameplateShowEnemyGuardians", nullptr, 32, "1", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowEnemyTotemsCVar = CVar::Register("nameplateShowEnemyTotems", nullptr, 32, "1", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowFriendsCVar = CVar::Register("nameplateShowFriends", nullptr, 32, "0", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowFriendlyPetsCVar = CVar::Register("nameplateShowFriendlyPets", nullptr, 32, "1", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowFriendlyGuardiansCVar = CVar::Register("nameplateShowFriendlyGuardians", nullptr, 32, "1", nullptr, 4, 0, nullptr, 0);
    g_nameplateShowFriendlyTotemsCVar = CVar::Register("nameplateShowFriendlyTotems", nullptr, 32, "1", nullptr, 4, 0, nullptr, 0);
    s_cvNameplateAllowOverlap = CVar::Register("nameplateAllowOverlap", "switches between overlapping nameplates or the (old) never overlapping version", 32, "1", nullptr, 4, 0, nullptr, 0);
    g_unitHighlightsCVar = CVar::Register("unitHighlights", "Whether the highlight circle around units should be displayed", 16, "1", &UnitHighlightsCVarCallback, 4, 0, nullptr, 0);
    g_enablePVPNotifyAFKCVar = CVar::Register("enablePVPNotifyAFK", "The ability to shutdown the AFK notification system", 16, "1", nullptr, 4, 0, nullptr, 0);
    char str[128];
    SStrPrintf(str, sizeof(str), "%d", 3);
    g_serviceTypeFilterCVar = CVar::Register("serviceTypeFilter", "Which trainer services to show", 16, str, nullptr, 4, 0, nullptr, 0);
    g_autojoinPartyVoiceCVar = CVar::Register("autojoinPartyVoice", "Automatically join the voice session in party/raid chat", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_autojoinBGVoiceCVar = CVar::Register("autojoinBGVoice", "Automatically join the voice session in battleground chat", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("PushToTalkSound", "Play a sound when voice recording activates and deactivates", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("combatLogOn", "Whether or not the combat log is shown", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showKeyring", "Whether or not the keyring is shown", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showBattlefieldMinimap", "Whether or not the battlefield minimap is shown", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("playerStatLeftDropdown", "The player stat selected in the left dropdown", 32, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("playerStatRightDropdown", "The player stat selected in the right dropdown", 32, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("talentFrameShown", "The talent UI has been shown", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("auctionDisplayOnCharacter", "Show auction items on the dress-up paperdoll", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("addFriendInfoShown", "The info for Add Friend has been shown", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("pendingInviteInfoShown", "The info for pending invites has been shown", 16, "0", nullptr, 4, 0, nullptr, 0);
    const char* militaryTimeDefault = "1";
    // size_t locale = GetLocaleIndex(<current locale>);
    // if (locale == 2 || locale == 5 || locale == 10) {
    //     militaryTimeDefault = "0";
    // }
    CVar::Register("timeMgrUseMilitaryTime", "Toggles the display of either 12 or 24 hour time", 16, militaryTimeDefault, nullptr, 4, 0, nullptr, 0);
    CVar::Register("timeMgrUseLocalTime", "Toggles the use of either the realm time or your system time", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("timeMgrAlarmTime", "The time manager's alarm time in minutes", 16, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("timeMgrAlarmMessage", "The time manager's alarm message", 16, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("timeMgrAlarmEnabled", "Toggles whether or not the time manager's alarm will go off", 16, "0", nullptr, 4, 0, nullptr, 0);
    g_combatLogRetentionTimeCVar = CVar::Register("combatLogRetentionTime", "The maximum duration in seconds to retain combat log entries", 16, "300", nullptr, 4, 0, nullptr, 0);
    g_currencyTokensUnused1CVar = CVar::Register("currencyTokensUnused1", "Currency token types marked as unused.", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_currencyTokensUnused2CVar = CVar::Register("currencyTokensUnused2", "Currency token types marked as unused.", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_currencyTokensBackpack1CVar = CVar::Register("currencyTokensBackpack1", "Currency token types shown on backpack.", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_currencyTokensBackpack2CVar = CVar::Register("currencyTokensBackpack2", "Currency token types shown on backpack.", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showTokenFrame", "The token UI has been shown", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("showTokenFrameHonor", "The token UI has shown Honor", 32, "0", nullptr, 4, 0, nullptr, 0);
    g_predictedHealthCVar = CVar::Register("predictedHealth", "Whether or not to use predicted health values in the UI", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_predictedPowerCVar = CVar::Register("predictedPower", "Whether or not to use predicted power values in the UI", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_threatWarningCVar = CVar::Register("threatWarning", "Whether or not to show threat warning UI (0 = off, 1 = in dungeons, 2 = in party/raid, 3 = always)", 32, "3", &CVarHandler_ThreatWarning, 4, 0, nullptr, 0);
    g_threatWorldTextCVar = CVar::Register("threatWorldText", "Whether or not to show threat floaters in combat", 16, "1", nullptr, 4, 0, nullptr, 0);
    s_cvThreatShowNumeric = CVar::Register("threatShowNumeric", "Whether or not to show numeric threat on the target and focus frames", 16, "0", nullptr, 4, 0, nullptr, 0);
    s_cvThreatPlaySounds = CVar::Register("threatPlaySounds", "Whether or not to sounds when certain threat transitions occur", 16, "1", nullptr, 4, 0, nullptr, 0);
    // ViolenceLevelsRegisterCVars();
    s_cvShowAllSpellRanks = CVar::Register("ShowAllSpellRanks", "show either all spell ranks, or only the highest rank", 16, "1", nullptr, 4, 0, nullptr, 0);
    g_ShowClassColorInNameplateCVar = CVar::Register("ShowClassColorInNameplate", "use this to display the class color in the nameplate health bar", 32, "0", &ShowClassColorInNameplateCallback, 4, 0, nullptr, 0);
    g_previewTalentsCVar = CVar::Register("previewTalents", "Toggles the ability to preview talents before spending talent points.", 32, "0", &SignalPreviewTalentPointsChanged, 4, 0, nullptr, 0);
    s_cvLfgSelectedRoles = CVar::Register("lfgSelectedRoles", "Stores what roles the player is willing to take on.", 288, "0", nullptr, 4, 0, nullptr, 0);
    s_cvLfdCollapsedHeaders = CVar::Register("lfdCollapsedHeaders", "Stores which LFD headers are collapsed.", 288, "", nullptr, 4, 0, nullptr, 0);
    s_cvLfdSelectedDungeons = CVar::Register("lfdSelectedDungeons", "Stores which LFD dungeons are selected.", 288, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("lastTalkedToGM", "Stores the last GM someone was talking to in case they reload the UI while the GM chat window is open.", 16, "", nullptr, 4, 0, nullptr, 0);
    CVar::Register("autoCompleteResortNamesOnRecency", "Shows people you recently spoke with higher up on the AutoComplete list.", 16, "1", &CVarHandler_autoCompleteResortNamesOnRecency, 4, 0, nullptr, 0);
    CVar::Register("autoCompleteWhenEditingFromCenter", "If you edit a name by inserting characters into the center, a smarter auto-complete will occur.", 16, "1", &CVarHandler_autoCompleteWhenEditingFromCenter, 4, 0, nullptr, 0);
    CVar::Register("autoCompleteUseContext", "The system will, for example, only show people in your guild when you are typing /gpromote. Names will also never be removed.", 16, "1", &CVarHandler_autoCompleteUseContext, 4, 0, nullptr, 0);
    CVar::Register("colorChatNamesByClass", "If enabled, the name of a player speaking in chat will be colored according to his class.", 16, "0", nullptr, 4, 0, nullptr, 0);
    s_cvAutoFilledMultiCastSlots = CVar::Register("autoFilledMultiCastSlots", "Bitfield that saves whether multi-cast slots have been automatically filled.", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("minimapTrackedInfo", "Stores the minimap tracking that was active last session.", 32, "", nullptr, 4, 0, nullptr, 0);
    g_questPOICVar = CVar::Register("questPOI", "If enabled, the quest POI system will be used.", 32, "1", nullptr, 4, 0, nullptr, 0);
    CVar::Register("miniWorldMap", "Whether or not the world map has been toggled to smaller size", 32, "0", nullptr, 4, 0, nullptr, 0);
    CVar::Register("dontShowEquipmentSetsOnItems", "Don't show which equipment sets an item is associated with", 16, "0", nullptr, 4, 0, nullptr, 0);
}
