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
#include <gameui/CGInputControl.hpp>
#include <gameui/CGWorldFrame.hpp>
#include "gameui/camera/CGCamera.hpp"
#include <util/Network.hpp>
#include <client/FrameTime.hpp>
#include <tempest/Math.hpp>
#include "clientobject/PlayerName.hpp"
#include "db/DBCache.hpp"
#include "db/DBCacheInstances.hpp"
#include "ui/FrameScript.hpp"
#include <gameui/CGGameUI.hpp>
#include <util/Animation.hpp>

WGUID CGUnit_C::s_activeMover = 0;
CVar* CGUnit_C::s_cvShowFootPrintParticles = nullptr;
CVar* CGUnit_C::s_cvPathingDistTolerance = nullptr;
int32_t CGUnit_C::m_trackingType = 0;
float CGUnit_C::m_trackingFacing = 0.0f;

static const uint32_t s_unitMirrorIndex[CGUnit::TotalFields() - CGObject::TotalFields()] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 61, 63, 64,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 76, 77, 78,
    79, 80, 81, 82, 93, 94, 95, 96, 97, 98, 99, 100,
    101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    113, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
    127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
    139, 140, 142,
};

// OFFSET: 0x4F52D0
uint32_t CGUnit::MirrorIndexFromFieldIndex(uint32_t fieldIndex) {
    for (uint32_t i = 0; i < CGUnit::TotalFields() - CGObject::TotalFields(); i++) {
        if (s_unitMirrorIndex[i] == fieldIndex) {
            return i;
        }
    }

    return CGUnit::GetFieldCount();
}

// OFFSET: 0x4D4240
uint32_t CGUnit::DescriptorToMirrorOffset(uint32_t fieldByteOffset, uint32_t fieldByteSize, int32_t localPlayer, uint32_t baseByteOffset) {
    uint32_t mirrorIndex = CGUnit::MirrorIndexFromFieldIndex((fieldByteOffset - baseByteOffset) >> 2);
    return (fieldByteOffset & 3) + 4 * (CGObject::TotalFields() + mirrorIndex);
}

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
    //*&this->data9D8 = 0.0;
    //*&this->data9DC = 0.0;
    //*&this->data9E0 = 1.0;
    //this->data9E8 = 0;
    //this->data9EC = 0;
    //this->data9F0 = 0;
    //LOBYTE(this->data9F4) = 0;
    //this->data9F8 = 0;
    //this->dataA1C = 0;
    //this->dataA20 = 0;
    //this->dataA24 = 0;
    //this->dataA28 = 0;
    //this->dataA2C = 0;
    this->m_animationState = 0x70;
    //this->dataA3C = -1;
    //this->dataA40 = -1;
    //*this->dataA44 = 0.27777779;
    //*&this->dataA44[1] = 0.27777779;
    //this->dataA44[5] = 0;
    //LOBYTE(this->dataA44[6]) = 0;
    //*&this->dataA44[2] = 1.0;
    //this->dataA44[7] = 0;
    //this->dataA44[8] = 0;
    //*&this->dataA44[3] = -10.0;
    //this->dataA44[9] = 0;
    //this->dataA44[10] = 0;
    //*&this->dataA44[4] = 3.4028235e38;
    //this->dataA44[13] = 0;
    //this->dataA44[14] = 0;
    this->m_renderFacing = this->movementData.m_facing;
    //this->dataA44[15] = 0;
    //this->dataA44[16] = 0;
    //*&this->dataA44[21] = 0.0;
    //this->dataA44[17] = 0;
    //this->dataA44[18] = 0;
    //*&this->dataA44[22] = 1.0;
    //this->dataA44[19] = 0;
    //this->dataAA4[7] = 0;
    //this->dataAA4[8] = 0;
    //this->dataAA4[9] = 0;
    //SSyncObject::SSyncObject(&this->dataAA4[10]);
    //this->dataAA4[11] = 0;
    //this->dataAA4[12] = 0;
    //*&this->dataAA4[14] = 0.0;
    //*&this->dataAA4[15] = 0.0;
    //*&this->dataAA4[19] = 0.0;
    //*&this->dataAA4[20] = 0.0;
    //*&this->dataAA4[21] = 0.0;
    //this->dataAA4[22] = 0;
    //this->dataAA4[23] = 0;
    //this->dataAA4[24] = 0;
    //this->dataAA4[25] = 0;
    //*&this->dataAA4[26] = 0.0;
    //this->dataAA4[27] = 0;
    //this->dataAA4[28] = 0;
    //LOBYTE(this->dataAA4[31]) = -1;
    //this->dataAA4[32] = 0;
    //this->dataAA4[33] = 0;
    //this->dataAA4[34] = 0;
    //this->dataAA4[35] = 0;
    //this->dataAA4[36] = 0;
    //*&this->dataB3C = 1.0;
    //m_unit = this->m_unit;
    //this->dataAA4[37] = 0;
    //this->modelB40 = 0;
    //this->modelB44 = 0;
    //this->dataB48 = 0;
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
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) == 0)
        this->OnMoveUpdate(time, 1, 1);
    //bn_CGUnit_C_UpdateSelectionRadius(this);
    //maybe_CGObject_C__UpdateEffectAttachments(this);
    this->CGObject_C::PostInit(time, objCreate, isUpdate3);
    //maybe_CGUnit_C__VehiclePassengerInit(this);
    //MovementUpdateCameraYaw(this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high, this->movementData.__base.transportGuid.guid_low, this->movementData.__base.transportGuid.guid_high);
    //m_worldModel = this->ObjectBase.m_worldModel;
    //CM2Model::SetSequenceCallback(m_worldModel, maybe_CGUnit_C__DispatchAnimEnd, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high);
    //CWorldScene::LoadModel(m_worldModel, COERCE_FLOAT(bn_AnimEventCallback_1), *&this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_low, *&this->ObjectBase.m_obj->OBJECT_FIELD_GUID.guid_high);
    if (this->m_displayInfo) {
    //    bn_CCharacterComponent_ApplyMonsterGeosets(this->ObjectBase.m_worldModel, this->displayInfo);
    //    maybe_CCharacterComponent__ReplaceMonsterSkin(this->ObjectBase.m_worldModel, this->displayInfo, this->modelData);
    //    modelData = this->modelData;
    //    if (modelData)
    //        this->ObjectBase.m_worldModel->f_flags ^= (this->ObjectBase.m_worldModel->f_flags ^ (modelData->m_flags >> 7)) & 4;
    }
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
    this->m_targetFacing = CMath::normalizeangle0to2pi(this->GetRawFacing());
    //*&this->data9E0[49] = 0.0;
    this->m_turnDelta[0] = 0.0;
    this->m_turnDelta[1] = 0.0;
    this->m_turnDelta[2] = 0.0;
    this->m_turnDelta[3] = 0.0;
    this->Animate(0.0f);
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
    this->m_nameDesc = PlayerNameCreate(this->m_obj->m_guid);
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
    this->UpdateBaseAnimation(0, -1);
    //v17 = this->ObjectBase.m_worldModel;
    //if (this->m_worldModel && this->m_worldModel->IsLoaded(0, 0)) {
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
        if ((this->movementData.m_flags & 0x2000) != 0)
            this->OnCollideFalling();
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

// OFFSET: 0x717C50
void CGUnit_C::InitActiveMover(WGUID guid) {
    CGUnit_C::s_activeMover = guid;
    CDataStore msg;
    msg.Put((uint32_t)CMSG_SET_ACTIVE_MOVER);
    msg.Put((uint64_t)CGUnit_C::s_activeMover);
    msg.Finalize();
    ClientServices::Send2(&msg);

    uint32_t time = OsGetAsyncTimeMs();
    CGInputControl::GetActive()->UpdatePlayer(time, 1);

    CGUnit_C* mover = ClntObjMgrObjectPtr<CGUnit_C*>(CGUnit_C::s_activeMover, TYPEMASK_UNIT);
    if ((mover->m_passenger->m_flags & 0xC0100F) != 0)
        mover->movementData.UpdateHeartbeatTimerA(time);

    //v5 = (v4->ObjectBase.GetTransportGUID)(v4);
    //if (v5) {
    //    v6 = ClntObjMgrObjectPtr(v5, TYPEMASK_GAMEOBJECT);
    //    if (v6) {
    //        v7 = (v6->data0DC[16].m_terminator.m_prevlink->m_prevlink[21].m_prevlink)(v6->data0DC[16].m_terminator.m_prevlink);
    //        MovementSetTransportUpdateTime(v7);
    //    }
    //}
    //result = bn_CVehiclePassenger_C_OnSetActiveMover(v4);

    msg.Destroy();
}

// OFFSET: 0x741D00
void CGUnit_C::RegisterMirrorHandlers() {
    //for (i = 200; i < 0xD4; i += 4)
    //    ClntObjMgrSetTypeMirrorHandler(3, i, 4, bn_VirtualItemIDMirrorHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xC0u, 4, maybe_CGUnit_C__OnLevelFieldChanged, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(0, 0xCu, 4, bn_UnitModeUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x48u, 4, maybe_CGUnit_C__OnFlagChanged_0, 0, 0, 0);
    //for (j = 108; j < 0x88; j += 4) {
    //    ClntObjMgrSetTypeMirrorHandler(3, j - 32, 4, maybe_CGUnit_C__UpdatePredictedPower, 0, 0, 0);
    //    ClntObjMgrSetTypeMirrorHandler(3, j, 4, maybe_CGUnit_C__OnPowerFieldChanged, 0, 0, 0);
    //}
    //ClntObjMgrSetTypeMirrorHandler(3, 0x47u, 1, maybe_CGUnit_C__OnDisplayPowerChanged, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xDCu, 4, maybe_Signal_EVENT_PET_BAR_UPDATE_USABLE, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xD4u, 4, bn_UnitFlagUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xD8u, 4, bn_UnitFlag2UpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x112u, 1, bn_UnitVisFlagUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x1D1u, 1, bn_UnitPvPFlagUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x113u, 1, maybe_CGUnit_C__SyncPowerTypeField, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xFCu, 4, bn_MountDisplayIDUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xC4u, 4, bn_UnitFactionUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x18u, 16, bn_UnitCharmedUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0xF4u, 4, bn_DisplayIDUpdateHandler, 0, 1, 0);
    //ClntObjMgrSetTypeMirrorHandler(ID_UNIT, 0x110u, 1, StandStateUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x130u, 4, maybe_NPCFlagsHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x134u, 4, maybe_CGUnit_C__ResetUnitByGuid, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x118u, 4, bn_PetNameChangeHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x124u, 4, bn_DynamicFlagsChangeHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x38u, 12, bn_ChannelSpellOrObjectChangeHandler, 0, 0, 1);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x114u, 4, bn_PetNumberChangeHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(0, 0x10u, 4, bn_ScaleUpdateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x1D0u, 1, bn_SheatheStateHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x30u, 8, bn_TargetChangeHandler, 0, 0, 0);
    //ClntObjMgrSetTypeMirrorHandler(3, 0x230u, 4, bn_HoverHeightChangeHandler, 0, 0, 0);
}

// OFFSET: 0x715C60
int32_t CGUnit_C::GetTrackingType() {
    return CGUnit_C::m_trackingType;
}

// OFFSET: 0x715CF0
float CGUnit_C::GetTrackingTurn() {
    return CGUnit_C::m_trackingFacing;
}

// OFFSET: 0x7395C0
bool UpdateAllSmoothFacingCallback(WGUID guid, void* param) {
    CGCamera* camera = reinterpret_cast<CGCamera*>(param);
    CGObject_C* obj = ClntObjMgrObjectPtr<CGObject_C*>(guid, TYPEMASK_OBJECT);
    if (obj) {
        if ((obj->m_obj->m_type & TYPEMASK_UNIT) != 0 && obj->m_obj->m_guid != camera->m_targetGUID && !obj->AsUnit()->HasVehicleTransport()) {
            obj->AsUnit()->UpdateSmoothFacing(nullptr);
        }
    }
    return true;
}

// OFFSET: 0x739630
void CGUnit_C::UpdateAllSmoothFacing() {
    auto camera = CGWorldFrame::GetActiveCamera();
    if (camera)
        ClntObjMgrEnumVisibleObjects(UpdateAllSmoothFacingCallback, camera);
}

// OFFSET: 0x72CDE0
void CreatureQueryCallback(uint32_t id, void* data, void* arg, int32_t success) {
    uint64_t requester = 0;
    CreatureStats_C* entry = g_creatureCache.GetRecord(static_cast<int32_t>(id), &requester, nullptr, nullptr, false);

    if (!entry) {
        return;
    }

    WGUID guid = *static_cast<uint64_t*>(data);
    CGUnit_C* unit = ClntObjMgrObjectPtr<CGUnit_C*>(guid, TYPEMASK_UNIT);

    if (unit) {
        //unit->DestroyNamePlate();
        unit->m_creatureCacheEntry = entry;
        //if (entry->m_family) {
        //    unit->UpdateModelScale(0);
        //}
        //unit->UpdateUnitCollisionBox(1, 0);
    }

    CGGameUI::UnitNameUpdate(guid);
    //Script_SendUnitSignal(&guid, 148);

    //auto totemInfo = CGGameUI::GetTotemInfo(guid);
    //if (totemInfo) {
    //    totemInfo->unk_10 = unit->GetUnitName(0, 1);
    //    FrameScript::SignalEvent(165, "%d", totemInfo->unk_00 + 1);
    //}
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
        uint64_t requester = static_cast<uint64_t>(this->m_obj->m_guid);
        this->m_creatureCacheEntry = g_creatureCache.GetRecord(static_cast<int32_t>(this->m_obj->m_entryID), &requester, CreatureQueryCallback, nullptr, false);
    }

    if (this->m_unit->UNIT_FIELD_NATIVEDISPLAYID == this->m_unit->UNIT_FIELD_DISPLAYID)
        this->unk_0A30 |= 0x100u;
    else
        this->unk_0A30 &= ~0x100u;
    if ((this->m_modelData->m_flags & 8) != 0)
        this->m_animationState |= 0x20000u;
    else
        this->m_animationState &= ~0x20000u;
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
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) != 0 || this->movementData.m_collisionHeight <= 2.0277777)
        return this->movementData.m_collisionHeight - 0.16666667;
    else
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

// OFFSET: 0x716FA0
bool CGUnit_C::IsRunning() {
    return this->movementData.m_walkSpeed + this->movementData.m_walkSpeed >= this->movementData.GetBaseSpeed(0);
}

// OFFSET: 0x714AC0
bool CGUnit_C::IsLocalClientControlled() {
    return (this->unk_0A30 >> 10) & 1;
}

// OFFSET: 0x71EF20
bool CGUnit_C::IsAllowedToSendMessage(NETMESSAGE msgId) {
    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        if (!this->m_passenger->IsOnSpline() || IsMessageAllowedWhileOnSpline(msgId))
            return 1;
    }
    return false;
}

// OFFSET: 0x74B9B0
void CGUnit_C::ToggleMovementFlag2_0x40(uint8_t flag) {
    this->movementData.ToggleMovementFlag2_0x40(flag);
}

// OFFSET: 0x717AD0
float CGUnit_C::GetStandHeight() {
    if (!this->m_modelData) {
        this->m_modelData = this->GetModelData();
        if (!this->m_modelData)
            return 0.0f;
    }
    return this->GetTrueScale() * (this->m_modelData->m_geoBoxMaxZ - this->m_modelData->m_geoBoxMinZ);
}

// OFFSET: 0x72A000
char* CGUnit_C::GetUnitName(char** a2, bool a3) {
    //v3 = 0;
    //if (a3 && (this->dataF00[16] & 0x800000) != 0) {
    //    WowClientDB::GetRow(&v18);
    //    v21 = 0;
    //    if (CGUnit_C::GetAuraCount(this)) {
    //        while (1) {
    //            auraCount2 = this->auraCount2;
    //            if (auraCount2 == -1)
    //                auraCount2 = this->auraData2[0].auraCount1;
    //            if (v21 >= auraCount2)
    //                v6 = 0;
    //            else
    //                v6 = this->auraCount2 == -1 ? &this->auraData2[0].auradata1[v3] : &this->auraData2[v3];
    //            if (ClientDb::GetLocalizedRow(&g_spellDB, v6->spellId, &v18))
    //                break;
//LABEL_16:
    //            ++v21;
    //            ++v3;
    //            if (v21 >= CGUnit_C::GetAuraCount(this))
    //                goto LABEL_17;
    //        }
    //        v7 = 0;
    //        while (v18.m_effectAura[v7] != 279 || ((1 << v7) & v6->flags) == 0) {
    //            if (++v7 >= 3)
    //                goto LABEL_16;
    //        }
    //        v11 = ClntObjMgrObjectPtr(v6->creator, TYPEMASK_UNIT);
    //        if (v11)
    //            UnitName = CGUnit_C::GetUnitName(v11, a2, 0);
    //        else
    //            UnitName = GetObjectNameFromGuid(&v6->creator.guid_low);
    //        NOP(v17);
    //        return UnitName;
    //    }
//LABEL_17:
    //    NOP(v17);
    //}

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) != 0) {
        //uint64_t guid = static_cast<uint64_t>(this->m_obj->m_guid);
        //NameCache* record = g_nameCache.GetRecord(guid, &guid, NameQueryCallback, nullptr, true);
        //if (record) {
        //    if (a2 && record->m_realmName[0]) {
        //        *a2 = record->m_realmName;
        //    }
        //    return record->m_name;
        //}
    } else if (this->m_unit->UNIT_FIELD_PETNUMBER) {
        //uint64_t guid = static_cast<uint64_t>(this->m_obj->m_guid);
        //PetNameCache* record = g_petNameCache.GetRecord(this->m_unit->UNIT_FIELD_PETNUMBER, &guid, NameQueryCallback, nullptr, true);
        //if (record) {
        //    if (record->m_timestamp == this->m_unit->UNIT_FIELD_PET_NAME_TIMESTAMP) {
        //        return record->m_name;
        //    }
        //    UnitCombatLogInvalidateName(record);
        //    g_petNameCache.Invalidate(this->m_unit->UNIT_FIELD_PETNUMBER);
        //    g_petNameCache.GetRecord(this->m_unit->UNIT_FIELD_PETNUMBER, &guid, NameQueryCallback, nullptr, true);
        //}
    } else if (this->m_creatureCacheEntry) {
        return this->m_creatureCacheEntry->m_name[0];
    }

    const char* text = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);

    if (!text || !text[0]) {
        text = "Unknown Being";
    }

    return const_cast<char*>(text);
}

// OFFSET: 0x715500
void CGUnit_C::UpdateUnitNameText() {
    if (this->m_nameDesc)
        PlayerNameTriggerNameRegenerate(this->m_nameDesc);
    //m_namePlateFrame = this->m_namePlateFrame;
    //if (m_namePlateFrame)
    //    bn_CGNamePlateFrame_UpdateNameDisplay(m_namePlateFrame, this);
}

// OFFSET: 0x7207E0
bool CGUnit_C::IsLowPrioritySelection(uint32_t time) {
    if ((this->m_unit->UNIT_FIELD_FLAGS & 0x2000000) == 0) {
        if (this->m_obj->m_type != (TYPEMASK_UNIT | TYPEMASK_OBJECT))
            return 0;
        if (this->m_unit->UNIT_FIELD_HEALTH > 0)
            return 0;
        //if (bn_CGUnit_C_CanBeLooted(a2)) {
        //    ClntObjMgrGetActivePlayerObj();
        //    if (CGPlayer_C::CanLoot(this))
        //        return 0;
        //}
        //if ((this->m_unit->UNIT_FIELD_FLAGS & 0x4000000) != 0 && CGSpellBook::GetSkinningSpell(this))
        //    return 0;
    }
    return true;
}

// OFFSET: 0x4CEE50
bool CGUnit_C::IsActivePlayer() {
    return this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
}

// OFFSET: 0x718FC0
bool CGUnit_C::IsDisarmed(uint8_t a2) {
    if (a2 == 2) {
        return false;
    }

    if ((this->m_unit->UNIT_FIELD_FLAGS & 0x200000) == 0) {
        return a2 == 1 && (this->m_unit->UNIT_FIELD_FLAGS_2 & 0x80) != 0;
    }

    CGUnitVirtualItem* mainHand = this->GetVirtualItem(0, 1);

    if (a2) {
        if (!this->IsDisarmed(0)) {
            CGUnitVirtualItem* offHand = this->GetVirtualItem(1, 1);

            if (offHand) {
                if (offHand->classID == 2) {
                    return true;
                }
            }
        }

        return a2 == 1 && (this->m_unit->UNIT_FIELD_FLAGS_2 & 0x80) != 0;
    }

    return mainHand && mainHand->classID == 2;
}

// OFFSET: 0x71B6B0
bool CGUnit_C::IsLooting() {
    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer())
        return this->AsPlayer()->m_lootTarget != 0;

    return (this->m_unit->UNIT_FIELD_FLAGS >> 10) & 1;
}

// OFFSET: 0x7222A0
bool CGUnit_C::ShouldKneelForLoot() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer())
        return (this->m_unit->UNIT_FIELD_FLAGS & 0x10000000) == 0;
    auto v6 = ClntObjMgrObjectPtr<CGObject_C*>(this->AsPlayer()->m_lootTarget, TYPEMASK_OBJECT);
    if (!v6)
        return false;

    if ((v6->m_obj->m_type & TYPEMASK_GAMEOBJECT) != 0) {
        //return BYTE1(v6->m_unit->UNIT_FIELD_CREATEDBY.guid_high) != 17;
    } else if ((v6->m_obj->m_type & TYPEMASK_UNIT) != 0) {
        return v6->AsUnit()->m_unit->UNIT_FIELD_HEALTH <= 0;
    }
    return (v6->m_obj->m_type & TYPEMASK_ITEM) == 0;
}

// OFFSET: 0x71DE90
bool CGUnit_C::CanShuffle() {
    if ((this->movementData.m_flags & (MOVEMENTFLAG_RIGHT | MOVEMENTFLAG_LEFT)) == 0 && (this->m_animationState & 0x1800) == 0)
        return 0;
    if (this->movementData.IsSplineFlyer_NotHoveringFlyingSwimming())
        return 0;
    //m_vehicle = this->m_vehicle;
    //if (m_vehicle) {
    //    if (m_vehicle[3] && CVehicle::sub_7571C0(m_vehicle))
    //        return 0;
    //}
    auto torsoAnim = this->GetCurrentTorsoAnimId();
    return !IsEmoteAnim(torsoAnim) && !IsSpellCastAnim(torsoAnim) && !IsThrownWeaponAnim(torsoAnim) && !IsBowAnim(torsoAnim) && !IsRifleAnim(torsoAnim) && (this->m_animationState & 0x40000C) == 0;
}

// OFFSET: 0x4F6250
bool CGUnit_C::IsVehicleDriver() {
    WHOA_UNIMPLEMENTED(false);
    //return this->m_vehiclePassenger && this->m_vehiclePassenger->m_seatState == 3;
}

// OFFSET: 0x715D70
bool CGUnit_C::IsBoss() {
    if (this->m_creatureCacheEntry)
        return (this->m_creatureCacheEntry->m_typeFlags >> 2) & 1;
    return false;
}

// OFFSET: 0x7413F0
bool CGUnit_C::ProcessLocalMoveEvent(int32_t time, NETMESSAGE msgId, bool needAck, float value, uint32_t index, WGUID transportGuid, uint8_t transportSeat) {
    // this->UpdateObjectEffectMovementStates();

    switch (msgId) {
    case MSG_MOVE_STOP:
    case MSG_MOVE_STOP_STRAFE:
    case MSG_MOVE_START_TURN_LEFT:
    case MSG_MOVE_START_TURN_RIGHT:
    case MSG_MOVE_STOP_TURN:
    case MSG_MOVE_SET_RUN_MODE:
    case MSG_MOVE_SET_WALK_MODE:
    case MSG_MOVE_SET_TURN_RATE_CHEAT:
    case MSG_MOVE_SET_TURN_RATE:
    case MSG_MOVE_TOGGLE_COLLISION_CHEAT:
    case MSG_MOVE_SET_FACING:
    case MSG_MOVE_SET_PITCH_RATE_CHEAT:
    case MSG_MOVE_SET_PITCH_RATE:
        break;

    case MSG_MOVE_START_PITCH_UP:
    case MSG_MOVE_START_PITCH_DOWN:
    case MSG_MOVE_STOP_PITCH:
    case MSG_MOVE_SET_PITCH:
        if ((this->m_passenger->m_flags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) == 0 && (this->m_passenger->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING) == 0)
            return 0;
        break;

    default:
        // if (!IsMovementAckPacket_NeedsMovementStatus(opcode) && this->ukn78()) {
        //     if ((this->m_obj->OBJECT_FIELD_TYPE & 0x10) != 0)
        //         this->ChangeStandState(0);
        // }
        break;
    }

    bool result = false;

    if (needAck) {
        bool handled = false;

        switch (msgId) {
        case MSG_MOVE_START_TURN_LEFT:
        case MSG_MOVE_START_TURN_RIGHT:
        case MSG_MOVE_STOP_TURN:
            if ((this->m_passenger->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING) != 0) {
                // result = this->MoveEventHandler_190(time, opcode);
                handled = true;
            }
            break;

        case MSG_MOVE_START_PITCH_UP:
        case MSG_MOVE_START_PITCH_DOWN:
        case MSG_MOVE_STOP_PITCH:
            if ((this->m_passenger->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING) != 0) {
                // result = this->MoveEventHandler_193(time, opcode);
                handled = true;
            }
            break;

        case MSG_MOVE_SET_FACING:
        case MSG_MOVE_SET_PITCH:
            // handled = this->sub_71AE80();
            break;
        }

        if (!handled) {
             if (this->SendMovementUpdate(time, msgId, value, index, transportGuid, transportSeat))
                 result = true;
            // if (this->sub_721C20(time))
            //     result = true;
        }
    }

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        switch (msgId) {
        case CMSG_FORCE_MOVE_ROOT_ACK:
        case CMSG_FORCE_MOVE_UNROOT_ACK:
        case MSG_MOVE_STOP:
        case MSG_MOVE_STOP_STRAFE:
            if (msgId == CMSG_FORCE_MOVE_ROOT_ACK || msgId == CMSG_FORCE_MOVE_UNROOT_ACK)
                CGInputControl::GetActive()->UpdatePlayer(time, 1);

            if ((this->m_passenger->m_flags & MOVEMASK_ANIMATING) == 0) {
                // this->HandlePendingTrackEvents();
            }
            break;

        case MSG_MOVE_TELEPORT_ACK:
            // CGInputControl::GetActive()->RemoveFlags_0xF0000();
            CGInputControl::GetActive()->UpdatePlayer(time, 1);
            // this->data9BC = OsGetAsyncTimeMs();

            // if (this->IsAutoTracking())
            //     this->ClearTrackingTarget(0, 1);

            if (this->m_obj->m_guid == CGWorldFrame::GetActiveCamera()->m_targetGUID) {
                // CGGameUI::ResetCamera(this->m_obj->m_guid);
            }
            break;

        case CMSG_MOVE_SET_FLY:
            CGInputControl::GetActive()->UpdatePlayer(time, 1);
            break;

        default:
            break;
        }
    }

    this->MoveEventHappened(msgId);
    return result;
}

// OFFSET: 0x71F0C0
bool CGUnit_C::SendMovementUpdate(int32_t time, NETMESSAGE msgId, float value, uint32_t index, WGUID transportGuid, uint8_t transportSeat) {
     //*&this->dataA34[7] = (GetRawFacing)(this);
     //m_passenger = this->m_passenger;
     //if ((m_passenger->m_flags & (MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING)) != 0 || (m_passenger->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING) != 0)
     //    *&this->dataA34[8] = (this->ObjectBase.__vftable[1].PostReenable)(this);

     CDataStore msg = CDataStore();
     if (this->BuildMovementUpdate(time, msgId, &msg, value, index)) {
         if (msgId == CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE) {
             msg << transportGuid;
             msg.Put(transportSeat);
         }

         if ((this->m_passenger->m_flags & (MOVEMENTFLAG_RIGHT | MOVEMENTFLAG_LEFT)) != 0)
             this->unk_0A30 |= 0x4000000u;
         if ((this->m_passenger->m_flags & (MOVEMENTFLAG_PITCH_DOWN | MOVEMENTFLAG_PITCH_UP)) != 0)
             this->unk_0A30 |= 0x8000000u;

         msg.Finalize();
         ClientServices::Send2(&msg);
         this->movementData.UpdateHeartbeatTimerA(time);
         msg.Destroy();
         return true;
     } else {
         msg.Destroy();
         return false;
     }
}

// OFFSET: 0x71EF80
bool CGUnit_C::BuildMovementUpdate(int32_t time, NETMESSAGE msgId, CDataStore* msg, float value, uint32_t index) {
     msg->Put((uint32_t)msgId);
     *msg << this->m_obj->m_guid;
     if (AckMessageNeedsIndex(msgId))
         msg->Put(index);
     if (IsAckMessage(msgId) || this->IsAllowedToSendMessage(msgId)) {
         this->movementData.WriteMovementStatusToPacket(msgId, time, msg);
         if (sub_7151F0(msgId))
             msg->Put(value);
         if ((this->movementData.m_flags & MOVEMENTFLAG_ONTRANSPORT) == 0) {
             if ((this->m_passenger->m_flags & MOVEMENTFLAG_FALLING) != 0) {
                 this->unk_0A30 |= 0x80u;
                 return 1;
             }
             this->unk_0A30 &= ~0x80u;
         }
         return 1;
     }
}

// OFFSET: 0x73C220
void CGUnit_C::SetUpdateInfo(CClientMoveUpdate* moveUpdate, bool localPlayer) {
    this->movementData.SetUpdateInfo(OsGetAsyncTimeMs(), moveUpdate, localPlayer);
    if ((this->movementData.m_flags & MOVEMENTFLAG_FALLING_FAR) != 0)
        this->OnCollideFalling();
}

 // OFFSET: 0x740D30
 bool CGUnit_C::OnMoveEvent(NETMESSAGE msgId, int32_t a3, CDataStore* msg) {
     CMovementStatus status = {};
     status.m_transportSeat = -1;

     *msg >> status;

     int32_t time = OsGetAsyncTimeMs();
     int32_t changed = 0;

     switch (msgId) {
     case MSG_MOVE_START_FORWARD:
         changed = this->movementData.OnMoveStart(time, &status, 1);
         break;
     case MSG_MOVE_START_BACKWARD:
         changed = this->movementData.OnMoveStart(time, &status, 0);
         break;
     case MSG_MOVE_STOP:
         changed = this->movementData.OnMoveStop(time, &status);
         break;

     case MSG_MOVE_START_STRAFE_LEFT:
         changed = this->movementData.OnStrafeStart(time, &status, 1);
         break;
     case MSG_MOVE_START_STRAFE_RIGHT:
         changed = this->movementData.OnStrafeStart(time, &status, 0);
         break;
     case MSG_MOVE_STOP_STRAFE:
         changed = this->movementData.OnStrafeStop(time, &status);
         break;

     case MSG_MOVE_JUMP:
         changed = this->movementData.OnJump(time, &status);
         break;

     case MSG_MOVE_START_TURN_LEFT:
         changed = this->OnTurnStart(time, &status, 1);
         break;
     case MSG_MOVE_START_TURN_RIGHT:
         changed = this->OnTurnStart(time, &status, 0);
         break;
     case MSG_MOVE_STOP_TURN:
         changed = this->movementData.OnTurnStop(time, &status);
         break;

     //case MSG_MOVE_START_PITCH_UP:
     //    changed = this->OnPitchStart(time, &status, 1);
     //    break;
     //case MSG_MOVE_START_PITCH_DOWN:
     //    changed = this->OnPitchStart(time, &status, 0);
     //    break;
     //case MSG_MOVE_STOP_PITCH:
     //    changed = this->movementData.OnPitchStop_1(time, &status);
     //    break;
     //
     //case MSG_MOVE_SET_RUN_MODE:
     //    changed = this->movementData.OnSetRunMode(time, &status, 1);
     //    break;
     //case MSG_MOVE_SET_WALK_MODE:
     //    changed = this->movementData.OnSetRunMode(time, &status, 0);
     //    break;
     //
     //case MSG_MOVE_TELEPORT:
     //    changed = this->OnUnitMoveEvent(time, &status);
     //    break;

     case MSG_MOVE_FALL_LAND:
     case MSG_MOVE_HEARTBEAT:
         changed = this->movementData.OnHeartbeat(time, &status);
         break;

     //case MSG_MOVE_START_SWIM:
     //case MSG_MOVE_START_SWIM_CHEAT:
     //    changed = this->movementData.OnStartSwim(time, &status);
     //    break;
     //case MSG_MOVE_STOP_SWIM:
     //case MSG_MOVE_STOP_SWIM_CHEAT:
     //    changed = this->movementData.OnStopSwim(time, &status);
     //    break;
     //
     //case MSG_MOVE_SET_FACING:
     //    changed = this->movementData.OnSetFacing(time, &status);
     //    break;
     //case MSG_MOVE_SET_PITCH:
     //    changed = this->movementData.OnSetPitch(time, &status);
     //    break;
     //
     //case MSG_MOVE_ROOT:
     //    changed = this->movementData.OnMoveRoot(time, &status);
     //    break;
     //case MSG_MOVE_UNROOT:
     //    changed = this->OnMoveUnRoot(time, &status);
     //    break;
     //
     //case MSG_MOVE_KNOCK_BACK:
     //    changed = this->OnKnockbackPacket(time, &status, msg);
     //    break;
     //
     //case MSG_MOVE_HOVER:
     //    changed = this->movementData.OnMoveHover(time, &status);
     //    break;
     //case MSG_MOVE_FEATHER_FALL:
     //    changed = this->movementData.OnSetFeatherFall(time, &status);
     //    break;
     //case MSG_MOVE_WATER_WALK:
     //    changed = this->movementData.OnSetWaterWalk(time, &status);
     //    break;
     //
     //case MSG_MOVE_UPDATE_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY:
     //    changed = this->movementData.OnUpdateCanTransitionBetweenSwimAndFlyPacket(time, &status);
     //    break;
     //
     //case MSG_MOVE_START_ASCEND:
     //    changed = this->movementData.OnStartAscendOrDescendPacket(time, &status, 1);
     //    break;
     //case MSG_MOVE_START_DESCEND:
     //    changed = this->movementData.OnStartAscendOrDescendPacket(time, &status, 0);
     //    break;
     //case MSG_MOVE_STOP_ASCEND:
     //    changed = this->movementData.OnMoveStopAscendPacket(time, &status);
     //    break;
     //
     //case MSG_MOVE_UPDATE_CAN_FLY:
     //    changed = this->movementData.OnMoveUpdateCanFlyPacket(time, &status);
     //    break;
     //case MSG_MOVE_GRAVITY_CHNG:
     //    changed = this->movementData.OnGravityChangePacket2(time, &status);
     //    break;

     case MSG_MOVE_TOGGLE_COLLISION_CHEAT:
         return 1;

     default:
         return 0;
     }

     if (changed) {
         this->MoveEventHappened(msgId);
         this->UpdateBaseAnimation(0, -1);
     }

     return 1;
}

// OFFSET: 0x73AB20
void CGUnit_C::OnMoveUpdate(int32_t time, bool a3, bool a4) {
    //m_vehicle = this->m_vehicle;
    //if (m_vehicle && *(m_vehicle + 12))
    //    CVehicle_C::UpdateWorldMatrix(m_vehicle);
    this->UpdateWorldObject(0);
    //m_worldObject = this->ObjectBase.m_worldObject;
    //if (!m_worldObject || !World::QueryGroundType(m_worldObject, &this->dataA34[3]))
    //    this->dataA34[3] = -1;
    //CGUnit_C::UpdateFlightStatus(this, a2);
    //CGUnit_C::UpdateSwimmingStatus(&this->ObjectBase, a2, a3);
}

// OFFSET: 0x73ED10
void CGUnit_C::MoveEventHappened(NETMESSAGE msgId) {
    switch (msgId) {
    case MSG_MOVE_START_FORWARD:
    case MSG_MOVE_START_BACKWARD:
    case MSG_MOVE_START_STRAFE_LEFT:
    case MSG_MOVE_START_STRAFE_RIGHT:
    case MSG_MOVE_START_SWIM:
    case MSG_MOVE_START_SWIM_CHEAT:
    case MSG_MOVE_START_ASCEND:
    case MSG_MOVE_START_DESCEND:
        //this->CancelRangedMode();
        this->UpdateBaseAnimation(0, -1);
        return;

    case MSG_MOVE_STOP:
    case MSG_MOVE_STOP_STRAFE:
    case MSG_MOVE_START_TURN_LEFT:
    case MSG_MOVE_START_TURN_RIGHT:
    case MSG_MOVE_STOP_TURN:
    case MSG_MOVE_STOP_SWIM:
    case MSG_MOVE_STOP_SWIM_CHEAT:
    case MSG_MOVE_STOP_ASCEND:
    case MSG_MOVE_TELEPORT:
    case MSG_MOVE_TELEPORT_ACK:
    case MSG_MOVE_TOGGLE_COLLISION_CHEAT:
    case MSG_MOVE_UPDATE_CAN_FLY:
    case MSG_MOVE_ROOT:
    case SMSG_SPLINE_MOVE_ROOT:
    case CMSG_FORCE_MOVE_ROOT_ACK:
    case CMSG_MOVE_GRAVITY_DISABLE_ACK:
    case CMSG_MOVE_GRAVITY_ENABLE_ACK:
    case SMSG_SPLINE_MOVE_GRAVITY_DISABLE:
    case SMSG_SPLINE_MOVE_GRAVITY_ENABLE:
        this->UpdateBaseAnimation(0, -1);
        return;

    case MSG_MOVE_SET_RUN_MODE:
    case MSG_MOVE_SET_WALK_MODE:
    case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:
    case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:
    case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:
    case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:
    case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:
    case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:
    case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:
        if ((this->m_passenger->m_flags & 0xC0100F) != 0) {
            this->UpdateBaseAnimation(0, -1);
        }
        return;

    case MSG_MOVE_JUMP:
        //this->CancelRangedMode();
        //this->PlayUnitSound(11, 1);
        //
        //if (!this->GetVehicleRecPtr() || !this->m_vehicle->Sub7571C0()) {
            this->PlayBaseAnimation(ANIM_JUMP_START, 0);
        //}
        return;

    case CMSG_MOVE_KNOCK_BACK_ACK:
        //if (!this->GetVehicleRecPtr() || !this->m_vehicle->Sub7571C0()) {
            this->PlayBaseAnimation(ANIM_FALL, 0);
        //}
        return;

    case CMSG_MOVE_SET_CAN_FLY_ACK:
        if ((this->movementData.m_flags & 0x1000000) == 0) {
            this->m_animationState &= ~0x800000u;
        }
        return;

    case CMSG_MOVE_SET_FLY:
        //if ((this->movementData.m_flags & 0x2000000) != 0) {
        //    this->Sub715810();
        //
        //    if (this->GetCurrentTorsoAnimId() == 40 || this->GetMountBoneSequenceId() == 40) {
        //        this->UpdateBaseAnimation(0, -1);
        //        return;
        //    }
        //
        //    this->PlayUnitSound(11, 1);
        //
        //    if (!this->GetVehicleRecPtr() || !this->m_vehicle->Sub7571C0()) {
        //        this->PlayBaseAnimation(ANIM_JUMP_START, 0);
        //    }
        //
        //    this->m_animationState |= 0x800000u;
        //} else {
        //    this->PlayUnitSound(12, 1);
        //
        //    if (!this->GetVehicleRecPtr() || !this->m_vehicle->Sub7571C0()) {
        //        this->PlayBaseAnimation(ANIM_JUMP_LAND_RUN, 0);
        //    }
        //
        //    this->m_animationState &= ~0x800000u;
        //}
        return;

    default:
        return;
    }
}

// OFFSET: 0x718890
bool CGUnit_C::OnTurnStart(int32_t eventTime, CMovementStatus* update, bool left) {
    if ((this->m_unit->UNIT_FIELD_FLAGS & 0x40000) != 0)
        return 0;

    return this->movementData.OnTurnStart(eventTime, update, left);
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

// OFFSET: 0x72EA50
void CGUnit_C::OnSetRawFacingLocal(int32_t eventTime, float facing) {
    // if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::m_trackingType != 13 && (CGUnit_C::s_trackingFlags & 1) == 0) {
    //     this->ClearTrackingTarget(this->m_obj->m_guid, 0, 1);
    // }

    this->movementData.OnSetRawFacingLocal(eventTime, facing);

    // if (this->GetStandState() == 3 && (this->m_obj->m_type & 0x10) != 0) {
    //     static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
    // }
}

// OFFSET: 0x72D3F0
void CGUnit_C::OnTurnToAngleLocal(int32_t eventTime, float facing) {
    // if (this->m_obj->m_guid == CGUnit_C::s_activeMover && CGUnit_C::m_trackingType != 13 && (CGUnit_C::s_trackingFlags & 1) == 0) {
    //     this->ClearTrackingTarget(this->m_obj->m_guid, 0, 1);
    // }

    this->movementData.OnTurnToAngleLocal(eventTime, facing);

    // if (this->GetStandState() == 3 && (this->m_obj->m_type & 0x10) != 0) {
    //     static_cast<CGPlayer_C*>(this)->ChangeStandState(0);
    // }
}

// OFFSET: 0x73D3D0
void CGUnit_C::OnCollideFallLand(uint32_t prevFlags, int32_t fellWithSpeed) {
    this->PlayFallLandAnimation(prevFlags, fellWithSpeed);

    if (!this->IsClientControlled()) {
        return;
    }

    //if (this->movementData.IsSplineFlyer_IsNotFlyingFeatherFalling()) {
    //    return;
    //}
    //
    //float distanceFallen = this->movementData.GetDistanceFallen();
    //
    //if (distanceFallen >= 70.0f || (distanceFallen > 13.0f && !this->Ukn74())) {
    //    if ((this->m_obj->m_type & TYPEMASK_PLAYER) == 0 || ((this->m_player->PLAYER_FLAGS & 0x4000) == 0 && static_cast<int32_t>(this->m_unit->UNIT_FIELD_HEALTH) > 0)) {
    //        this->PlayUnitSound(13, 1);
    //    }
    //
    //    this->HandleEnvironmentDamage(2, 0, 0, 0);
    //}
}

// OFFSET: 0x73D4A0
bool CGUnit_C::OnCollideFallLandNotify(uint32_t time, uint32_t prevFlags, uint32_t prevFlags2, int32_t wasFalling) {
    this->OnCollideFallLand(prevFlags, wasFalling);
    //v6 = 0;
    //if ((SLOBYTE(this->unk_0A30) < 0 || CMovementShared::GetDistanceFallen(&this->movementData) > 0.027777778) && CGUnit_C::SendMovementUpdate(this, a2, MSG_MOVE_FALL_LAND, 0.0, 0, 0i64, 255))
    //    v6 = 1;
    //unk_0A30 = this->unk_0A30;
    //if ((unk_0A30 & 0x100000) != 0) {
    //    this->unk_0A30 = unk_0A30 & 0xFFEFFFFF;
    //    CGPlayer_C::HandleRepopRequest(this, 1);
    //}
    //return v6;
    return false;
}

// OFFSET: 0x73AD00
void CGUnit_C::OnCollideFalling() {
    if ((this->movementData.m_flags & MOVEMENTFLAG_FALLING_FAR) != 0 && this->m_unit->UNIT_FIELD_HEALTH > 0) {
        //if (!this->m_vehicle || !this->m_vehicle[3] || !this->m_vehicle->sub_7571C0()) {
        //    if (!this->m_vehiclePassenger || !*(this->m_vehiclePassenger + 20))
                this->PlayBaseAnimation(ANIM_FALL, 0);
        //}
    }
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
            this->UpdateBaseAnimation(0, -1);
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

                haveSpline = this->movementData.OnSpline(points, n, durationMs, splineFlags, moveTicks) != 0;
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

    this->UpdateBaseAnimation(0, -1);
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

// OFFSET: 0x74B900
bool CGUnit_C::IsVehiclePreventingTurning() {
    // guid_high = this->movementData.transportGuid.guid_high;
    // guid_low = this->movementData.transportGuid.guid_low;
    // return ((guid_high & 0xF0F00000) == -263192576
    //     || (guid_high & 0xF0000000) == 0 && guid_high & 0xF07FFFFF | guid_low)
    //     && (v5 = ClntObjMgrObjectPtr(__PAIR64__(guid_high, guid_low), TYPEMASK_UNIT)) != 0
    //     && (v6 = v5->dataF00[23]) != 0
    //     && (VehicleSeatRec = CVehicle_C::GetVehicleSeatRec(v6, BYTE2(this->movementData.m_flags2))) != 0
    //     && (*(VehicleSeatRec + 4) & 0x400) == 0;
    return false;
}

// OFFSET: 0x74B8B0
bool CGUnit_C::HasVehicleTransport() {
    //guid_high = this->movementData.transportGuid.guid_high;
    //return (guid_high & 0xF0F00000) == 0xF0500000 || (guid_high & 0xF0000000) == 0 && guid_high & 0xF07FFFFF | this->movementData.transportGuid.guid_low;
    return false;
}

// OFFSET: 0x74BA40
bool CGUnit_C::IsAlteredFormTransitionPreventingMovement() {
    if (!this->IsLocalClientControlled())
        return 0;
    //dataF60 = this->dataF60;
    //if (dataF60) {
    //    v4 = *(dataF60 + 20);
    //    if (v4) {
    //        if (v4 != 3)
    //            return 1;
    //    }
    //}
    //if (dataF60 && (*(dataF60 + 16) & 0x200) != 0)
    //    return 1;
    auto v5 = ClntObjMgrObjectPtr<CGUnit_C*>(this->m_unit->UNIT_FIELD_CHARMEDBY, TYPEMASK_UNIT);
    if (v5) {
        //v6 = v5->dataF60;
        //if (!v6 || ((v7 = *(v6 + 20)) == 0 || v7 == 3) && (*(v6 + 16) & 0x200) == 0)
        //    return CGUnit_C::GetUnitF58Field_14_4F03C0(v5) && (*(v6 + 16) & 8) != 0;
        //return 1;
    }
    if ((this->m_unit->UNIT_FIELD_FLAGS & 0x1000000) == 0 && this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return 1;
    }
    return 0;
}

// OFFSET: 0x71C1E0
bool CGUnit_C::ClampRawAngleToLegalFacingRange(float* yaw) {
    //if (!CMovement_C::ComputeLegalRawFacingRange(&this->movementData, &v6, &v5))
    //    return 0;
    //v2 = a2;
    //a2 = *a2;
    //bn_CMovement_C_WrapFacingToRange(&a2, v6, v5);
    //v3 = v6;
    //if (*&a2 >= v6) {
    //    v3 = v5;
    //    if (v5 >= *&a2)
    //        return 0;
    //}
    //*v2 = v3;
    return false;
}

// OFFSET: 0x719660
void CGUnit_C::SmoothFacingAngle(float target) {
    float ref = this->m_targetFacing;

    if (ref + 3.1415927f >= this->m_renderFacing) {
        if (ref - 3.1415927f > this->m_renderFacing)
            ref -= 6.2831855f;
    } else {
        ref += 6.2831855f;
    }

    if (this->m_renderFacing + 1.5704823f >= ref) {
        if (this->m_renderFacing - 1.5704823f > ref)
            this->m_renderFacing = ref + 1.5704823f;
    } else {
        this->m_renderFacing = ref - 1.5704823f;
    }

    this->m_renderFacing = CMath::normalizeAnglePi(this->m_renderFacing);

    float goal = target;

    if (target + 3.1415927f >= this->m_renderFacing) {
        if (target - 3.1415927f > this->m_renderFacing)
            goal = target - 6.2831855f;
    } else {
        goal = target + 6.2831855f;
    }

    float elapsed = CGWorldFrame::s_currentWorldFrame->m_elapsedSec;
    float x = elapsed * 20.0f;
    float damping = 1.0f / (x + x * x * x * 0.235f + x * x * 0.47999999f + 1.0f);

    float offset = this->m_renderFacing - goal;
    float step = elapsed * (offset * 20.0f + this->m_facingVelocity);

    this->m_renderFacing = (offset + step) * damping + goal;
    this->m_facingVelocity = damping * (this->m_facingVelocity - 20.0f * step);
    this->m_renderFacing = goal * (1.0f - this->m_facingBlend) + this->m_renderFacing * this->m_facingBlend;
}

// OFFSET: 0x735F60
void CGUnit_C::UpdateSmoothFacing(float* facingOffset) {
    float facing = this->movementData.m_facing;
    float target = facing;

    if (this->m_obj->m_guid == CGUnit_C::s_activeMover) {
        this->m_targetFacing = facing;
        //this->m_facingUnkAA4 = 0.0f;

        if ((this->movementData.m_flags & 0x30) != 0 || CGInputControl::GetActive()->CameraCanTurnPlayer()) {
            this->m_animationState |= 1;
        } else {
            this->m_animationState &= ~1u;
            this->m_lastTurnTimeMs = OsGetAsyncTimeMs();
        }

        if ((this->movementData.m_flags & 0x30) != 0 /* || CGInputControl::GetActive()->Sub5FA420() */) {
            this->m_animationState |= 2;
        } else {
            this->m_animationState &= ~2u;
        }

        //uint32_t vehicle = this->m_vehicle;
        //
        //if (vehicle && vehicle->unk_000C) {
        //    float propagated = facingOffset
        //                           ? this->m_targetFacing + *facingOffset
        //                           : this->GetSmoothFacing();
        //    this->PropagateToPassengers(propagated);
        //}

        return;
    }

    bool emoteBlocksFacing = false;
    uint32_t emoteState = this->m_unit->UNIT_NPC_EMOTESTATE;

    if (emoteState) {
        EmotesRec* emote = g_emotesDB.GetRecord(emoteState);
        if (emote && (emote->m_emoteFlags & 0x2000) == 0) {
            emoteBlocksFacing = true;
        }
    }

    if ((this->m_passenger->m_flags & 0xF) == 0) {
        if (this->GetClientStandState() || emoteBlocksFacing) {
            goto applyFacing;
        }

        uint32_t unitFlags = this->m_unit->UNIT_FIELD_FLAGS;

        if ((unitFlags & 0x40000) != 0 || (this->m_unit->UNIT_FIELD_FLAGS_2 & 0x8000) != 0) {
            goto applyFacing;
        }

        if ((this->unk_0A30 & 1) != 0) {
            facing = this->m_scriptedFacing;
            target = this->m_scriptedFacing;
        } else {
            // if ((unitFlags & 0x1000000) != 0
            //    || (this->m_obj->m_type & 0x10) != 0
            //    || !this->Sub722640()
            //    || (this->HasVehicleTransport()
            //        && (v17 = ClntObjMgrObjectPtr<CGUnit_C*>(this->GetTransportGUID(), TYPEMASK_UNIT)) != nullptr
            //        && (seatRec = v17->GetVehicleSeatRec(BYTE2(this->movementData.m_flags2))) != nullptr
            //        && (seatRec->flags & 0x400) == 0)) {
            //    goto applyFacing;
            //}
            //
            // WGUID faceTarget;
            // if (this->GetVehicleRecPtr() && (this->m_vehicle->targetGuid)) {
            //    faceTarget = this->m_vehicle->targetGuid;
            //} else {
            //    if (this->m_unit->UNIT_CHANNEL_SPELL && (this->unk_0A30 & 0x8000) != 0 && !this->IsClientControlled())
            //        faceTarget = this->m_unit->UNIT_FIELD_CHANNEL_OBJECT;
            //    else
            //        faceTarget = this->m_unit->UNIT_FIELD_TARGET;
            //
            //    if (!faceTarget)
            //        faceTarget = this->GetComboPointTarget();
            //}
            //
            // if (!faceTarget) {
            //    if (CGGameUI::m_interactTarget != this->m_obj->m_guid)
            //        goto applyFacing;
            //    faceTarget = ClntObjMgrGetActivePlayer();
            //    if (!faceTarget)
            //        goto applyFacing;
            //}
            //
            // CGUnit_C* other = ClntObjMgrObjectPtr<CGUnit_C*>(faceTarget, TYPEMASK_UNIT);
            // if (!other)
            //    goto applyFacing;
            //
            // C3Vector otherPos;
            // other->GetPosition(otherPos);
            // float toTarget = CalculateFacingTo(this->GetPosition(), &otherPos);
            //
            // if (facingOffset) {
            //    facing = CMath::normalizeangle0to2pi(toTarget - *facingOffset);
            //} else if (!this->HasVehicleTransport()) {
            //    facing = this->GetTransportGUID()
            //        ? CMath::normalizeangle0to2pi(toTarget - MovementGetTransportFacing(this->GetTransportGUID()))
            //        : CMath::normalizeangle0to2pi(toTarget);
            //} else {
            //    CGUnit_C* carrier = ClntObjMgrObjectPtr<CGUnit_C*>(this->GetTransportGUID(), TYPEMASK_UNIT);
            //    facing = carrier
            //        ? CMath::normalizeangle0to2pi(toTarget - carrier->GetSmoothFacing())
            //        : CMath::normalizeangle0to2pi(target);
            //}
            // target = facing;
        }
    }

applyFacing:
    float eased;

    CreatureMovementInfoRec* movementInfo = nullptr;
    // uint32_t movementId = this->m_creatureCacheEntry ? this->m_creatureCacheEntry->MovementId : 0;
    // movementInfo = g_CreatureMovementInfoDB.GetRecord(movementId);

    if (movementInfo && movementInfo->m_smoothFacingChaseRate > 0.0000099999997f) {
        if (facing + 3.1415927f >= this->m_targetFacing) {
            if (facing - 3.1415927f > this->m_targetFacing)
                target = facing - 6.2831855f;
        } else {
            target = facing + 6.2831855f;
        }

        //this->Sub7160B0(&this->m_targetFacing, &target, movementInfo->m_smoothFacingChaseRate, CGWorldFrame::s_currentWorldFrame->m_elapsedSec);
        eased = CMath::normalizeangle0to2pi(this->m_targetFacing);
    } else if ((this->m_unit->UNIT_FIELD_FLAGS & 8) != 0 && (this->m_obj->m_type & 0x10) == 0) {
        if (facing + 3.1415927f >= this->m_targetFacing) {
            if (facing - 3.1415927f > this->m_targetFacing)
                target = facing - 6.2831855f;
        } else {
            target = facing + 6.2831855f;
        }

        float rate = 20.0f;
        // rate = this->IsInCombat() ? 30.0f : 20.0f;

        //this->Sub7160B0(&this->m_targetFacing, &target, rate, CGWorldFrame::s_currentWorldFrame->m_elapsedSec);
        eased = CMath::normalizeangle0to2pi(this->m_targetFacing);
    } else {
        float delta = facing - this->m_targetFacing;

        if (delta > 3.1415927f) {
            delta -= 6.2831855f;
        } else if (delta < -3.1415927f) {
            delta += 6.2831855f;
        }

        if (fabsf(delta) <= 0.0099999998f) {
            this->m_turnDelta[0] = 0.0f;
            this->m_targetFacing = facing;

            //uint32_t vehicle = this->m_vehicle;
            //if (vehicle && vehicle->unk_000C) {
            //    float propagated = facingOffset
            //                           ? facing + *facingOffset
            //                           : this->GetSmoothFacing();
            //    this->PropagateToPassengers(propagated);
            //}
            return;
        }

        float step = delta;

        if ((delta >= 0.0f) != (this->m_turnDelta[0] >= 0.0f)) {
            this->m_turnDelta[0] = 0.0f;
        }
        
        if (this->m_turnDelta[0] == 0.0f) {
            this->m_turnDelta[0] = delta;
            this->m_turnDelta[1] = delta;
            this->m_turnDelta[2] = delta;
            this->m_turnDelta[3] = delta;
        } else {
            memmove(&this->m_turnDelta[1], &this->m_turnDelta[0], 3 * sizeof(float));
            this->m_turnDelta[0] = delta;
        
            float average = (this->m_turnDelta[0] + this->m_turnDelta[1] + this->m_turnDelta[2] + this->m_turnDelta[3]) * 0.25f;
        
            if (step <= 0.0f) {
                if (average >= step)
                    step = average;
            } else if (average <= step) {
                step = average;
            }
        }

        eased = CMath::normalizeangle0to2pi(step * 0.5f + this->m_targetFacing);
    }

    //uint32_t vehicle = this->m_vehicle;
    this->m_targetFacing = eased;

    //if (vehicle && vehicle->unk_000C) {
    //    if (facingOffset) {
    //        this->PropagateToPassengers(this->m_targetFacing + *facingOffset);
    //    } else {
    //        this->PropagateToPassengers(this->GetSmoothFacing());
    //    }
    //}
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

// OFFSET: 0x73E840
void CGUnit_C::ModelLoaded(CM2Model* model) {
    CGObject_C::ModelLoaded(model);

    if (this->GetTransportGUID()) {
        C44Matrix transform;
        MovementGetTransportMtxX(this->GetTransportGUID(), &transform);

        if (model->m_loaded) {
            // C44Matrix::Copy(&model->unk_0134, &transform);
            // model->ChangeFrameOfReference(&model->unk_0134);
        }
    }

    if (model == this->m_worldModel) {
        this->m_animationState &= 0xFFFFFE7F;

        if (model->HasKeyBone(4)) {
            this->m_animationState |= 0x80;
        }

        if (model->HasKeyBone(6)) {
            this->m_animationState |= 0x100;
        }

        if ((this->m_animationState & 0x80) != 0) {
            this->m_torsoKeyBone = 4;
        } else {
            this->m_torsoKeyBone = (this->m_animationState & 0x100) != 0 ? 6 : -1;
        }

        // this->UpdateInteractIconAttach();
    } else if (model == this->data98C /*&& !model->HasAttachment(0)*/) {
        // SysMsgPrintf_0(2, 16, "MOUNTDISPLAYIDNOMOUNTATTACHMENT|%d", this->data9C0);
    }

    if ((this->m_modelFlags & 0x40000) != 0) {
        this->UpdateBaseAnimation(1, -1);

        ANIMATION_ID torsoAnim = this->GetCurrentTorsoAnimId();
        AnimationDataRec* row = g_animationDataDB.GetRecord(torsoAnim);

        if (row && row->m_behaviorID == 127) {
            //uint8_t saved = BYTE2(this->ukn_00C8);
            //this->ukn_00C4 = 0;
            //LOBYTE(this->ukn_00C8) = saved;
        }

        // this->UpdateObjectEffectAnimationStates();
    }

    if (model == this->GetObjectModel()) {
        // this->InitWheels();
    }

    // if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_seatState == 3) {
    //     CGUnit_C* carrier = ClntObjMgrObjectPtr<CGUnit_C*>(this->GetTransportGUID(), TYPEMASK_UNIT);
    //     if (carrier) {
    //         if (carrier->m_vehicle && carrier->m_vehicle->unk_000C)
    //             carrier->m_vehicle->UpdateLargestPassengerBoundsRadius();
    //         carrier->UpdateWorldObject();
    //     }
    // }

    // if (this->dataA44[18]) {
    //     if (this->HasAuraBySpellId(this->dataA44[18])) {
    //         SpellRec spell;
    //         if (ClientDb::GetLocalizedRow(&g_spellDB, this->dataA44[18], &spell)) {
    //             auto visual = GetSpellVisual(&spell);
    //             if (visual) {
    //                 auto kit = ClientDB::GetRow(&g_spellVisualKitDB, visual->kitId);
    //                 if (kit)
    //                     this->PlaySpellVisualKit(PlaySpellVisualKitData(&spell, kit, 2));
    //             }
    //         }
    //     }
    //     this->dataA44[18] = 0;
    // }

    // if (model == this->GetObjectModel()) {
    //     if (this->m_vehicle && this->m_vehicle->unk_000C && this->m_vehicle->ShouldMirrorAnimations())
    //         this->m_vehicle->StartMirroringAnimsToPassengers();
    //     if (this->m_vehiclePassenger && this->m_vehiclePassenger->OverridesModelAnimation())
    //         CAnimKitManager::MirrorToModel(this->m_vehiclePassenger);
    // }
}

// OFFSET: 0x73DAB0
void CGUnit_C::PreAnimate(CGWorldFrame* worldFrame) {
    uint32_t time = FrameTime::s_curTimeMs;

    //if (this->m_vehiclePassenger) {
    //    this->m_vehiclePassenger->CheckForVehicleTransitionAnimTimeout(time);
    //}

    //if (static_cast<int32_t>(time - this->data9BC) >= 0) {
    //    this->UpdateBreathState(time);
    //}

    //if (CGGameUI::m_lockedTarget == this->m_obj->m_guid) {
    //    CGUnit_C* player = ClntObjMgrObjectPtr<CGUnit_C*>(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER);
    //
    //    if (player && ((player->movementData.m_flags2 & 2) != 0 || player->dataA20) && player->CanAttack(this)) {
    //        uint32_t toggledAt = dword_CA12C0;
    //        this->unk_0A30 |= 0x10;
    //
    //        if (static_cast<int32_t>(time - toggledAt - 500) >= 0) {
    //            dword_CA12BC = (dword_CA12BC == 0);
    //            toggledAt = time;
    //            dword_CA12C0 = time;
    //        }
    //
    //        float phase = (toggledAt - time + 500) * 0.0020000001f;
    //        if (dword_CA12BC) {
    //            phase = 1.0f - phase;
    //        }
    //
    //        reinterpret_cast<uint8_t*>(&dword_ADAA98)[1] = static_cast<uint8_t>(phase * 128.0f);
    //        PlayerNameTriggerColorUpdate(this->m_nameDesc);
    //    } else if ((this->unk_0A30 & 0x10) != 0) {
    //        this->unk_0A30 &= 0xFFFFFFEF;
    //        PlayerNameTriggerColorUpdate(this->m_nameDesc);
    //    }
    //} else if ((this->unk_0A30 & 0x10) != 0) {
    //    this->unk_0A30 &= 0xFFFFFFEF;
    //    PlayerNameTriggerColorUpdate(this->m_nameDesc);
    //}

    //PLAYERNAMEDESC_UpdateVisibility(this->m_nameDesc);
    //this->Sub720DB0(time);

    //if (this->m_fadeDelayMs && static_cast<int32_t>(time - this->m_fadeStartMs - this->m_fadeDelayMs) >= 0) {
    //    uint32_t fadeArg = this->m_fadeArg;
    //    float alpha = this->GetFadeAlpha(fadeArg);
    //    this->DoFade(alpha, fadeArg);
    //    this->m_fadeDelayMs = 0;
    //}

    CGObject_C::PreAnimate(worldFrame);

    //while (this->data9F0) {
    //    CMissile* missile = this->data9F0;
    //    if (static_cast<int32_t>(time - missile->m_expireTime) < 0) {
    //        break;
    //    }
    //    missile->DeleteSelf();
    //}

    //for (CMissile* missile = this->data9F0; missile; missile = missile->m_next) {
    //    float t = (missile->m_fadeEndTime - time) * 0.00050000002f;
    //    float weight;
    //
    //    if (t >= 0.0f) {
    //        weight = (t <= 1.0f) ? t * (t * t) : 1.0f;
    //    } else {
    //        weight = 0.0f;
    //    }
    //
    //    if (missile->m_model) {
    //        missile->m_model->m_fadeWeight = weight;
    //    }
    //}

    //this->UpdateProceduralBoneAnimation();
    //this->CreateRipple(0);

    uint32_t vehicleSeatFlags = 0; //this->m_vehicle ? this->m_vehicle->unk_000C : 0;
    uint32_t mirrorFlag = 0;
    bool vehicleOverridesFacing = false;

    if (vehicleSeatFlags) {
        uint32_t seatFlags = *reinterpret_cast<uint32_t*>(vehicleSeatFlags + 4);
        mirrorFlag = seatFlags & 0x200;
        if ((seatFlags & 0x200) != 0 || (seatFlags & 0x1000) != 0) {
            vehicleOverridesFacing = true;
        }
    }

    uint32_t passengerFlags = this->m_passenger->m_flags;
    uint32_t seatState = 0; //this->m_vehiclePassenger ? this->m_vehiclePassenger->m_seatState : 0;

    if (static_cast<int32_t>(this->m_unit->UNIT_FIELD_HEALTH) <= 0 /*|| (this->m_vehiclePassenger && (seatState == 2 || seatState == 5))*/ || (passengerFlags & 0x2200000) != 0 || vehicleOverridesFacing || (this->m_animationState & 0x180) == 0) {
        this->m_renderFacing = this->m_targetFacing;
        this->m_facingVelocity = 0.0f;
        this->m_facingBlend = 0.0f;
    } else if ((passengerFlags & 0xC) != 0) {
        float offset = ((this->movementData.m_flags & 3) != 0) ? 0.78539819f : 1.5707964f;

        uint32_t dir = this->movementData.m_flags & 6;
        if (dir == 6 || dir == 0) {
            offset = -offset;
        }

        this->m_facingBlend = 1.0f;

        float delta = CMath::normalizeAnglePi(offset + this->m_targetFacing - this->m_renderFacing);
        this->SmoothFacingAngle(CMath::normalizeAnglePi(delta + this->m_renderFacing));
    } else if ((passengerFlags & 0x1003) == 0) {
        if (this->m_facingBlend < 1.0f) {
            this->m_facingBlend = worldFrame->m_elapsedSec * 2.5f + this->m_facingBlend;
        }
    } else if (this->m_facingBlend <= 0.0f) {
        this->m_renderFacing = this->m_targetFacing;
        this->m_facingVelocity = 0.0f;
    } else {
        this->m_facingBlend -= worldFrame->m_elapsedSec * 2.5f;
        if (this->m_facingBlend > 1.0f) {
            this->m_facingBlend = 1.0f;
        }
        this->SmoothFacingAngle(this->m_targetFacing);
    }

    float lag = CMath::normalizeAnglePi(this->m_targetFacing - this->m_renderFacing);
    float absLag = fabsf(lag);
    float torsoTwist = 0.0f;
    float headTwist = 0.0f;

    if (absLag < 0.001f) {
        this->m_worldModel->SetBoneFlags(4, 0, 128);
        this->m_worldModel->SetBoneFlags(6, 0, 128);
        //this->RotateWheels();
        this->m_animationState &= 0xFFFFE7FF;
    } else {
        if (absLag > 1.5707964f) {
            torsoTwist = copysignf(absLag - 1.5707964f, lag);
            headTwist = torsoTwist;
        }

        if ((this->m_passenger->m_flags & 0xC) == 0 && (this->m_animationState & 1) == 0) {
            int32_t sinceTurn = OsGetAsyncTimeMs() - this->m_lastTurnTimeMs;
            float step = sinceTurn * 0.001f * this->movementData.m_turnRate * 8.0f;

            if (absLag < step) {
                step = absLag;
            }

            headTwist = copysignf(step, lag) + torsoTwist;
        }

        this->m_renderFacing = CMath::normalizeAnglePi(headTwist + this->m_renderFacing);

        float residual = CMath::normalizeAnglePi(this->m_targetFacing - this->m_renderFacing);
        float absResidual = fabsf(residual);

        if (absResidual >= 0.0000099999997f) {
            if (!this->data98C && (this->m_animationState & 0x80) != 0) {
                float headAngle = absResidual;

                if (this->m_obj->m_guid != CGUnit_C::s_activeMover || CGUnit_C::m_trackingType == 13) {
                    headAngle *= 0.5f;
                }

                if (headAngle > 0.78539819f) {
                    headAngle = 0.78539819f;
                }

                this->m_worldModel->SetBoneFlags(4, 128, 128);
                this->m_worldModel->SetBoneProceduralTransform(4, &C44Matrix::Rotation(copysignf(headAngle, residual), C3Vector(0.0f, 0.0f, 1.0f), true));

                absResidual -= headAngle;
            }

            if ((this->m_animationState & 0x100) != 0) {
                if (absResidual >= 0.78539819f) {
                    absResidual = 0.78539819f;
                }

                this->m_worldModel->SetBoneFlags(6, 128, 128);
                this->m_worldModel->SetBoneProceduralTransform(6, &C44Matrix::Rotation(copysignf(absResidual, residual), C3Vector(0.0f, 0.0f, 1.0f), true));
            }
        } else {
            this->m_worldModel->SetBoneFlags(4, 0, 128);
            this->m_worldModel->SetBoneFlags(6, 0, 128);
        }

        if (mirrorFlag) {
            this->m_worldModel->SetBoneFlags(4, 128, 128);
            this->m_worldModel->SetBoneProceduralTransform(4, &C44Matrix::Rotation(-this->GetPitch(), C3Vector(0.0f, 1.0f, 0.0f), true));
        }

        //this->RotateWheels();
        this->m_animationState &= 0xFFFFE7FF;
    }

    if ((this->movementData.m_flags & 0x2E0100F) == 0) {
        if ((this->m_obj->m_guid != ClntObjMgrGetActivePlayer() || this->AsPlayer()->m_lootTarget == 0) && !this->GetClientStandState()) {
            if (headTwist > 0.0000099999997f) {
                this->m_animationState |= 0x800;
            } else if (headTwist < -0.0000099999997f) {
                this->m_animationState |= 0x1000;
            }

            uint32_t current = this->GetObjectModel()->GetBoneSequenceId(-1);
            uint32_t passengerFlags = this->m_passenger->m_flags;
            uint32_t wanted;

            if ((passengerFlags & 0x10) != 0 || (this->m_animationState & 0x800) != 0) {
                wanted = 11;
            } else if ((passengerFlags & 0x20) != 0 || (this->m_animationState & 0x1000) != 0) {
                wanted = 12;
            } else if (current != 11 && current != 12) {
                wanted = current;
            } else {
                wanted = 0;
            }

            if (current != wanted) {
                //this->ShouldPlayTurnInPlaceAnim();
                this->UpdateBaseAnimation(0, -1);
            }
        }
    }

    CM2Model* model = this->m_worldModel;
    if (model) {
        uint32_t bit = ((this->m_unit->UNIT_FIELD_FLAGS_2 >> 1) & 1) << 14;
        model->f_flags ^= (model->f_flags ^ bit) & 0x4000;
    }

    //this->UpdateDelayedSpellVisualKits();
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

// OFFSET: 0x7156A0
float CGUnit_C::GetRenderFacing() {
    return this->movementData.GetFacing(this->m_renderFacing);
}

// OFFSET: 0x71A390
bool CGUnit_C::CanHighlight() {
    if ((this->m_unit->UNIT_FIELD_FLAGS & 0x2000000) != 0) {
        if (this->m_unit->UNIT_FIELD_CREATEDBY != ClntObjMgrGetActivePlayer())
            return false;
        //m_obj = this->m_obj;
        //guid_low = m_obj->m_guid.guid_low;
        //guid_high = m_obj->m_guid.guid_high;
        //if (__PAIR64__(guid_high, guid_low) != CGPetInfo__GetPet(0))
        //    return 0;
    }
    return true;
}

// OFFSET: 0x6E6EC0
bool CGUnit_C::CanBeTargetted() {
    return this->CanHighlight();
}

// OFFSET: 0x6E6EE0
char* CGUnit_C::GetObjectName() {
    return this->GetUnitName(nullptr, 1);
}

// OFFSET: 0x653A10 (NOP)
void CGUnit_C::GetAFKText(char* text, uint32_t textLength) {

}

// OFFSET: 0x653A10 (NOP)
void CGUnit_C::GetDNDText(char* text, uint32_t textLength) {

}

// OFFSET: 0x653A10 (NOP)
void CGUnit_C::GetGMText(char* text, uint32_t textLength) {

}

// OFFSET: 0x653A10 (NOP)
void CGUnit_C::GetDevText(char* text, uint32_t textLength) {

}

// OFFSET: 0x6E6FC0
float CGUnit_C::GetPitch() {
    return this->movementData.m_pitch;
}

// OFFSET: 0x71F440
CGUnitVirtualItem* CGUnit_C::GetVirtualItem(uint8_t a2, uint32_t a3) {
    if (!this->GetVirtualItemDisplayID(a2)) {
        return nullptr;
    }

    if (a3) {
        return &this->m_virtualItem[a2];
    }

    if (a2) {
        bool visible;

        if (a2 == 1) {
            if ((this->m_unit->UNIT_FIELD_FLAGS & 0x200000) != 0) {
                //this->GetVirtualItem(0, 1); // Dead call?

                if (!this->IsDisarmed(0)) {
                    CGUnitVirtualItem* offHand = this->GetVirtualItem(1, 1);

                    if (offHand) {
                        if (offHand->classID == 2) {
                            return nullptr;
                        }
                    }
                }
            }

            visible = (this->m_unit->UNIT_FIELD_FLAGS_2 & 0x80) == 0;
        } else {
            if (a2 != 2) {
                return &this->m_virtualItem[a2];
            }

            visible = (this->m_unit->UNIT_FIELD_FLAGS_2 & 0x400) == 0;
        }

        if (!visible) {
            return nullptr;
        }
    } else if ((this->m_unit->UNIT_FIELD_FLAGS & 0x200000) != 0) {
        CGUnitVirtualItem* mainHand = this->GetVirtualItem(0, 1);

        if (mainHand) {
            if (mainHand->classID != 2) {
                return &this->m_virtualItem[0];
            }

            return nullptr;
        }
    }

    return &this->m_virtualItem[a2];
}

// OFFSET: 0x71F540
uint32_t CGUnit_C::GetVirtualItemDisplayRec(uint8_t a2, ItemDisplayInfoRec* rec) {
    return g_itemDisplayInfoDB.GetRecord(this->m_virtualItemDisplayId[a2], rec);
}

// OFFSET: 0x718B10
uint32_t CGUnit_C::GetVirtualItemDisplayID(uint8_t a2) {
    return this->m_virtualItemDisplayId[a2];
}


// OFFSET: 0x71A380
uint8_t CGUnit_C::GetClientStandState() {
    return LOBYTE(this->m_unit->UNIT_FIELD_BYTES_1);
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
    ClientServices::SetMessageHandler(MSG_MOVE_START_FORWARD, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_BACKWARD, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_STRAFE_LEFT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_STRAFE_RIGHT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP_STRAFE, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_ASCEND, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_DESCEND, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP_ASCEND, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_JUMP, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_TURN_LEFT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_TURN_RIGHT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP_TURN, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_PITCH_UP, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_PITCH_DOWN, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP_PITCH, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_SET_RUN_MODE, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_SET_WALK_MODE, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_TELEPORT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_SET_FACING, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_SET_PITCH, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_TOGGLE_COLLISION_CHEAT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_GRAVITY_CHNG, &CGUnit_C::HandleMovementPacket, 0);
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
    ClientServices::SetMessageHandler(MSG_MOVE_ROOT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_UNROOT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_SWIM, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP_SWIM, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_START_SWIM_CHEAT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_STOP_SWIM_CHEAT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_HEARTBEAT, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_FALL_LAND, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_UPDATE_CAN_FLY, &CGUnit_C::HandleMovementPacket, 0);
    ClientServices::SetMessageHandler(MSG_MOVE_UPDATE_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY, &CGUnit_C::HandleMovementPacket, 0);
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
    //ClientServices::SetMessageHandler(MSG_MOVE_KNOCK_BACK, &CGUnit_C::OnMoveEvent, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_HOVER, &CGUnit_C::OnMoveEvent, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_FEATHER_FALL, &CGUnit_C::OnMoveEvent, 0);
    //ClientServices::SetMessageHandler(MSG_MOVE_WATER_WALK, &CGUnit_C::OnMoveEvent, 0);
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
    CGUnit_C::RegisterMirrorHandlers();
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

// OFFSET: 0x741B60
int32_t CGUnit_C::HandleMovementPacket(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WGUID guid;
    *msg >> guid;

    CGUnit_C* unit = ClntObjMgrObjectPtr<CGUnit_C*>(guid, TYPEMASK_UNIT);
    if (unit) {
        return unit->OnMoveEvent(msgId, time, msg);
    }
    msg->Seek(msg->Size());
    return 0;
}
