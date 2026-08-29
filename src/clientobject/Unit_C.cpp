#include "clientobject/Unit_C.hpp"

#include "db/Db.hpp"
#include "clientobject/Player_C.hpp"
#include <util/Byte.hpp>
#include "ObjectMgrClient.hpp"
#include <common/time/Time.hpp>

WGUID CGUnit_C::s_activeMover;

CGUnit_C::CGUnit_C() {

}

// OFFSET: 0x73F660
CGUnit_C::CGUnit_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    this->m_passenger = &this->movementData;
    //data0DC = this->data0DC;
    //this->ObjectBase.__vftable = off_A34D90;
    //v6 = 141;
    //p_m_terminator = &this->data0DC[0].m_terminator;
    //do {
    //    data0DC->m_linkoffset = 0;
    //    p_m_terminator->m_prevlink = p_m_terminator;
    //    p_m_terminator->m_next = (p_m_terminator | 1);
    //    ++data0DC;
    //    p_m_terminator = (p_m_terminator + 12);
    //    --v6;
    //} while (v6 >= 0);
    //m_obj = this->ObjectBase.m_obj;
    //*&this->unk_0784 = 0.0;

    new (&this->movementData) CMovement_C(&m_obj->m_guid, objCreate.m_moveUpdate.status.m_position, objCreate.m_moveUpdate.status.m_facing, &m_obj->m_guid, this);

    //this->m_creatureCacheEntry = nullptr;
    this->m_displayInfo = nullptr;
    this->m_displayInfoExtra = nullptr;
    this->m_modelData = nullptr;
    this->m_soundData = nullptr;
    //this->ukn = 0;
    this->m_bloodlevels = nullptr;
    //this->data980 = 0;
    //this->data984 = 0;
    //this->data988 = 0;
    //this->data98C = 0;
    //LOBYTE(this->data994) = 0;
    //this->data9C4 = 0;
    //this->data9C8 = 0;
    //this->data9CC = 0;
    //this->data9D0 = 0;
    this->m_displayId = 0;

    //this->dataA34[67] = 0;
    //this->dataA34[68] = 0;
    //this->dataA34[69] = 0;
    this->m_characterComponent = nullptr;
    //this->dataB50[2] = LOBYTE(m_unit->UNIT_FIELD_BYTES_2);
    //this->dataB50[3] = LOBYTE(m_unit->UNIT_FIELD_BYTES_2);

    this->unk_0A30 = 0x400000;
    this->SetClientInitData(objCreate, 0);
    //if (this->m_unit->UNIT_FIELD_HEALTH / this->m_unit->UNIT_FIELD_MAXHEALTH < 0.2f && this->m_unit->UNIT_FIELD_HEALTH > 0 && this->bloodlevels)
    //    this->unk_0A30 |= 2u;

    this->RefreshDataPointers();
    //if ((objCreate.flags & 1) != 0)
    //    bn_CGUnit_C_InitializeActivePlayerComponent(this);
}

// OFFSET: 0x73FCC0
void CGUnit_C::PostInit(uint32_t time, CClientObjCreate* objCreate, bool isUpdate3) {
    //CMovement::sub_6EA520(&this->movementData, a3);
    //*&this->data9E0[87] = bn_CGUnit_C_GetModelScale(this->m_unit->UNIT_FIELD_DISPLAYID);
    //if ((this->ObjectBase.m_obj->OBJECT_FIELD_TYPE & TYPEMASK_PLAYER) == 0)
    //    CGUnit_C::OnMoveUpdate(this, a2, 1, 1);
    //bn_CGUnit_C_UpdateSelectionRadius(this);
    //maybe_CGObject_C__UpdateEffectAttachments(this);
    this->CGObject_C::PostInit(time, objCreate, isUpdate3);
    //maybe_CGUnit_C__VehiclePassengerInit(this);
    //MovementUpdateCameraYaw(this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high, this->movementData.__base.transportGuid.guid_low, this->movementData.__base.transportGuid.guid_high);
    //m_worldModel = this->ObjectBase.m_worldModel;
    //CM2Model::SetSequenceCallback(m_worldModel, maybe_CGUnit_C__DispatchAnimEnd, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high);
    //CWorldScene::LoadModel(m_worldModel, COERCE_FLOAT(bn_AnimEventCallback_1), *&this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, *&this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high);
    //if (this->displayInfo) {
    //    bn_CCharacterComponent_ApplyMonsterGeosets(this->ObjectBase.m_worldModel, this->displayInfo);
    //    maybe_CCharacterComponent__ReplaceMonsterSkin(this->ObjectBase.m_worldModel, this->displayInfo, this->modelData);
    //    modelData = this->modelData;
    //    if (modelData)
    //        this->ObjectBase.m_worldModel->f_flags ^= (this->ObjectBase.m_worldModel->f_flags ^ (modelData->m_flags >> 7)) & 4;
    //}
    //this->data9C0 = this->m_unit->UNIT_FIELD_MOUNTDISPLAYID;
    //v8 = this->modelData;
    //this->data9E0[23] = v8->m_footprintTextureID;
    //*&this->data9E0[25] = v8->m_footprintTextureWidth * 0.027777778;
    //*&this->data9E0[26] = 0.027777778 * v8->m_footprintTextureLength;
    //*&this->data9E0[27] = v8->m_footprintParticleScale;
    //maybe_CGUnit_C__CheckLoopSound(this);
    //if ((*(v4 + 680) & 1) != 0 && (v9 = this->modelData) != 0 && (v9->m_flags & 4) != 0 && CGPlayer_C::s_displayId == this->m_unit->UNIT_FIELD_DISPLAYID && this->characterComponent) {
    //    this->data9E0[20] &= 0xFFBDFFFF;
    //    maybe_CGPlayer_C__RefreshVisibleItems(this);
    //} else {
    //    if (this->characterComponent) {
    //        CCharacterComponent::FreeComponent(this->characterComponent);
    //        v10 = this->data9E0[20] & 0xFFBDFFFF | 0x400000;
    //        this->characterComponent = 0;
    //        this->data9E0[20] = v10;
    //    }
    //    this->data9E0[20] = this->data9E0[20] & 0xFFBDFFFF | 0x400000;
    //}
    //CGUnit_C::UpdateUnitCollisionBox(1, 1);
    //UNIT_FIELD_BYTES_0_low = LOBYTE(this->m_unit->UNIT_FIELD_BYTES_0);
    //if (UNIT_FIELD_BYTES_0_low >= g_ChrRacesDB.minIndex && UNIT_FIELD_BYTES_0_low <= g_ChrRacesDB.maxIndex) {
    //    v12 = g_ChrRacesDB.Rows[UNIT_FIELD_BYTES_0_low - g_ChrRacesDB.minIndex];
    //    if (v12)
    //        this->data8D0[8] = *(v12 + 40);
    //}
    //v40 = (this->ObjectBase.GetRawFacing)(this);
    //*&this->data9E0[48] = CMath::normalizeangle0to2pi_(v40);
    //*&this->data9E0[49] = 0.0;
    //this->data9E0[50] = 0;
    //this->data9E0[51] = 0;
    //this->data9E0[52] = 0;
    //this->data9E0[53] = 0;
    //(this->ObjectBase.Animate)(this, 0.0);
    //m_unit = this->m_unit;
    //if (m_unit->UNIT_FIELD_HEALTH > 0) {
    //    if (m_unit->UNIT_FIELD_MOUNTDISPLAYID > 0)
    //        CGUnit_C::CreateUnitMount(1, 1);
    //} else {
    //    this->data9E0[56] = a2;
    //    CGObject_C::ClearEffectList(this, 0);
    //    maybe_CGUnit_C__CreateOrReuseSpellVisualEffect(this);
    //}
    //if (!ClntObjMgrGetPlayerType() && World::QueryGroundType(this->ObjectBase.m_worldObject, &a3))
    //    this->data9E0[24] = a3;
    //maybe_CGUnit_C__UpdateScriptRegistration(this);
    //if (this->ObjectBase.ukn_00B0)
    //    PlayerNameDelete(this->ObjectBase.ukn_00B0);
    //m_obj = this->ObjectBase.m_obj;
    //guid_low = m_obj->OBJECT_FIELD_GUID.guid_low;
    //guid_high = m_obj->OBJECT_FIELD_GUID.guid_high;
    //this->ObjectBase.ukn_00B0 = PlayerNameCreate(&guid_low);
    //maybe_CGUnit_C__UpdateBreathState(this, FrameTime::s_curTimeMs);
    //CGUnit_C::UpdateChannelEffects(this);
    //if (!(this->ObjectBase.ukn57)(this)) {
    //    v15 = BYTE2(this->ObjectBase.ukn_00C8);
    //    this->ObjectBase.ukn_00C4 = 0;
    //    LOBYTE(this->ObjectBase.ukn_00C8) = v15;
    //}
    //UNIT_FIELD_BYTES_1_high = HIBYTE(this->m_unit->UNIT_FIELD_BYTES_1);
    //this->dataB50[12] = UNIT_FIELD_BYTES_1_high;
    //CMovement_C::UpdateHoverState(&this->movementData.__base.unk_0000, UNIT_FIELD_BYTES_1_high == 2, 0);
    //this->movementData.__base.ukn34 = this->m_unit->UNIT_FIELD_HOVERHEIGHT;
    //maybe_CGUnit_C__CreateOrDestroyObjectEffectManager(this);
    //if (a4 && (this->ObjectBase.m_obj->OBJECT_FIELD_TYPE & 0x10) == 0)
    //    this->data9E0[6] = 10;
    //CGUnit_C::sub_73AC30(this, 0, -1);
    //v17 = this->ObjectBase.m_worldModel;
    //if (v17 && CM2Model::IsLoaded(v17, 0, 0)) {
    //    if (this->dataB50[13] == -1 || (BoneSequenceId = bn_CM2Model_GetBoneSequenceId(this->dataB50[13]), BoneSequenceId == -1))
    //        BoneSequenceId = bn_CM2Model_GetBoneSequenceId(-1);
    //} else {
    //    BoneSequenceId = -1;
    //}
    //if (BoneSequenceId >= g_AnimationDataDB.minIndex && BoneSequenceId <= g_AnimationDataDB.maxIndex) {
    //    v19 = g_AnimationDataDB.Rows[BoneSequenceId - g_AnimationDataDB.minIndex];
    //    if (v19) {
    //        if (v19->m_BehaviorID == 127) {
    //            v20 = BYTE2(this->ObjectBase.ukn_00C8);
    //            this->ObjectBase.ukn_00C4 = 0;
    //            LOBYTE(this->ObjectBase.ukn_00C8) = v20;
    //        }
    //    }
    //}
    //v21 = this->m_unit;
    //v22 = v21->UNIT_FIELD_CHARMEDBY.guid_low;
    //p_UNIT_FIELD_CHARMEDBY = &v21->UNIT_FIELD_CHARMEDBY;
    //if (p_UNIT_FIELD_CHARMEDBY->guid_high | v22)
    //    Script_SendUnitSignal(p_UNIT_FIELD_CHARMEDBY, 0);
    //v24 = this->m_unit;
    //v25 = v24->UNIT_FIELD_SUMMONEDBY.guid_low;
    //p_UNIT_FIELD_SUMMONEDBY = &v24->UNIT_FIELD_SUMMONEDBY;
    //if (p_UNIT_FIELD_SUMMONEDBY->guid_high | v25)
    //    Script_SendUnitSignal(p_UNIT_FIELD_SUMMONEDBY, 2);
    //bn_CGUnit_C_UpdatePartyMemberPetState(this);
    //TotemInfo_0 = bn_CGGameUI_GetTotemInfo_0(this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high);
    //if (TotemInfo_0) {
    //    TotemInfo_0[4] = CGUnit_C::GetUnitName(this, 0, 1);
    //    FrameScript::SignalEvent(EVENT_PLAYER_TOTEM_UPDATE, "%d", *TotemInfo_0 + 1);
    //}
    //bn_CMovement_C_SnapToGroundIfCloseEnough(&this->movementData);
    //v28 = this->objectclass1[23];
    //if (v28)
    //    v29 = *(v28 + 12);
    //else
    //    v29 = 0;
    //if (v29 && (*(v29 + 4) & 0x10000000) != 0) {
    //    v30 = this->ObjectBase.m_obj;
    //    guid_low = v30->OBJECT_FIELD_GUID.guid_low;
    //    guid_high = v30->OBJECT_FIELD_GUID.guid_high;
    //    bn_CGBattlefieldInfo_AddVehicle(&guid_low);
    //}
    this->unk_0A30 |= 0x80000u;
    //if (bnl_CGUnit_C__s_deferredClientControlUpdateGUID == *&this->ObjectBase.m_obj->OBJECT_FIELD_GUID) {
    //    maybe_CGUnit_C__ExecuteClientControlUpdate(bnl_CGUnit_C__s_deferredClientControlUpdateGUID, SHIDWORD(bnl_CGUnit_C__s_deferredClientControlUpdateGUID), bnl_CGUnit_C__s_deferredClientControlUpdateState);
    //    bnl_CGUnit_C__s_deferredClientControlUpdateGUID = 0i64;
    //}
    //bn_CMissileCollision_MaybeAddUnitToSystem(this);
    //ActivePlayer = ClntObjMgrGetActivePlayer();
    //v32 = ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    //if (CGBattlefieldInfo::m_instanceType == 4 && this->m_unit->UNIT_FIELD_PETNUMBER && v32 && CGUnit_C::UnitReaction(v32, this) <= 1) {
    //    v33 = this->ObjectBase.m_obj;
    //    v34 = this->m_unit;
    //    guid_low = v33->OBJECT_FIELD_GUID.guid_low;
    //    guid_high = v33->OBJECT_FIELD_GUID.guid_high;
    //    p_UNIT_FIELD_CREATEDBY = &v34->UNIT_FIELD_CHARMEDBY;
    //    if (!*&v34->UNIT_FIELD_CHARMEDBY)
    //        p_UNIT_FIELD_CREATEDBY = &v34->UNIT_FIELD_CREATEDBY;
    //    bn_CGBattlefieldInfo_AddArenaOpponentPet(&guid_low, p_UNIT_FIELD_CREATEDBY);
    //}
    //v36 = this->objectclass1[23];
    //if (v36 && *(v36 + 12)) {
    //    v37 = this->ObjectBase.m_obj;
    //    v38 = v37->OBJECT_FIELD_GUID.guid_low;
    //    v39 = v37->OBJECT_FIELD_GUID.guid_high;
    //    if (__PAIR64__(v39, v38) == ClntObjMgrGetActivePlayer())
    //        bn_CGUnit_C_SignalPlayerGainsVehicleDataEvent(this);
    //}
}

// OFFSET: 0x73C260
void CGUnit_C::SetClientInitData(CClientObjCreate& objCreate, bool a3) {
    //Combat::SetClientInitData(&this->data9E0[16], a2);
    //if (SLOBYTE(a2->flags) < 0)
    //    CGUnit_C::CreateVehicleData(this, a2, a2->m_vehicleId);
    if (!a3) {
        this->movementData.SetUpdateInfo(OsGetAsyncTimeMs(), &objCreate.m_moveUpdate, objCreate.flags & 1);
        //if ((this->movementData.m_flags & 0x2000) != 0)
        //    CGUnit_C::OnCollideFalling(this);
        //if ((a2->flags & 1) != 0) {
        //    v6 = this->ObjectBase.__vftable;
        //    this->data9E0[20] |= 0x80u;
        //    v7 = bnl_World__s_weather;
        //    v8 = (v6->GetPosition)(this, v9);
        //    v7->unk_000C[92] = *v8;
        //    v7->unk_000C[93] = v8[1];
        //    v7->unk_000C[94] = v8[2];
        //}
        //if ((a2->flags & 0x400) != 0)
        //    this->data9E0[20] |= 0x40000000u;
    }
}

// OFFSET: 0x730100
bool CGUnit_C::InitializeComponent() {
    if (!this->m_worldModel || !this->m_worldModel->IsLoaded(0, 0))
        return false;

    this->unk_0A30 &= ~0x400000u;
    if (this->m_characterComponent) {
        CCharacterComponent::FreeComponent(this->m_characterComponent);
        this->m_characterComponent = nullptr;
    }

    if ((this->m_unit->UNIT_FIELD_FLAGS_2 & 0x10) != 0) {
        //if ((this->unk_0A30 & 0x20000) == 0)
        //    CGUnit_C::RequestMirrorImageData(this);
        this->unk_0A30 |= 0x400000u;
        return 0;
    }

    if (this->sub_71A430()) {
        this->InitializeExtendedDisplay(reinterpret_cast<CGPlayer_C*>(this), 1);
    } else if (this->m_displayInfoExtra) {
        if (!this->InitializeExtendedDisplay(nullptr, 1))
            return 0;
    } else if ((this->m_obj->m_type & TYPEMASK_PLAYER) != 0 && (this->m_modelData->m_flags & 4) != 0) {
        this->InitializeExtendedDisplay(reinterpret_cast<CGPlayer_C*>(this), 0);
    }
    //if ((this->ObjectBase.m_obj->OBJECT_FIELD_TYPE & TYPEMASK_PLAYER) == 0 || !maybe_CGPlayer_C__RefreshVisibleItems(this)) {
    //    if ((!bn_CGPlayer_C_IsXRayVisionActive() || !CGUnit_C::sub_71C500(this)) && this->characterComponent && this->m_displayInfoExtra) {
    //        for (i = 32; i < 0x4C; i += 4) {
    //            v7 = *(&this->m_displayInfoExtra->m_ID + i);
    //            if (v7)
    //                CCharacterComponent::AddItem(this->characterComponent, v3, v7, 0);
    //            ++v3;
    //        }
    //    }
    //    maybe_CGUnit_C__AddHandItem(this, 0);
    //    maybe_CGUnit_C__AddHandItem(this, 1);
    //    maybe_CGUnit_C__AddHandItem(this, 2);
    //}
    //bn_CGUnit_C_ApplyComponentItemsFromEffects(this);
    //m_obj = this->ObjectBase.m_obj;
    //v9[0] = m_obj->OBJECT_FIELD_GUID.guid_low;
    //v9[1] = m_obj->OBJECT_FIELD_GUID.guid_high;
    //CGGameUI::UnitModelUpdate(v9, 3);
    return 1;
}

// OFFSET: 0x71D010
bool CGUnit_C::InitializeExtendedDisplay(CGPlayer_C* player, bool hasExtendedData) {
    this->m_characterComponent = CCharacterComponent::AllocComponent();
    ComponentData data = ComponentData();

    uint8_t sexId = 0;
    if (hasExtendedData) {
        data.m_preferences.raceID = m_displayInfoExtra->m_displayRaceID;
        sexId = m_displayInfoExtra->m_displaySexID;
    } else {
        data.m_preferences.raceID = LOBYTE(m_unit->UNIT_FIELD_BYTES_0);
        sexId = BYTE2(m_unit->UNIT_FIELD_BYTES_0);
    }

    data.m_preferences.sexID = sexId;
    data.m_preferences.classID = BYTE1(m_unit->UNIT_FIELD_BYTES_0);
    if (player) {
        data.m_preferences.skinID = player->m_player->PLAYER_BYTES[0];
        data.m_preferences.faceID = player->m_player->PLAYER_BYTES[1];
        data.m_preferences.hairStyleID = player->m_player->PLAYER_BYTES[2];
        data.m_preferences.hairColorID = player->m_player->PLAYER_BYTES[3];
        data.m_preferences.facialHairStyleID = player->m_player->PLAYER_BYTES_2[0];
    } else {
        data.m_preferences.skinID = this->m_displayInfoExtra->m_skinID;
        data.m_preferences.faceID = this->m_displayInfoExtra->m_faceID;
        data.m_preferences.hairStyleID = this->m_displayInfoExtra->m_hairStyleID;
        data.m_preferences.hairColorID = this->m_displayInfoExtra->m_hairColorID;
        data.m_preferences.facialHairStyleID = this->m_displayInfoExtra->m_facialHairID;
        if (/*!bn_CGPlayer_C_IsXRayVisionActive() ||*/ !this ->sub_71C500()) {
            if (!this->m_displayInfoExtra)
                return 0;
            auto bakeName = this->m_displayInfoExtra->m_bakeName;
            if (!bakeName || !*bakeName)
                return 0;
            data.m_flags |= 1u;
            SStrPrintf(data.m_npcSkinTexture, 0x104u, "%s%s", "Textures\\BakedNpcTextures\\", bakeName);
        }
    }
    data.m_model = this->m_worldModel;
    data.m_flags ^= (LOBYTE(data.m_flags) ^ (2 * (m_obj->m_guid == ClntObjMgrGetActivePlayer()))) & 2;
    ++data.m_model->m_refCount;
    if (player)
        CCharacterComponent::ValidateComponentData(&data, CONTEXT_1);
    else
        CCharacterComponent::ValidateComponentData(&data, CONTEXT_2);
    this->m_characterComponent->Init(&data, 0);
    return 1;
}

// OFFSET: 0x72D940
void CGUnit_C::RefreshDataPointers() {
    uint32_t displayId = this->m_displayId;
    if (!this->m_displayId || this->m_unit->UNIT_FIELD_NATIVEDISPLAYID != this->m_unit->UNIT_FIELD_DISPLAYID)
        displayId = this->m_unit->UNIT_FIELD_DISPLAYID;
    this->m_displayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);
    if (!this->m_displayInfo) {
        //UnitName = CGUnit_C::GetUnitName(this, 0, 1);
        //SysMsgPrintf_0(2, 2, "NOUNITDISPLAYID|%d|%s", m_displayId, UnitName);
        this->m_displayInfo = g_creatureDisplayInfoDB.GetRecordByIndex(0);
        //if (!this->m_displayInfo)
        //    NOP("Error, NO creature display records found");
    }
    this->m_displayInfoExtra = g_creatureDisplayInfoExtraDB.GetRecord(this->m_displayInfo->m_extendedDisplayInfoID);
    this->m_modelData = g_creatureModelDataDB.GetRecord(this->m_displayInfo->m_modelID);
    this->m_soundData = g_creatureSoundDataDB.GetRecord(this->m_displayInfo->m_soundID);
    if (!this->m_soundData) {
        this->m_soundData = g_creatureSoundDataDB.GetRecord(this->m_modelData->m_soundID);
    }

    this->m_bloodlevels = g_unitBloodLevelsDB.GetRecord(this->m_displayInfo->m_bloodID);
    if (!this->m_bloodlevels) {
        this->m_bloodlevels = g_unitBloodLevelsDB.GetRecord(this->m_modelData->m_bloodID);
        if (!this->m_bloodlevels)
            this->m_bloodlevels = g_unitBloodLevelsDB.GetRecordByIndex(0);
    }

    if (this->m_obj->m_type == HIER_TYPE_UNIT) {
        //v21[0] = m_obj->OBJECT_FIELD_GUID.guid_low;
        //v21[1] = m_obj->OBJECT_FIELD_GUID.guid_high;
        //this->m_creatureCacheEntry = DbCreatureCache_GetInfoBlockById(WDB_CACHE_CREATURE, m_obj->OBJECT_FIELD_ENTRY, v21, bn_CreatureQueryCallback, 0, 0);
    }

    if (this->m_unit->UNIT_FIELD_NATIVEDISPLAYID == this->m_unit->UNIT_FIELD_DISPLAYID)
        this->unk_0A30 |= 0x100u;
    else
        this->unk_0A30 &= ~0x100u;
    //if ((this->m_modelData->m_flags & 8) != 0)
    //    this->dataA34[1] |= 0x20000u;
    //else
    //    this->dataA34[1] &= ~0x20000u;
    if ((this->m_modelData->m_flags & 0x40) != 0)
        this->unk_0A30 |= 0x2000000u;
    else
        this->unk_0A30 &= ~0x2000000u;
}

// OFFSET: 0x71A430
bool CGUnit_C::sub_71A430() {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) != 0) {
        if (this->m_modelData) {
            if ((this->m_modelData->m_flags & 4) != 0) {
                if (this->m_displayInfoExtra) {
                    if ((this->m_displayInfoExtra->m_flags & 1) != 0)
                        return true;
                }
            }
        }
    }
    return false;
}

// OFFSET: 0x71C500
bool CGUnit_C::sub_71C500() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        if ((this->m_obj->m_type & TYPE_PLAYER) != 0) {
            if (this->m_modelData) {
                if ((this->m_modelData->m_flags & 4) != 0) {
                    if (this->m_displayInfoExtra) {
                        if ((this->m_displayInfoExtra->m_flags & 1) != 0)
                            return 1;
                    }
                }
            }
        }
        if (!this->m_displayInfoExtra && (this->m_obj->m_type & TYPE_PLAYER) != 0 && (this->m_modelData->m_flags & 4) != 0)
            return 1;
    }
    return 0;
}

// OFFSET: 0x73AB20
void CGUnit_C::OnMoveUpdate(int32_t time, bool a3, bool a4) {
    //v5 = this->objectclass1[23];
    //if (v5 && *(v5 + 12))
    //    CVehicle_C::UpdateWorldMatrix(v5);
    this->UpdateWorldObject(0);
    //m_worldObject = this->ObjectBase.m_worldObject;
    //if (!m_worldObject || !World::QueryGroundType(m_worldObject, &this->dataA34[3]))
    //    this->dataA34[3] = -1;
    //CGUnit_C::UpdateFlightStatus(this, a2);
    //CGUnit_C::UpdateSwimmingStatus(&this->ObjectBase, a2, a3);
}

// OFFSET: 0x72E5D0
void CGUnit_C::OnMoveStartLocal(int32_t eventTime, bool forward) {
    this->OnMovementInitiated();
    this->movementData.OnMoveStartLocal(eventTime, forward);
}

// OFFSET: 0x71AE10
void CGUnit_C::OnMoveStopLocal(int32_t eventTime) {
    this->movementData.OnMoveStopLocal(eventTime);
}

// OFFSET: 0x72E680
void CGUnit_C::OnStrafeStartLocal(int32_t eventTime, bool left) {
    this->OnMovementInitiated();
    this->movementData.OnStrafeStartLocal(eventTime, left);
}

// OFFSET: 0x71AE20
void CGUnit_C::OnStrafeStopLocal(int32_t eventTime) {
    this->movementData.OnStrafeStopLocal(eventTime);
}

// OFFSET: 0x72E730
void CGUnit_C::OnAscendDescendStartLocal(int32_t eventTime, bool up) {
    this->OnMovementInitiated();
    this->movementData.OnAscendDescendStartLocal(eventTime, up);
}

// OFFSET: 0x71AE30
void CGUnit_C::OnAscendDescendStopLocal(int32_t eventTime) {
    this->movementData.OnAscendDescendStopLocal(eventTime);
}

// OFFSET: 0x72E900
void CGUnit_C::OnPitchStartLocal(int32_t eventTime, bool up) {
    this->OnMovementInitiated();
    this->movementData.OnPitchStartLocal(eventTime, up);
}

// OFFSET: 0x72E9B0
void CGUnit_C::OnPitchStopLocal(int32_t eventTime) {
    this->OnMovementInitiated();
    this->movementData.OnPitchStopLocal(eventTime);
}

// OFFSET: 0x72E7E0
void CGUnit_C::OnTurnStartLocal(int32_t eventTime, bool left) {
    //WowClientDB::GetRow(v9);
    //if (ClientDb::GetLocalizedRow(&g_spellDB, this->m_unit->UNIT_CHANNEL_SPELL, v9) && (v11 & 0x10) != 0 && (v10 & 0x4000) != 0 && CGUnit_C::IsAutoTracking(this))
    //    Spell_C_CancelChannelSpell(this->m_unit->UNIT_CHANNEL_SPELL);

    this->OnMovementInitiated();
    this->movementData.OnTurnStartLocal(eventTime, left);
}

// OFFSET: 0x71AE40
void CGUnit_C::OnTurnStopLocal(int32_t eventTime) {
    this->movementData.OnTurnStopLocal(eventTime);
}

// OFFSET: none (inlined)
void CGUnit_C::OnMovementInitiated() {
    //m_obj = this->ObjectBase.m_obj;
    //if (m_obj->OBJECT_FIELD_GUID.guid_low == CGUnit_C::m_activeMover) {
    //    guid_high = m_obj->OBJECT_FIELD_GUID.guid_high;
    //    if (guid_high == HIDWORD(CGUnit_C::m_activeMover) && dword_CA11F4 != 13 && (dword_CA1200 & 1) == 0)
    //        CGUnit_C::ClearTrackingTarget(this, guid_high, 0, 1);
    //}
    //if (*&this->ObjectBase.m_obj->OBJECT_FIELD_GUID == CGUnit_C::m_activeMover) {
    //    ActivePlayer = ClntObjMgrGetActivePlayer();
    //    v7 = ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER);
    //    if (v7) {
    //        if (CGUnit_C::IsLooting(v7))
    //            CGGameUI::CloseLoot(1, 1, 0);
    //    }
    //}
}

// OFFSET: 0x74B9A0
bool CGUnit_C::NoStrafe() {
    return this->movementData.m_flags2 & MOVEMENTFLAG2_NO_STRAFE;
}

// OFFSET: 0x717A20
CreatureModelDataRec* CGUnit_C::GetModelData() {
    uint32_t displayId = 0; //this->m_displayId;
    if (!displayId || this->m_unit->UNIT_FIELD_NATIVEDISPLAYID != this->m_unit->UNIT_FIELD_DISPLAYID)
        displayId = this->m_unit->UNIT_FIELD_DISPLAYID;

    CreatureDisplayInfoRec* creatureDisplayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);
    if (!creatureDisplayInfo) {
        //SysMsgPrintf_0(1, 2, "NOCREATUREDISPLAYIDFOUND|%d", displayId);
        return nullptr;
    }

    CreatureModelDataRec* creatureModelData = g_creatureModelDataDB.GetRecord(creatureDisplayInfo->m_modelID);
    if (!creatureModelData) {
        //SysMsgPrintf_0(1, 16, "INVALIDDISPLAYMODELRECORD|%d|%d", creatureDisplayInfo->m_modelID, creatureDisplayInfo->m_ID);
        return nullptr;
    }

    return creatureModelData;
}

// OFFSET: 0x6E6EF0
void CGUnit_C::GetPosition(C3Vector& pos) {
    this->m_passenger->GetPosition(&pos, &this->m_passenger->m_position);
}

// OFFSET: 0x6E6F40
float CGUnit_C::GetFacing() {
    return this->m_passenger->GetFacing(this->m_passenger->m_facing);
}

// OFFSET: 0x717B20
bool CGUnit_C::GetModelFileName(const char** fileName) {
    CreatureModelDataRec* creatureModelData = this->GetModelData();
    if (!creatureModelData) {
        *fileName = "Spells\\ErrorCube.mdx";
        return true;
    }

    *fileName = creatureModelData->m_modelName;
    return creatureModelData->m_modelName;
}

// OFFSET: 0x730F30
void CGUnit_C::ShouldRender(uint32_t flags, uint32_t* culled, uint32_t* out) {
    this->CGObject_C::ShouldRender(flags, culled, out);

    if ((this->unk_0A30 & 0x400000) != 0 && !this->InitializeComponent()) {
        *out = 1;
        // goto LABEL_24;
    }

    if (*culled || *out) {
        if (this->m_characterComponent)
            this->m_characterComponent->Prep();
    } else {
        if (this->m_characterComponent && !this->m_characterComponent->RenderPrep(0)) {
            *out = 1;
            // goto LABEL_24;
        }
    //    m_worldModel = this->ObjectBase.m_worldModel;
    //    if (m_worldModel && (m_worldModel->f_flags & 0x4000) != 0) {
    //        m_attachList = m_worldModel->m_attachList;
    //        if (!m_attachList) {
//LABEL_17:
    //            *out = 1;
    //            goto LABEL_24;
    //        }
    //        while (1) {
    //            f_flags = m_attachList->f_flags;
    //            v9 = m_attachList->m_attachParent ? f_flags >> 7 : f_flags >> 3;
    //            if ((v9 & 1) != 0)
    //                break;
    //            m_attachList = m_attachList->m_attachNext;
    //            if (!m_attachList)
    //                goto LABEL_17;
    //        }
    //    }
    //    v10 = this->objectclass1[24];
    //    if (v10 && *(v10 + 20) && (*(v10 + 16) & 0x400) != 0)
    //        *out = 1;
    //}
//LABEL_24:
    //if (this->data98C) {
    //    v12 = !*culled && !*out;
    //    v13 = this->ObjectBase.m_worldModel;
    //    m_attachParent = v13->m_attachParent;
    //    v15 = v13->f_flags;
    //    v16 = v12;
    //    if (m_attachParent) {
    //        v17 = v16 << 7;
    //        v18 = v15 & 0xFFFFFF7F;
    //    } else {
    //        v17 = 8 * v16;
    //        v18 = v15 & 0xFFFFFFF7;
    //    }
    //    v19 = v18 | v17;
    //    v13->f_flags = v19;
    //    if (m_attachParent)
    //        v13->f_flags = v19 & 0xFFFDFFFF | (v16 << 17);
    //    else
    //        v13->f_flags = v19 & 0xFFFEFFFF | (v16 << 16);
    //    *out = 0;
    }
}

const char* CGUnit_C::GetDisplayRaceNameFromRecord(ChrRacesRec* record, uint8_t sexIn, uint8_t* sexOut) {
    if (sexOut) {
        *sexOut = sexIn;
    }
    if (!record) {
        return nullptr;
    }
    if (!sexIn) {
        if (record->m_nameMale[0]) {
            return record->m_nameMale;
        }

        if (record->m_nameFemale[0]) {
            if (sexOut) {
                *sexOut = 1;
            }
            return record->m_nameFemale;
        }

        return record->m_name;
    }

    if (sexIn != 1) {
        return record->m_name;
    }

    if (record->m_nameFemale[0]) {
        return record->m_nameFemale;
    }

    if (!record->m_nameMale[0]) {
        return record->m_name;
    }

    if (sexOut) {
        *sexOut = 0;
    }
    return record->m_nameMale;
}

const char* CGUnit_C::GetDisplayClassNameFromRecord(ChrClassesRec* record, uint8_t sexIn, uint8_t* sexOut) {
    if (sexOut) {
        *sexOut = sexIn;
    }
    if (!record) {
        return nullptr;
    }
    if (!sexIn) {
        if (record->m_nameMale[0]) {
            return record->m_nameMale;
        }

        if (record->m_nameFemale[0]) {
            if (sexOut) {
                *sexOut = 1;
            }
            return record->m_nameFemale;
        }

        return record->m_name;
    }

    if (sexIn != 1) {
        return record->m_name;
    }

    if (record->m_nameFemale[0]) {
        return record->m_nameFemale;
    }

    if (!record->m_nameMale[0]) {
        return record->m_name;
    }

    if (sexOut) {
        *sexOut = 0;
    }
    return record->m_nameMale;
}

// OFFSET: 0x70CBA0
void CGUnit_C::SetStorage(CGUnit_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_unit = reinterpret_cast<CGUnitData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_unitMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
