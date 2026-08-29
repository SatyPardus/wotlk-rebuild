#include "clientobject/Player_C.hpp"
#include "clientobject/Types.hpp"
#include "db/Db.hpp"
#include <storm/Error.hpp>
#include "clientobject/ObjectMgrClient.hpp"

CGPlayer_C::CGPlayer_C() {

}

CGPlayer_C::CGPlayer_C(CClientObjCreate& objCreate, uint32_t time)
    : CGUnit_C(objCreate, time) {
    
}

// OFFSET: 0x6E8280
void CGPlayer_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //this->unk_1020[51] = LOBYTE(this->m_unit->UNIT_FIELD_BYTES_1);
    this->CGUnit_C::PostInit(time, objCreate, isUpdate3);
    //CGUnit_C::GetUnitName(this, 0, 1);
    //CGPlayer_C::OnGuildChanged(this, 0);
    //CGChat::UpdateGuildStatus();
    //(this->ObjectBase.Animate)(this, 0.0);
    //EquippedItemDisplayId = maybe_CGPlayer_C__GetEquippedItemDisplayId(this);
    //CGUnit_C::sub_7206A0(this, EquippedItemDisplayId, 30);
    //CGPartyInfo::EnableMember(this, 1);
    //CGRaidInfo::EnableMember(this, 1);
    //if (CGBattlefieldInfo::m_instanceType == 4 && this->m_player->PLAYER_BYTES_3[3] != CGBattlefieldInfo::m_arenaFaction) {
    //    OBJECT_FIELD_GUID = this->ObjectBase.m_obj->OBJECT_FIELD_GUID;
    //    CGBattlefieldInfo::AddArenaOpponent(&OBJECT_FIELD_GUID);
    //    m_obj = this->ObjectBase.m_obj;
    //    guid_high = m_obj->OBJECT_FIELD_GUID.guid_high;
    //    OBJECT_FIELD_GUID.guid_low = m_obj->OBJECT_FIELD_GUID.guid_low;
    //    OBJECT_FIELD_GUID.guid_high = guid_high;
    //    if (sub_6CF670(&OBJECT_FIELD_GUID, &a4))
    //        dword_BE9EE0[a4] = 1;
    //}
    //v8 = this->ObjectBase.m_obj;
    //guid_low = v8->OBJECT_FIELD_GUID.guid_low;
    //v10 = v8->OBJECT_FIELD_GUID.guid_high;
    //if (__PAIR64__(v10, guid_low) == ClntObjMgrGetActivePlayer())
    //    CGPlayer_C::PostInitActivePlayer(this);
    //else
    //    CGPlayer_C::UpdatePartyMemberState(this);
    //CGUnit_C::UpdatePetReaction(this);
    //CGUnit_C::OnMoveUpdate(this, a2, 1, 1);

    //#### TESTIN
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGUnit_C::s_activeMover = this->m_obj->m_guid;
    }
    //#####
}

const CreatureModelDataRec* Player_C_GetModelName(uint32_t race, uint32_t sex) {
    STORM_ASSERT(sex < UNITSEX_LAST);

    auto displayId = Player_C_GetDisplayId(race, sex);
    auto record = g_creatureDisplayInfoDB.GetRecord(displayId);
    if (!record) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, unknown displayInfo %d specified for player race %d sex %d!", displayId, race, sex);
    }

    auto modelData = g_creatureModelDataDB.GetRecord(record->m_modelID);
    if (!modelData) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, unknown model record %d specified for player race %d sex %d!", record->m_modelID, race, sex);
    }

    return modelData;
}

uint32_t Player_C_GetDisplayId(uint32_t race, uint32_t sex) {
    STORM_ASSERT(sex < UNITSEX_LAST);

    auto record = g_chrRacesDB.GetRecord(race);
    if (!record) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, race %d not found in race table!", race);
    }

    if (sex == UNITSEX_MALE) {
        return record->m_maleDisplayID;
    }

    if (sex == UNITSEX_FEMALE) {
        return record->m_femaleDisplayID;
    }

    if (sex == UNITSEX_NONE) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatal("Error, attempted to look up model for player with sex %d (UNITSEX_NONE), all players have sex! =D", 2);
    }

    SErrPrepareAppFatal(__FILE__, __LINE__);
    SErrDisplayAppFatal("Error, unrecognized sex code %d!", sex);
    return 0;
}

// OFFSET: 0x6D1CF0
void CGPlayer_C::SetStorage(CGPlayer_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGUnit_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_player = reinterpret_cast<CGPlayerData*>(descriptorPtr + CGUnit::GetDataSize());
    obj->m_playerMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGUnit::TotalFields());
}
