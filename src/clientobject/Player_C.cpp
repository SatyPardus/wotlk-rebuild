#include "clientobject/Player_C.hpp"
#include "clientobject/Types.hpp"
#include "db/Db.hpp"
#include <storm/Error.hpp>
#include "clientobject/ObjectMgrClient.hpp"
#include <gameui/CGGameUI.hpp>

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
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer())
        this->PostInitActivePlayer();
    //else
    //    CGPlayer_C::UpdatePartyMemberState(this);
    //CGUnit_C::UpdatePetReaction(this);
    //CGUnit_C::OnMoveUpdate(this, a2, 1, 1);
}

// OFFSET: 0x6E7F50
void CGPlayer_C::PostInitActivePlayer() {
    //this->movementData.m_flags |= 0x200u;
    //CGPlayer_C::SetActiveMirrorHandlers(this);
    //CGPlayer_C__LoadVocalUISounds(LOBYTE(this->m_unit->UNIT_FIELD_BYTES_0), this->m_player->PLAYER_BYTES_3[0]);
    //NOP_0(this, 0, 0);
    //maybe_CGSpellBook__ClearSpells();
    //for (i = 0; i < dword_C9EB3C; ++i)
    //    CGPlayer_C::AddKnownSpell(this, *(dword_C9EB40 + 2 * i), *(dword_C9EB40 + 4 * i + 2), 0, 1);
    //CGSpellBook::UpdateSpells(1, 0, 1);
    //bn_CGSpellBook_UpdateCompanions();
    //CGClassTrainer::RefreshList();
    //if (!ClntObjMgrGetPlayerType()) {
    //    for (j = 0; j < 0x90; ++j) {
    //        if ((dword_AD9F6C[j] & 0xF0000000) != 0 || bn_CGUnit_C_IsSpellKnown(dword_AD9F6C[j]))
    //            maybe_CGActionBar__SetAction(j, dword_AD9F6C[j], 0, 1);
    //    }
    //    FrameScript::SignalEvent(176, "%d", 0);
    //}
    //if (CGUnit_C::CurrentShapeshiftForm_HasFlag_0x1(this)) {
    //    CGSpellBook::UpdateUsable();
    //    bn_CGSpellBook_UpdateSelection();
    //    bn_CGActionBar_UpdateShapeShiftBar();
    //    maybe_CGActionBar__UpdateBonusBar();
    //    FrameScript::SignalEvent(377, 0);
    //}
    //v27[0] = -1;
    //bn_CGWorldFrame_UpdateScreenEffect();
    //ClntObjMgrEnumVisibleObjects(bn_AuraVisionUpdateHandler, v27);
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //CGCamera::sub_6053D0(ActiveCamera, 0.0);
    //if (!ClntObjMgrGetPlayerType()) {
    //    bn_CGUnit_C_SetLocalClientControl(1);
    //    v5 = &this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low;
    //    v6 = *v5;
    //    v7 = v5[1];
    //    if (__PAIR64__(v7, v6) == ClntObjMgrGetActivePlayer()) {
    //        PlayerData = this->m_player;
    //        guid_low = PlayerData->PLAYER_FARSIGHT.guid_low;
    //        guid_high = PlayerData->PLAYER_FARSIGHT.guid_high;
    //        v22 = guid_low;
    //    } else {
    //        guid_high = 0;
    //        v22 = 0;
    //    }
    //    v23 = guid_high;
    //    v11 = this->m_unit;
    //    v12 = *v11;
    //    v13 = v11[1];
    //    if (*v11) {
    //        v14 = ClntObjMgrObjectPtr(__PAIR64__(v13, v12), TYPEMASK_UNIT);
    //        v26 = v14;
    //        if (v14 && (v14->m_unit->UNIT_FIELD_FLAGS & 0x1000000) != 0 && v22 == v12 && v23 == v13) {
    //            bn_CGUnit_C_SetLocalClientControl(1);
    //            v24 = v12;
    //            v25 = v13;
    //            goto LABEL_24;
    //        }
    //    } else {
    //        v26 = 0;
    //    }
    //    v15 = this->ObjectBase.m_obj;
    //    v24 = *v15;
    //    v25 = v15[1];
//LABEL_24:
        CGGameUI::InitClientControlState(this->m_obj->m_guid);
    //    v16 = this->ObjectBase.m_obj;
    //    if (v24 != v16->OBJECT_FIELD_GUID.guid_low || v25 != v16->OBJECT_FIELD_GUID.guid_high)
    //        maybe_CGPlayer_C__ToggleFarSight(this, v26);
    //    CGGameUI::EnterWorld();
    //    CGGameUI::UpdateActivePlayer();
    //}
    //Current = ClientServices::GetCurrent();
    //CNetClient::sub_6B1840(Current, 1);
    //if (dword_C9EAAC) {
    //    maybe_CGGameUI__StartCinematic(dword_C9EAAC);
    //    dword_C9EAAC = 0;
    //} else if ((this->ObjectBase.GetTransportGUID)(this)) {
    //    LoadingScreenSetTransparent(1);
    //} else {
    //    LoadingScreenDisable();
    //}
    //Spell_C_SetPlayerClass(BYTE1(this->m_unit->UNIT_FIELD_BYTES_0));
    //bn_CGPlayer_C_CountEquippedGems(this);
    //PLAYER_FLAGS = this->m_player->PLAYER_FLAGS;
    //if ((PLAYER_FLAGS & 0x200) != 0) {
    //    this->unk_1020[59] = 0;
    //} else if ((PLAYER_FLAGS & 0x40000) == 0) {
    //    this->unk_1020[59] = FrameTime::s_curTimeMs + 300000;
    //}
    //ClntObjMgrEnumVisibleObjects(bn_TrackingMaskUpdateProc, 0);
    //CGCommentator::PostInit(this);
    //if (SFile::IsStreamingMode()) {
    //    if ((this->ObjectBase.GetTransportGUID)(this)) {
    //        LoadingScreenDisable();
    //        v19 = (this->ObjectBase.GetPosition)(this);
    //        World::Preload(v19, v21);
    //    }
    //}
}

// OFFSET: 0x6DE980
bool CGPlayer_C::IsCommentatorUberOrInArena() {
    //if (CGGameUI::m_iCurrentMapID < g_MapDB.minIndex || CGGameUI::m_iCurrentMapID > g_MapDB.maxIndex)
    //    v1 = 0;
    //else
    //    v1 = g_MapDB.Rows[CGGameUI::m_iCurrentMapID - g_MapDB.minIndex];
    //v2 = *(this[1026] + 8);
    //return (v2 & 0x80000) != 0 && ((v2 & 0x400000) != 0 || v1 && *(v1 + 8) == 4);
    return false;
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
