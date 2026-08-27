#include "clientobject/Unit_C.hpp"

#include "db/Db.hpp"

CGUnit_C::CGUnit_C() {

}

CGUnit_C::CGUnit_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    tempPosition = objCreate.m_moveUpdate.status.m_position;
    tempFacing = objCreate.m_moveUpdate.status.m_facing;
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
    //this->data9E0[20] |= 0x80000u;
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
    // TODO
    pos = tempPosition;
}

// OFFSET: 0x6E6F40
float CGUnit_C::GetFacing() {
    // TODO
    return tempFacing;
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
