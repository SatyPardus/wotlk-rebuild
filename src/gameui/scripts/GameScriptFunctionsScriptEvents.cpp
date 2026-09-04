#include "gameui/GameScriptFunctions.hpp"
#include "ui/FrameScript.hpp"
#include "ui/ScriptFunctions.hpp"
#include "util/Lua.hpp"
#include "util/Unimplemented.hpp"
#include "client/ClientServices.hpp"
#include <common/Time.hpp>
#include <util/StringTo.hpp>
#include <db/StaticDb.hpp>
#include <util/Lang.hpp>

// OFFSET: 0x60C2A0
static int32_t Script_UnitExists(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C350
static int32_t Script_UnitIsVisible(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C3D0
static int32_t Script_UnitIsUnit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C4B0
static int32_t Script_UnitIsPlayer(lua_State* L) {
    // TODO
    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x60C550
static int32_t Script_UnitIsInMyGuild(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C6F0
static int32_t Script_UnitIsCorpse(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C770
static int32_t Script_UnitIsPartyLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C810
static int32_t Script_UnitGroupRolesAssigned(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C8A0
static int32_t Script_UnitIsRaidOfficer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C920
static int32_t Script_UnitInParty(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60C9A0
static int32_t Script_UnitPlayerOrPetInParty(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CA20
static int32_t Script_UnitInRaid(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CB20
static int32_t Script_UnitPlayerOrPetInRaid(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CBA0
static int32_t Script_UnitPlayerControlled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CC30
static int32_t Script_UnitIsAFK(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CD50
static int32_t Script_UnitIsDND(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CE20
static int32_t Script_UnitIsPVP(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CF20
static int32_t Script_UnitIsPVPSanctuary(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CFB0
static int32_t Script_UnitIsPVPFreeForAll(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D0A0
static int32_t Script_UnitFactionGroup(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D280
static int32_t Script_UnitReaction(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D330
static int32_t Script_UnitIsEnemy(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D3D0
static int32_t Script_UnitIsFriend(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D530
static int32_t Script_UnitCanCooperate(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D690
static int32_t Script_UnitCanAssist(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D730
static int32_t Script_UnitCanAttack(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D7D0
static int32_t Script_UnitIsCharmed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D860
static int32_t Script_UnitIsPossessed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D8F0
static int32_t Script_PlayerCanTeleport(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60D970
static int32_t Script_UnitClassification(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DA00
static int32_t Script_UnitSelectionColor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E630
static int32_t Script_UnitGUID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E740
static int32_t Script_UnitName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E9A0
static int32_t Script_UnitPVPName(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60EA60
static int32_t Script_UnitXP(lua_State* L) {
    // TODO
    lua_pushnumber(L, 50.0);
    return 1;
}

// OFFSET: 0x60EAE0
static int32_t Script_UnitXPMax(lua_State* L) {
    // TODO
    lua_pushnumber(L, 100.0);
    return 1;
}

// OFFSET: 0x60EB60
static int32_t Script_UnitHealth(lua_State* L) {
    // TODO
    lua_pushnumber(L, 50.0);
    return 1;
}

// OFFSET: 0x60EC60
static int32_t Script_UnitHealthMax(lua_State* L) {
    // TODO
    lua_pushnumber(L, 100.0);
    return 1;
}

// OFFSET: 0x60ED40
static int32_t Script_UnitMana(lua_State* L) {
    // TODO
    lua_pushnumber(L, 50.0);
    return 1;
}

// OFFSET: 0x60EF40
static int32_t Script_UnitManaMax(lua_State* L) {
    // TODO
    lua_pushnumber(L, 100.0);
    return 1;
}

// OFFSET: 0x60ED40
static int32_t Script_UnitPower(lua_State* L) {
    // TODO
    lua_pushnumber(L, 50.0);
    return 1;
}

// OFFSET: 0x60EF40
static int32_t Script_UnitPowerMax(lua_State* L) {
    // TODO
    lua_pushnumber(L, 100.0);
    return 1;
}

// OFFSET: 0x60F100
static int32_t Script_UnitPowerType(lua_State* L) {
    // TODO
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x60F350
static int32_t Script_UnitOnTaxi(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60F3D0
static int32_t Script_UnitIsFeignDeath(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60F480
static int32_t Script_UnitIsDead(lua_State* L) {
    // TODO
    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x60F580
static int32_t Script_UnitIsGhost(lua_State* L) {
    // TODO
    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x60F680
static int32_t Script_UnitIsDeadOrGhost(lua_State* L) {
    // TODO
    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x60F790
static int32_t Script_UnitIsConnected(lua_State* L) {
    // TODO
    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x60F860
static int32_t Script_UnitAffectingCombat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60F8E0
static int32_t Script_UnitSex(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60F9E0
static int32_t Script_UnitLevel(lua_State* L) {
    // TODO
    lua_pushnumber(L, 25.0);
    return 1;
}

// OFFSET: 0x60FBA0
static int32_t Script_GetMoney(lua_State* L) {
    // TODO
    lua_pushnumber(L, 10.0);
    return 1;
}

// OFFSET: 0x60FC40
static int32_t Script_GetHonorCurrency(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60FCC0
static int32_t Script_GetArenaCurrency(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60FD40
static int32_t Script_UnitRace(lua_State* L) {
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    WHOA_UNIMPLEMENTED(2);
}

// OFFSET: 0x60FEC0
static int32_t Script_UnitClass(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610040
static int32_t Script_UnitClassBase(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6101A0
static int32_t Script_UnitResistance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610300
static int32_t Script_UnitStat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610450
static int32_t Script_UnitAttackBothHands(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610860
static int32_t Script_UnitDamage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610550
static int32_t Script_UnitRangedDamage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6107D0
static int32_t Script_UnitRangedAttack(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610A00
static int32_t Script_UnitAttackSpeed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610B60
static int32_t Script_UnitAttackPower(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610CA0
static int32_t Script_UnitRangedAttackPower(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610DE0
static int32_t Script_UnitDefense(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610EC0
static int32_t Script_UnitArmor(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x610FB0
static int32_t Script_UnitCharacterPoints(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x614CA0
static int32_t Script_UnitBuff(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x614CF0
static int32_t Script_UnitDebuff(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x614D40
static int32_t Script_UnitAura(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611130
static int32_t Script_UnitIsTapped(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6111B0
static int32_t Script_UnitIsTappedByPlayer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611230
static int32_t Script_UnitIsTappedByAllThreatList(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6112B0
static int32_t Script_UnitIsTrivial(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611330
static int32_t Script_UnitHasRelicSlot(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6113E0
static int32_t Script_SetPortraitTexture(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611600
static int32_t Script_HasFullControl(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611670
static int32_t Script_GetComboPoints(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DB20
static int32_t Script_IsInGuild(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DB80
static int32_t Script_IsGuildLeader(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DC70
static int32_t Script_IsArenaTeamCaptain(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DBF0
static int32_t Script_IsInArenaTeam(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DD40
static int32_t Script_IsResting(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DDB0
static int32_t Script_GetCombatRating(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DE70
static int32_t Script_GetCombatRatingBonus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6082C0
static int32_t Script_GetMaxCombatRatingBonus(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DF30
static int32_t Script_GetDodgeChance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DF90
static int32_t Script_GetBlockChance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60DFF0
static int32_t Script_GetShieldBlock(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E070
static int32_t Script_GetParryChance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E130
static int32_t Script_GetCritChanceFromAgility(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E1B0
static int32_t Script_GetSpellCritChanceFromIntellect(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E0D0
static int32_t Script_GetCritChance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E230
static int32_t Script_GetRangedCritChance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E290
static int32_t Script_GetSpellCritChance(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E310
static int32_t Script_GetSpellBonusDamage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E3B0
static int32_t Script_GetSpellBonusHealing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E410
static int32_t Script_GetPetSpellBonusDamage(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E470
static int32_t Script_GetSpellPenetration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E4E0
static int32_t Script_GetArmorPenetration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60E560
static int32_t Script_GetAttackPowerForStat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611780
static int32_t Script_UnitCreatureType(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611820
static int32_t Script_UnitCreatureFamily(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6118C0
static int32_t Script_GetResSicknessDuration(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611A20
static int32_t Script_GetPVPSessionStats(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611AD0
static int32_t Script_GetPVPYesterdayStats(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611B80
static int32_t Script_GetPVPLifetimeStats(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611C40
static int32_t Script_UnitPVPRank(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611CB0
static int32_t Script_GetPVPRankInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x608560
static int32_t Script_GetPVPRankProgress(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x611DF0
static int32_t Script_UnitCastingInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612090
static int32_t Script_UnitChannelInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60A450
static int32_t Script_IsLoggedIn(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612260
static int32_t Script_IsFlyableArea(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612300
static int32_t Script_IsIndoors(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612360
static int32_t Script_IsOutdoors(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6123C0
static int32_t Script_IsOutOfBounds(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612430
static int32_t Script_IsFalling(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6124A0
static int32_t Script_IsSwimming(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612500
static int32_t Script_IsFlying(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6125A0
static int32_t Script_IsMounted(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612610
static int32_t Script_IsStealthed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612670
static int32_t Script_UnitIsSameServer(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6127F0
static int32_t Script_GetUnitHealthModifier(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612870
static int32_t Script_GetUnitMaxHealthModifier(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612900
static int32_t Script_GetUnitPowerModifier(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612980
static int32_t Script_GetUnitHealthRegenRateFromSpirit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612A00
static int32_t Script_GetUnitManaRegenRateFromSpirit(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612A90
static int32_t Script_GetManaRegen(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612B40
static int32_t Script_GetPowerRegen(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613020
static int32_t Script_GetRuneCooldown(lua_State* L) {
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    WHOA_UNIMPLEMENTED(3);
}

// OFFSET: 0x613140
static int32_t Script_GetRuneCount(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6131E0
static int32_t Script_GetRuneType(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612D50
static int32_t Script_ReportPlayerIsPVPAFK(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612E20
static int32_t Script_PlayerIsPVPInactive(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612BF0
static int32_t Script_GetExpertise(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612CB0
static int32_t Script_GetExpertisePercent(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60CAA0
static int32_t Script_UnitInBattleground(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x612F10
static int32_t Script_UnitInRange(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613290
static int32_t Script_GetUnitSpeed(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613330
static int32_t Script_GetUnitPitch(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6133D0
static int32_t Script_UnitInVehicle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6134A0
static int32_t Script_UnitUsingVehicle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613570
static int32_t Script_UnitControllingVehicle(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613700
static int32_t Script_UnitInVehicleControlSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613740
static int32_t Script_UnitHasVehicleUI(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613780
static int32_t Script_UnitTargetsVehicleInRaidUI(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6137D0
static int32_t Script_UnitVehicleSkin(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613830
static int32_t Script_UnitVehicleSeatCount(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6138C0
static int32_t Script_UnitVehicleSeatInfo(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x6139B0
static int32_t Script_UnitSwitchToVehicleSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x608580
static int32_t Script_CanSwitchVehicleSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x614E60
static int32_t Script_GetVehicleUIIndicator(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x614EF0
static int32_t Script_GetVehicleUIIndicatorSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613A60
static int32_t Script_UnitThreatSituation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613B40
static int32_t Script_UnitDetailedThreatSituation(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613C90
static int32_t Script_UnitIsControlling(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613E10
static int32_t Script_EjectPassengerFromSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613D20
static int32_t Script_CanEjectPassengerFromSeat(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613ED0
static int32_t Script_RespondInstanceLock(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60A490
static int32_t Script_GetPlayerFacing(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x613F90
static int32_t Script_GetPlayerInfoByGUID(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x608690
static int32_t Script_GetItemStats(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x608760
static int32_t Script_GetItemStatDelta(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x614140
static int32_t Script_IsXPUserDisabled(lua_State* L) {
    WHOA_UNIMPLEMENTED(0);
}

// OFFSET: 0x60A510
static int32_t Script_FillLocalizedClassList(lua_State* L) {
    if (lua_type(L, 1) != 5)
        luaL_error(L, "Usage: FillLocalizedClassList(classTable[, isFemale])");
    auto isFemale = StringToBOOL(L, 2, 0);
    lua_settop(L, 1);

    for (int32_t i = 0; i < g_chrClassesDB.GetNumRecords(); i++) {
        auto rec = g_chrClassesDB.GetRecordByIndex(i);
        if (rec) {
            lua_pushstring(L, rec->m_filename);
            lua_pushstring(L, GetClassGenderName(rec, isFemale, nullptr));
            lua_settable(L, -3);
        }
    }
    return 1;
}


void ScriptEventsRegisterFunctions() {
    // The client keeps one table for these and registers it from two places:
    // bn_SystemRegisterFunctions (0x60A120, glue) and this function
    // (ScriptEventsRegisterFunctions, 0x60A170, game). See FrameScript::s_ScriptFunctions_System.
    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_SYSTEM; ++i) {
        FrameScript_RegisterFunction(
            FrameScript::s_ScriptFunctions_System[i].name,
            FrameScript::s_ScriptFunctions_System[i].method);
    }

    for (int32_t i = 0; i < NUM_SCRIPT_FUNCTIONS_SCRIPT_EVENTS_UNIT; ++i) {
        FrameScript_RegisterFunction(
            GameScript::s_ScriptFunctions_ScriptEventsUnit[i].name,
            GameScript::s_ScriptFunctions_ScriptEventsUnit[i].method);
    }
}


FrameScript_Method GameScript::s_ScriptFunctions_ScriptEventsUnit[NUM_SCRIPT_FUNCTIONS_SCRIPT_EVENTS_UNIT] = {
    { "UnitExists", &Script_UnitExists },
    { "UnitIsVisible", &Script_UnitIsVisible },
    { "UnitIsUnit", &Script_UnitIsUnit },
    { "UnitIsPlayer", &Script_UnitIsPlayer },
    { "UnitIsInMyGuild", &Script_UnitIsInMyGuild },
    { "UnitIsCorpse", &Script_UnitIsCorpse },
    { "UnitIsPartyLeader", &Script_UnitIsPartyLeader },
    { "UnitGroupRolesAssigned", &Script_UnitGroupRolesAssigned },
    { "UnitIsRaidOfficer", &Script_UnitIsRaidOfficer },
    { "UnitInParty", &Script_UnitInParty },
    { "UnitPlayerOrPetInParty", &Script_UnitPlayerOrPetInParty },
    { "UnitInRaid", &Script_UnitInRaid },
    { "UnitPlayerOrPetInRaid", &Script_UnitPlayerOrPetInRaid },
    { "UnitPlayerControlled", &Script_UnitPlayerControlled },
    { "UnitIsAFK", &Script_UnitIsAFK },
    { "UnitIsDND", &Script_UnitIsDND },
    { "UnitIsPVP", &Script_UnitIsPVP },
    { "UnitIsPVPSanctuary", &Script_UnitIsPVPSanctuary },
    { "UnitIsPVPFreeForAll", &Script_UnitIsPVPFreeForAll },
    { "UnitFactionGroup", &Script_UnitFactionGroup },
    { "UnitReaction", &Script_UnitReaction },
    { "UnitIsEnemy", &Script_UnitIsEnemy },
    { "UnitIsFriend", &Script_UnitIsFriend },
    { "UnitCanCooperate", &Script_UnitCanCooperate },
    { "UnitCanAssist", &Script_UnitCanAssist },
    { "UnitCanAttack", &Script_UnitCanAttack },
    { "UnitIsCharmed", &Script_UnitIsCharmed },
    { "UnitIsPossessed", &Script_UnitIsPossessed },
    { "PlayerCanTeleport", &Script_PlayerCanTeleport },
    { "UnitClassification", &Script_UnitClassification },
    { "UnitSelectionColor", &Script_UnitSelectionColor },
    { "UnitGUID", &Script_UnitGUID },
    { "UnitName", &Script_UnitName },
    { "UnitPVPName", &Script_UnitPVPName },
    { "UnitXP", &Script_UnitXP },
    { "UnitXPMax", &Script_UnitXPMax },
    { "UnitHealth", &Script_UnitHealth },
    { "UnitHealthMax", &Script_UnitHealthMax },
    { "UnitMana", &Script_UnitMana },
    { "UnitManaMax", &Script_UnitManaMax },
    { "UnitPower", &Script_UnitPower },
    { "UnitPowerMax", &Script_UnitPowerMax },
    { "UnitPowerType", &Script_UnitPowerType },
    { "UnitOnTaxi", &Script_UnitOnTaxi },
    { "UnitIsFeignDeath", &Script_UnitIsFeignDeath },
    { "UnitIsDead", &Script_UnitIsDead },
    { "UnitIsGhost", &Script_UnitIsGhost },
    { "UnitIsDeadOrGhost", &Script_UnitIsDeadOrGhost },
    { "UnitIsConnected", &Script_UnitIsConnected },
    { "UnitAffectingCombat", &Script_UnitAffectingCombat },
    { "UnitSex", &Script_UnitSex },
    { "UnitLevel", &Script_UnitLevel },
    { "GetMoney", &Script_GetMoney },
    { "GetHonorCurrency", &Script_GetHonorCurrency },
    { "GetArenaCurrency", &Script_GetArenaCurrency },
    { "UnitRace", &Script_UnitRace },
    { "UnitClass", &Script_UnitClass },
    { "UnitClassBase", &Script_UnitClassBase },
    { "UnitResistance", &Script_UnitResistance },
    { "UnitStat", &Script_UnitStat },
    { "UnitAttackBothHands", &Script_UnitAttackBothHands },
    { "UnitDamage", &Script_UnitDamage },
    { "UnitRangedDamage", &Script_UnitRangedDamage },
    { "UnitRangedAttack", &Script_UnitRangedAttack },
    { "UnitAttackSpeed", &Script_UnitAttackSpeed },
    { "UnitAttackPower", &Script_UnitAttackPower },
    { "UnitRangedAttackPower", &Script_UnitRangedAttackPower },
    { "UnitDefense", &Script_UnitDefense },
    { "UnitArmor", &Script_UnitArmor },
    { "UnitCharacterPoints", &Script_UnitCharacterPoints },
    { "UnitBuff", &Script_UnitBuff },
    { "UnitDebuff", &Script_UnitDebuff },
    { "UnitAura", &Script_UnitAura },
    { "UnitIsTapped", &Script_UnitIsTapped },
    { "UnitIsTappedByPlayer", &Script_UnitIsTappedByPlayer },
    { "UnitIsTappedByAllThreatList", &Script_UnitIsTappedByAllThreatList },
    { "UnitIsTrivial", &Script_UnitIsTrivial },
    { "UnitHasRelicSlot", &Script_UnitHasRelicSlot },
    { "SetPortraitTexture", &Script_SetPortraitTexture },
    { "HasFullControl", &Script_HasFullControl },
    { "GetComboPoints", &Script_GetComboPoints },
    { "IsInGuild", &Script_IsInGuild },
    { "IsGuildLeader", &Script_IsGuildLeader },
    { "IsArenaTeamCaptain", &Script_IsArenaTeamCaptain },
    { "IsInArenaTeam", &Script_IsInArenaTeam },
    { "IsResting", &Script_IsResting },
    { "GetCombatRating", &Script_GetCombatRating },
    { "GetCombatRatingBonus", &Script_GetCombatRatingBonus },
    { "GetMaxCombatRatingBonus", &Script_GetMaxCombatRatingBonus },
    { "GetDodgeChance", &Script_GetDodgeChance },
    { "GetBlockChance", &Script_GetBlockChance },
    { "GetShieldBlock", &Script_GetShieldBlock },
    { "GetParryChance", &Script_GetParryChance },
    { "GetCritChanceFromAgility", &Script_GetCritChanceFromAgility },
    { "GetSpellCritChanceFromIntellect", &Script_GetSpellCritChanceFromIntellect },
    { "GetCritChance", &Script_GetCritChance },
    { "GetRangedCritChance", &Script_GetRangedCritChance },
    { "GetSpellCritChance", &Script_GetSpellCritChance },
    { "GetSpellBonusDamage", &Script_GetSpellBonusDamage },
    { "GetSpellBonusHealing", &Script_GetSpellBonusHealing },
    { "GetPetSpellBonusDamage", &Script_GetPetSpellBonusDamage },
    { "GetSpellPenetration", &Script_GetSpellPenetration },
    { "GetArmorPenetration", &Script_GetArmorPenetration },
    { "GetAttackPowerForStat", &Script_GetAttackPowerForStat },
    { "UnitCreatureType", &Script_UnitCreatureType },
    { "UnitCreatureFamily", &Script_UnitCreatureFamily },
    { "GetResSicknessDuration", &Script_GetResSicknessDuration },
    { "GetPVPSessionStats", &Script_GetPVPSessionStats },
    { "GetPVPYesterdayStats", &Script_GetPVPYesterdayStats },
    { "GetPVPLifetimeStats", &Script_GetPVPLifetimeStats },
    { "UnitPVPRank", &Script_UnitPVPRank },
    { "GetPVPRankInfo", &Script_GetPVPRankInfo },
    { "GetPVPRankProgress", &Script_GetPVPRankProgress },
    { "UnitCastingInfo", &Script_UnitCastingInfo },
    { "UnitChannelInfo", &Script_UnitChannelInfo },
    { "IsLoggedIn", &Script_IsLoggedIn },
    { "IsFlyableArea", &Script_IsFlyableArea },
    { "IsIndoors", &Script_IsIndoors },
    { "IsOutdoors", &Script_IsOutdoors },
    { "IsOutOfBounds", &Script_IsOutOfBounds },
    { "IsFalling", &Script_IsFalling },
    { "IsSwimming", &Script_IsSwimming },
    { "IsFlying", &Script_IsFlying },
    { "IsMounted", &Script_IsMounted },
    { "IsStealthed", &Script_IsStealthed },
    { "UnitIsSameServer", &Script_UnitIsSameServer },
    { "GetUnitHealthModifier", &Script_GetUnitHealthModifier },
    { "GetUnitMaxHealthModifier", &Script_GetUnitMaxHealthModifier },
    { "GetUnitPowerModifier", &Script_GetUnitPowerModifier },
    { "GetUnitHealthRegenRateFromSpirit", &Script_GetUnitHealthRegenRateFromSpirit },
    { "GetUnitManaRegenRateFromSpirit", &Script_GetUnitManaRegenRateFromSpirit },
    { "GetManaRegen", &Script_GetManaRegen },
    { "GetPowerRegen", &Script_GetPowerRegen },
    { "GetRuneCooldown", &Script_GetRuneCooldown },
    { "GetRuneCount", &Script_GetRuneCount },
    { "GetRuneType", &Script_GetRuneType },
    { "ReportPlayerIsPVPAFK", &Script_ReportPlayerIsPVPAFK },
    { "PlayerIsPVPInactive", &Script_PlayerIsPVPInactive },
    { "GetExpertise", &Script_GetExpertise },
    { "GetExpertisePercent", &Script_GetExpertisePercent },
    { "UnitInBattleground", &Script_UnitInBattleground },
    { "UnitInRange", &Script_UnitInRange },
    { "GetUnitSpeed", &Script_GetUnitSpeed },
    { "GetUnitPitch", &Script_GetUnitPitch },
    { "UnitInVehicle", &Script_UnitInVehicle },
    { "UnitUsingVehicle", &Script_UnitUsingVehicle },
    { "UnitControllingVehicle", &Script_UnitControllingVehicle },
    { "UnitInVehicleControlSeat", &Script_UnitInVehicleControlSeat },
    { "UnitHasVehicleUI", &Script_UnitHasVehicleUI },
    { "UnitTargetsVehicleInRaidUI", &Script_UnitTargetsVehicleInRaidUI },
    { "UnitVehicleSkin", &Script_UnitVehicleSkin },
    { "UnitVehicleSeatCount", &Script_UnitVehicleSeatCount },
    { "UnitVehicleSeatInfo", &Script_UnitVehicleSeatInfo },
    { "UnitSwitchToVehicleSeat", &Script_UnitSwitchToVehicleSeat },
    { "CanSwitchVehicleSeat", &Script_CanSwitchVehicleSeat },
    { "GetVehicleUIIndicator", &Script_GetVehicleUIIndicator },
    { "GetVehicleUIIndicatorSeat", &Script_GetVehicleUIIndicatorSeat },
    { "UnitThreatSituation", &Script_UnitThreatSituation },
    { "UnitDetailedThreatSituation", &Script_UnitDetailedThreatSituation },
    { "UnitIsControlling", &Script_UnitIsControlling },
    { "EjectPassengerFromSeat", &Script_EjectPassengerFromSeat },
    { "CanEjectPassengerFromSeat", &Script_CanEjectPassengerFromSeat },
    { "RespondInstanceLock", &Script_RespondInstanceLock },
    { "GetPlayerFacing", &Script_GetPlayerFacing },
    { "GetPlayerInfoByGUID", &Script_GetPlayerInfoByGUID },
    { "GetItemStats", &Script_GetItemStats },
    { "GetItemStatDelta", &Script_GetItemStatDelta },
    { "IsXPUserDisabled", &Script_IsXPUserDisabled },
    { "FillLocalizedClassList", &Script_FillLocalizedClassList }
};
