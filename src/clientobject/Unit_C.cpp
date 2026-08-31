#include "clientobject/Unit_C.hpp"

#include "db/Db.hpp"
#include "clientobject/Player_C.hpp"
#include <util/Byte.hpp>
#include "ObjectMgrClient.hpp"
#include <common/time/Time.hpp>
#include "client/ClientServices.hpp"
#include "util/DataStore.hpp"
#include "console/CVar.hpp"
#include "clientobject/Movement.hpp"

WGUID CGUnit_C::s_activeMover;
CVar* CGUnit_C::s_cvShowFootPrintParticles;
CVar* CGUnit_C::s_cvPathingDistTolerance;

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

// OFFSET: 0x718080
float CGUnit_C::GetMaxCameraHeight() {
    //ukn3 = this->movementData.ukn3;
    //if ((this->m_obj->m_type & TYPEMASK_PLAYER) != 0 || ukn3 <= 2.0277777)
    //    return ukn3 - 0.16666667;
    //else
        return 2.0277777 - 0.16666667;
}

// OFFSET: 0x71B810
bool CGUnit_C::GetCanFly() {
    return this->movementData.m_flags & MOVEMENTFLAG_CAN_FLY;
}

// OFFSET: 0x716710
bool CGUnit_C::IsClientControlled() {
    if ((this->m_unit->UNIT_FIELD_FLAGS & 2) == 0 && (this->m_unit->UNIT_FIELD_FLAGS & 0xC00004) != 0)
        return 0;

    if ((this->m_unit->UNIT_FIELD_FLAGS & 0x1000000) != 0) {
        WGUID v7 = this->m_unit->UNIT_FIELD_CHARMEDBY;
        if (v7 == 0)
            v7 = this->m_unit->UNIT_FIELD_CREATEDBY;
        auto v6 = ClntObjMgrObjectPtr<CGUnit_C*>(v7, TYPEMASK_UNIT);
        if (!v6 || (v6->m_obj->m_type & TYPEMASK_PLAYER) == 0)
            return 0;
        return (v6->m_unit->UNIT_FIELD_FLAGS & 1) == 0;
    } else {
        if ((this->m_obj->m_type & TYPEMASK_PLAYER) == 0 || this->m_unit->UNIT_FIELD_CHARMEDBY)
            return 0;
        return (this->m_unit->UNIT_FIELD_FLAGS & 1) == 0;
    }
}

// OFFSET: 0x714AC0
bool CGUnit_C::IsLocalClientControlled() {
    return (this->unk_0A30 >> 10) & 1;
}

// OFFSET: 0x74B9B0
void CGUnit_C::ToggleMovementFlag2_0x40(uint8_t flag) {
    this->movementData.ToggleMovementFlag2_0x40(flag);
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

// OFFSET: 0x73C8E0
void CGUnit_C::OnMonsterMove(CDataStore* msg, NETMESSAGE msgId, WGUID transportGuid, uint8_t transportFlags, bool flush) {
    this->unk_0A30 |= 0x20000000u;
    if (flush) {
        //this->movementData.FlushMoveQueue(0, 0);
    }
    //this->movementData.ForceSetTransport(transportGuid, transportFlags, 1);
    this->unk_0A30 &= ~0x20000000u;

    if (this->movementData.m_transportGuid != transportGuid)
        return;

    this->movementData.ToggleMovementFlag2_0x100(0);

    C3Vector dest = { 0.0f, 0.0f, 0.0f };
    *msg >> dest;

    uint32_t moveTicks;
    msg->Get(moveTicks);

    uint8_t type;
    msg->Get(type);

    C3Vector faceVector = { 0.0f, 0.0f, 0.0f };
    uint64_t faceGuid = 0;
    float faceAngle = 0.0f;

    if (type == 1) {
        C3Vector raw;
        this->GetRawPosition(raw);

        float dx = dest.x - raw.x;
        float dy = dest.y - raw.y;
        float dz = dest.z - raw.z;
        float tolerance = s_cvPathingDistTolerance->m_floatValue;

        if (dx * dx + dy * dy + dz * dz < tolerance * tolerance) {
            // this->movementData.sub_6F11B0(moveTicks, &dest, 0, 1);
            // this->sub_73AC30(0, -1);
            return;
        }
    } else if (type == 2) {
        *msg >> faceVector;
    } else if (type == 3) {
        msg->Get(faceGuid);
    } else if (type == 4) {
        msg->Get(faceAngle);
    }

    float vertSpeed = 0.0f;
    uint32_t vertTime = 0;
    uint8_t animState = 0;
    uint32_t animTime = 0;

    uint32_t splineFlags;
    uint32_t duration;
    uint32_t pointCount;

    if (type == 1) {
        duration = 0;
        splineFlags = SPLINE_FLAG_CAN_SWIM;
        pointCount = 1;
    } else {
        msg->Get(splineFlags);

        if ((splineFlags & 0x200000) != 0) {
            msg->Get(animState);
            msg->Get(animTime);
        }

        msg->Get(duration);

        if ((splineFlags & SPLINE_FLAG_PARABOLIC) != 0) {
            msg->Get(vertSpeed);
            msg->Get(vertTime);
        }

        msg->Get(pointCount);
    }

    C3Vector* block = static_cast<C3Vector*>(alloca(sizeof(C3Vector) * (pointCount + 4)));
    C3Vector* points = block;

    float facing = this->GetRawFacing();
    C3Vector facingDir = { cosf(facing), sinf(facing), 0.0f };

    C3Vector lastPoint = { 0.0f, 0.0f, 0.0f };
    uint32_t n = 0;
    bool haveSpline = true;

    if (type == 1) {
        C3Vector raw;
        this->GetRawPosition(raw);

        points[0] = { raw.x - facingDir.x, raw.y - facingDir.y, raw.z - facingDir.z };
        points[1] = raw;
        points[2] = dest;
        points[3] = dest;

        lastPoint = dest;
        n = 4;
    } else if ((splineFlags & 0x42000) != 0) {
        C3Vector raw;
        this->GetRawPosition(raw);

        points[0] = { raw.x - facingDir.x, raw.y - facingDir.y, raw.z - facingDir.z };
        points[1] = raw;
        n = 2;

        C3Vector point = { 0.0f, 0.0f, 0.0f };
        msg->Get(point.x);
        msg->Get(point.y);
        msg->Get(point.z);

        float dx = point.x - raw.x;
        float dy = point.y - raw.y;
        float dz = point.z - raw.z;

        if (dx * dx + dy * dy + dz * dz >= 0.00077160494f) {
            points[2] = point;
            n = 3;
        }

        for (uint32_t i = 1; i < pointCount; i++) {
            msg->Get(point.x);
            msg->Get(point.y);
            msg->Get(point.z);

            points[n] = point;
            n++;
        }

        if ((splineFlags & 0x80000) != 0) {
            points[n] = points[2];
            n++;
            points[n] = points[3];
            lastPoint = points[2];
        } else {
            points[n] = point;
            lastPoint = point;
        }

        n++;
    } else {
        points = block + 2;

        C3Vector endPoint = { 0.0f, 0.0f, 0.0f };
        *msg >> endPoint;

        points[0] = dest;
        n = 1;

        if (pointCount > 1) {
            C3Vector mid;
            mid.x = (dest.x + endPoint.x) * 0.5f;
            mid.y = (endPoint.y + dest.y) * 0.5f;
            mid.z = 0.5f * (endPoint.z + dest.z);

            for (uint32_t i = 0; i < pointCount - 1; i++) {
                C3Vector packed = { 0.0f, 0.0f, 0.0f };
                ReadPackedVector3(msg, &mid, &packed);
                points[n] = packed;
                n++;
            }

            points[n] = endPoint;
            n++;
        } else {
            float dx = endPoint.x - dest.x;
            float dy = endPoint.y - dest.y;
            float dz = endPoint.z - dest.z;

            if (dx * dx + dy * dy + dz * dz > 0.00077160494f) {
                points[1] = endPoint;
                n = 2;
            }
        }

        lastPoint = endPoint;

        C3Vector world;
        this->GetPosition(world);

        C3Vector* relative = this->ComputeTransportRelativeMovement(transportGuid, &world, points, &n) - 1;

        if (relative && n) {
            relative[n + 1] = relative[n];
            n += 2;

            relative[0].x = relative[1].x - (relative[2].x - relative[1].x);
            relative[0].y = relative[1].y - (relative[2].y - relative[1].y);
            relative[0].z = relative[1].z - (relative[2].z - relative[1].z);

            points = relative;
        } else {
            haveSpline = false;
        }
    }

    if (haveSpline && n > 3) {
        float length = 0.0f;

        for (uint32_t i = 1; i < n - 2; i++) {
            float dx = points[i + 1].x - points[i].x;
            float dy = points[i + 1].y - points[i].y;
            float dz = points[i + 1].z - points[i].z;

            length += sqrtf(dx * dx + dy * dy + dz * dz);
        }

        if (length > 0.16666667f) {
            float speed = this->m_passenger->m_runSpeed * 4.0f;
            if (speed <= 28.0f)
                speed = 28.0f;

            if ((splineFlags & 0x42000) != 0)
                speed = 50.0f;

            if (duration) {
                float bySeconds = length / (duration * 0.001f);
                if (bySeconds < speed)
                    speed = bySeconds;
            }

            if (speed > 0.00000095367432f) {
                int32_t durationMs = (int32_t)(length / speed * 1000.0f);
                if (durationMs <= 1)
                    durationMs = 1;

                duration = durationMs;

                // haveSpline = this->movementData.sub_6EB680(points, n, durationMs, splineFlags, moveTicks) != 0;
                haveSpline = false;
            } else {
                haveSpline = false;
            }
        } else {
            haveSpline = false;
        }
    } else {
        haveSpline = false;
    }

    if (haveSpline) {
        if (type == 2) {
            // this->movementData.SetSplineFaceData_VectorPos(&faceVector);
        } else if (type == 3) {
            // this->movementData.SetSplineFaceData_GuidTarget(&faceGuid);
        } else if (type == 4) {
            // this->movementData.SetSplineFaceData_FacingAngle(faceAngle);
        }

        if ((splineFlags & SPLINE_FLAG_PARABOLIC) != 0) {
            // this->movementData.OnMosterMoveFlag_0x800(vertSpeed, vertTime);
        } else if ((splineFlags & 0x200000) != 0) {
            // uint32_t state = this->dataB50[12];
            //
            // if ((state == 3 && (animState == 0 || animState == 2)) || (state == 2 && animState == 0)) {
            //     float scratch;
            //     sub_52E570(&scratch);
            //     this->GetObjectModel()->sub_82CED0(0x1CF, 0, &scratch);
            //
            //     if (duration > animBase + animTime)
            //         animTime = duration - animBase;
            // }
            //
            // this->movementData.OnMonsterMoveFlag_0x200000(animState, animTime);
        }

        // if (this->m_obj->OBJECT_FIELD_GUID == CGUnit_C::m_activeMover)
        //     CGInputControl::GetActive()->UpdatePlayer(OsGetAsyncTimeMs(), 1);
    } else {
        if (type == 2) {
            C3Vector origin;
            C3Vector target;
            this->movementData.GetPosition(&origin, &faceVector);
            this->GetPosition(target);
            // this->movementData.sub_6EE510(CalculateFacingTo(&target, &origin), flush);
        } else if (type == 3) {
            // this->sub_718930(&faceGuid, flush);
        } else if (type == 4) {
            // this->movementData.sub_6EE510(this->m_passenger->GetFacing(faceAngle), flush);
        }

        // this->movementData.sub_6F11B0(moveTicks, &lastPoint, splineFlags, flush);
    }

    // if (this->dataF00[23] && this->dataF00[23]->unk_0C)
    //     CVehicle_C::UpdateWorldMatrix(this->dataF00[23]);

    // if ((this->m_obj->unk_08 & 0x10) != 0)
    //     this->ChangeStandState(0);

    // this->sub_72AFE0(this->m_obj);

    // if (this->data980) {
    //     CEffect::Release(this->data980);
    //     this->data980 = nullptr;
    // }

    // this->sub_73AC30(0, -1);
}

// OFFSET: 0x7180C0
C3Vector* CGUnit_C::ComputeTransportRelativeMovement(WGUID guid, C3Vector* position, C3Vector* points, uint32_t* count) {
    C3Vector pos = *position;

    if (guid) {
        C44Matrix matrix;
        MovementGetTransportMtxX(guid, &matrix);
        pos = pos * matrix.AffineInverse();
    }

    uint32_t segments = *count - 1;

    if (*count == 1) {
        float dx = points[0].x - pos.x;
        float dy = points[0].y - pos.y;
        float dz = points[0].z - pos.z;

        if (dx * dx + dy * dy + dz * dz < 0.00077160494f) {
            *count = 0;
            return nullptr;
        }

        points[-1] = pos;
        *count += 1;
        return &points[-1];
    }

    uint32_t ahead = 0;
    uint32_t behind = 0;

    for (uint32_t i = 0; i < segments; i++) {
        C3Vector d = { points[i + 1].x - points[i].x,
                       points[i + 1].y - points[i].y,
                       points[i + 1].z - points[i].z };

        float c = -(d.z * pos.z + d.y * pos.y + d.x * pos.x);
        float s0 = d.z * points[i].z + d.y * points[i].y + d.x * points[i].x + c;
        float s1 = c + d.z * points[i + 1].z + d.y * points[i + 1].y + d.x * points[i + 1].x;

        if (s0 >= 0.0f && s1 >= 0.0f) {
            ahead++;
            continue;
        }

        if (s0 > 0.0f || s1 > 0.0f)
            break;

        behind++;
    }

    if (ahead == segments) {
        float dx = points[0].x - pos.x;
        float dy = points[0].y - pos.y;

        if (dx * dx + dy * dy < 1.0f)
            return points;

        points[-1] = pos;
        *count += 1;
        return &points[-1];
    }

    if (behind == segments) {
        C3Vector* last = &points[*count - 1];

        float dx = last->x - pos.x;
        float dy = last->y - pos.y;
        float dz = last->z - pos.z;

        if (dx * dx + dy * dy + dz * dz < 0.00077160494f) {
            *count = 0;
            return nullptr;
        }

        C3Vector tail = *last;
        points[0] = pos;
        points[1] = tail;
        *count = 2;
        return points;
    }

    if (segments == 0)
        return points;

    uint32_t k = 0;

    while (1) {
        C3Vector d = { points[k + 1].x - points[k].x,
                       points[k + 1].y - points[k].y,
                       points[k + 1].z - points[k].z };

        float inverse = 1.0f / sqrtf(d.z * d.z + d.y * d.y + d.x * d.x);

        C3Vector n = { d.x * inverse, d.y * inverse, d.z * inverse };

        float c = -(pos.z * n.z + n.y * pos.y + n.x * pos.x);
        float s0 = n.z * points[k].z + n.y * points[k].y + n.x * points[k].x + c;
        float s1 = n.x * points[k + 1].x + (n.y * points[k + 1].y + n.z * points[k + 1].z) + c;

        if (s0 > 0.0f || s1 < 0.0f) {
            if (s0 >= 0.0f && s1 >= 0.0f) {
                *count -= k;

                C3Vector* head = &points[k];

                float dx = head->x - pos.x;
                float dy = head->y - pos.y;

                if (dx * dx + dy * dy < 1.0f)
                    return head;

                head[-1] = pos;
                *count += 1;
                return &head[-1];
            }

            k++;

            if (k >= *count - 1)
                return points;

            continue;
        }

        float t = s0 / (s0 - s1);

        C3Vector hit;
        hit.x = (points[k + 1].x - points[k].x) * t + points[k].x;
        hit.y = (points[k + 1].y - points[k].y) * t + points[k].y;
        hit.z = t * (points[k + 1].z - points[k].z) + points[k].z;

        float ax = points[k + 1].x - hit.x;
        float ay = points[k + 1].y - hit.y;
        float az = points[k + 1].z - hit.z;

        if (ax * ax + ay * ay + az * az >= 0.00077160494f) {
            points[k] = hit;
        } else {
            float bx = points[k + 1].x - pos.x;
            float by = points[k + 1].y - pos.y;
            float bz = points[k + 1].z - pos.z;

            if (bx * bx + by * by + bz * bz >= 0.00077160494f) {
                points[k] = pos;
                *count -= k;
                return &points[k];
            }

            k++;

            if (k == segments) {
                *count = 0;
                return nullptr;
            }
        }

        *count -= k;
        return &points[k];
    }
}

// OFFSET: 0x74B9A0
bool CGUnit_C::NoStrafe() {
    return this->movementData.m_flags2 & MOVEMENTFLAG2_NO_STRAFE;
}

// OFFSET: 0x717A20
CreatureModelDataRec* CGUnit_C::GetModelData() {
    uint32_t displayId = this->m_displayId;
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

// OFFSET: 0x715330
void CGUnit_C::ClientInitialize() {
    s_cvShowFootPrintParticles = CVar::Register("showfootprintparticles", "toggles rendering of footprint particles", 1, "1", 0, 1, 0, 0, 0);
    s_cvPathingDistTolerance = CVar::Register("pathDistTol", "Sets acceptable distance from pathing destination in yards", 0, "1", 0, 4, 0, 0, 0);
}

// OFFSET: 0x742220
void CGUnit_C::Initialize() {
    //ClientServices::SetMessageHandler(MSG_MOVE_START_FORWARD, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_BACKWARD, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_STRAFE_LEFT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_STRAFE_RIGHT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP_STRAFE, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_ASCEND, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_DESCEND, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP_ASCEND, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_JUMP, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_TURN_LEFT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_TURN_RIGHT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP_TURN, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_PITCH_UP, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_PITCH_DOWN, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP_PITCH, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_RUN_MODE, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_WALK_MODE, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_TELEPORT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_FACING, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_PITCH, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_TOGGLE_COLLISION_CHEAT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_GRAVITY_CHNG, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_RUN_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_RUN_BACK_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_WALK_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_SWIM_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_SWIM_BACK_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_FLIGHT_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_FLIGHT_BACK_SPEED, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_TURN_RATE, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_PITCH_RATE, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_SET_COLLISION_HGT, Packet_Group_22, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_ROOT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_UNROOT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_SWIM, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP_SWIM, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_START_SWIM_CHEAT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_STOP_SWIM_CHEAT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_HEARTBEAT, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_FALL_LAND, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_UPDATE_CAN_FLY, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_UPDATE_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_TELEPORT_ACK, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_TIME_SKIPPED, Packet_MSG_MOVE_TIME_SKIPPED, 0);
    ClientServices::SetMessageHandler(SMSG_MONSTER_MOVE, &CGUnit_C::HandleMonsterMovePacket, 0);
    ClientServices::SetMessageHandler(SMSG_MONSTER_MOVE_TRANSPORT, &CGUnit_C::HandleMonsterMovePacket, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_RUN_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_RUN_BACK_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_SWIM_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_FLIGHT_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_WALK_SPEED_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_TURN_RATE_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_PITCH_RATE_CHANGE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_MOVE_ROOT, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_MOVE_UNROOT, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_WATER_WALK, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_LAND_WALK, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_FEATHER_FALL, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_NORMAL_FALL, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_SET_HOVER, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_UNSET_HOVER, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_GRAVITY_DISABLE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_GRAVITY_ENABLE, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_SET_COLLISION_HGT, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_SET_CAN_FLY, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_UNSET_CAN_FLY, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOVE_KNOCK_BACK, Packet_Group_23, 0);
    //ClientServices::SetMessageHandler(SMSG_MOUNTSPECIAL_ANIM, Packet_SMSG_MOUNTSPECIAL_ANIM, 0);
    //ClientServices::SetMessageHandler(SMSG_AI_REACTION, Packet_SMSG_AI_REACTION, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_KNOCK_BACK, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_HOVER, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_FEATHER_FALL, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_WATER_WALK, Packet_Group_21, 0);
    //ClientServices::SetMessageHandler(SMSG_PET_ACTION_SOUND, Packet_SMSG_PET_ACTION_SOUND, 0);
    //ClientServices::SetMessageHandler(SMSG_PET_DISMISS_SOUND, Packet_SMSG_PET_DISMISS_SOUND, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_ROOT, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_GRAVITY_DISABLE, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_GRAVITY_ENABLE, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_UNROOT, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_FEATHER_FALL, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_NORMAL_FALL, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_SET_HOVER, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_UNSET_HOVER, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_WATER_WALK, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_LAND_WALK, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_START_SWIM, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_STOP_SWIM, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_SET_RUN_MODE, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_SET_WALK_MODE, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_SET_FLYING, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_MOVE_UNSET_FLYING, Packet_Group_25, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_RUN_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_RUN_BACK_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_SWIM_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_SWIM_BACK_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_FLIGHT_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_FLIGHT_BACK_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_WALK_SPEED, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_TURN_RATE, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_SPLINE_SET_PITCH_RATE, Packet_Group_26, 0);
    //ClientServices::SetMessageHandler(SMSG_STANDSTATE_UPDATE, Packet_SMSG_STANDSTATE_UPDATE, 0);
    //ClientServices::SetMessageHandler(SMSG_COMPRESSED_MOVES, Packet_SMSG_COMPRESSED_MOVES, 0);
    //ClientServices::SetMessageHandler(SMSG_COMPRESSED_UNKNOWN_1310, Packet_SMSG_UNKNOWN_1310, 0);
    //ClientServices::SetMessageHandler(SMSG_CLIENT_CONTROL_UPDATE, Packet_SMSG_CLIENT_CONTROL_UPDATE, 0);
    //ClientServices::SetMessageHandler(SMSG_FLIGHT_SPLINE_SYNC, Packet_SMSG_FLIGHT_SPLINE_SYNC, 0);
    //ClientServices::SetMessageHandler(SMSG_AURA_UPDATE_ALL, Packet_Group_27, 0);
    //ClientServices::SetMessageHandler(SMSG_AURA_UPDATE, Packet_Group_27, 0);
    //ClientServices::SetMessageHandler(SMSG_DISMOUNT, Packet_SMSG_DISMOUNT, 0);
    //ClientServices::SetMessageHandler(SMSG_LOOT_LIST, Packet_SMSG_LOOT_LIST, 0);
    //ClientServices::SetMessageHandler(SMSG_MIRRORIMAGE_DATA, Packet_SMSG_MIRRORIMAGE_DATA, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_DISPLAY_UPDATE, Packet_SMSG_FORCE_DISPLAY_UPDATE, 0);
    //ClientServices::SetMessageHandler(SMSG_CANCEL_AUTO_REPEAT, Packet_SMSG_CANCEL_AUTO_REPEAT, 0);
    //ClientServices::SetMessageHandler(SMSG_HEALTH_UPDATE, Packet_SMSG_HEALTH_UPDATE, 0);
    //ClientServices::SetMessageHandler(SMSG_POWER_UPDATE, Packet_SMSG_POWER_UPDATE, 0);
    //ClientServices::SetMessageHandler(SMSG_HIGHEST_THREAT_UPDATE, Packet_Group_28, 0);
    //ClientServices::SetMessageHandler(SMSG_THREAT_UPDATE, Packet_Group_28, 0);
    //ClientServices::SetMessageHandler(SMSG_THREAT_REMOVE, Packet_SMSG_THREAT_REMOVE, 0);
    //ClientServices::SetMessageHandler(SMSG_THREAT_CLEAR, Packet_SMSG_THREAT_CLEAR, 0);
    //ClientServices::SetMessageHandler(SMSG_PRE_RESURRECT, Packet_SMSG_PRE_RESURRECT, 0);
    //ClientServices::SetMessageHandler(SMSG_SET_VEHICLE_REC_ID, Packet_SMSG_PLAYER_VEHICLE_DATA, 0);
    //ClientServices::SetMessageHandler(SMSG_COMPOUND_MOVE, Packet_SMSG_MULTIPLE_PACKETS, 0);
    //ClientServices::SetMessageHandler(SMSG_FORCE_ANIM, Packet_SMSG_UNKNOWN_1240, 0);
    //maybe_UnitSoundInitialize();
    //Spell_C::SystemInitialize();
    //bn_UnitCombatClientInitialize();
    //maybe_CGUnit_C__RegisterMirrorHandlers();
    //sub_7165D0();
    //numRows = bnl_g_environmentalDamageDB.numRows;
    //dword_CA120C[0] = 0;
    //dword_CA1210 = 0;
    //dword_CA1214 = 0;
    //dword_CA1218 = 0;
    //dword_CA121C = 0;
    //dword_CA1220 = 0;
    //v1 = bnl_g_environmentalDamageDB.numRows;
    //if (bnl_g_environmentalDamageDB.numRows) {
    //    v2 = &bnl_g_environmentalDamageDB.FirstRow[3 * bnl_g_environmentalDamageDB.numRows];
    //    do {
    //        --v1;
    //        v2 -= 3;
    //        if (v1 < 0 || v1 >= numRows)
    //            v3 = 0;
    //        else
    //            v3 = v2;
    //        v4 = v3[1];
    //        if (v4 < 6)
    //            dword_CA120C[v4] = v3[2];
    //    } while (v1);
    //}
    //dword_CA11F4 = 13;
    //dword_CA11D4 = 0;
    //dword_CA11C8 = 0;
    //dword_CA11CC = 0;
    //dword_CA11C0 = 0;
    //dword_CA11C4 = 0;
    //dword_CA11B8 = 0;
    //dword_CA11BC = 0;
    //dword_CA11B0 = 0;
    //dword_CA11B4 = 0;
    //dword_CA11A8 = 0;
    //dword_CA11AC = 0;
    //CGUnit_C::m_activeMover = 0i64;
    //CMissile::Initialize();
    //maybe_TSFixedArray__ReallocData_6();
    //maybe_CVehiclePassenger_C__InitSystem();
    //maybe_TSFixedArray__ReallocData_3();
    //maybe_CGUnit_C__InitMissileTrajectorySystem();
    //bn_CGUnit_C_VehiclePassengerInitWorldCameraState();
    //result = SMemAlloc(4, ".\\Unit_C.cpp", 10361, 0);
    //v6 = result;
    //if (result) {
    //    result = ObjectAllocAddHeap(48, 16, "Unit Threat", 1);
    //    *v6 = result;
    //    bnl_CGUnit_C__s_unitThreatPool = v6;
    //} else {
    //    bnl_CGUnit_C__s_unitThreatPool = 0;
    //}
    //bnl_CGUnit_C__s_deferredClientControlUpdateGUID = 0i64;
    //bnl_CGUnit_C__s_deferredClientControlUpdateState = 0;
    //bnl_CGUnit_C__m_initialized = 1;
}

// OFFSET: 0x73F590
int32_t CGUnit_C::HandleMonsterMovePacket(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WGUID guid;
    *msg >> guid;

    CGUnit_C* unit = ClntObjMgrObjectPtr<CGUnit_C*>(guid, TYPEMASK_UNIT);
    if (unit) {
        WGUID transportGuid = 0;
        uint8_t v9 = 0;
        if (msgId == SMSG_MONSTER_MOVE_TRANSPORT) {
            *msg >> transportGuid;
            msg->Get(v9);
        }
        uint8_t v10 = 0;
        msg->Get(v10);
        unit->ToggleMovementFlag2_0x40(v10);
        //if ( !unit->sub_74C040(msg, transportGuid, v9) )
        unit->OnMonsterMove(msg, msgId, transportGuid, v9, 1);
        return 1;
    }

    msg->Seek(msg->Size());
    return 0;
}
