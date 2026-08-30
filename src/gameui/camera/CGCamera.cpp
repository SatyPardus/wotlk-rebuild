#include "gameui/camera/CGCamera.hpp"
#include "clientobject/Unit_C.hpp"
#include <common/time/Time.hpp>
#include <clientobject/ObjectMgrClient.hpp>
#include <world/CWorld.hpp>

CGCamera::CGCamera()
    : CSimpleCamera(CWorld::s_nearClip, CWorld::s_farClip, 1.5707964f) {
    this->m_model = nullptr;

    // #### TESTING
    // Taken from game defaults
    this->m_distance = 5.55f;
    this->m_pitch = 10.0f * 0.017453292f;
    //####

    //this->m_someTime = OsGetAsyncTimeMs();
    //this->unk_050 = 0;
    //this->m_modelMatrix.a0 = 1.0;
    //this->m_modelMatrix.a1 = 0.0;
    //this->m_modelMatrix.a2 = 0.0;
    //this->m_modelMatrix.b0 = 0.0;
    //this->m_modelMatrix.b2 = 0.0;
    //this->m_modelMatrix.c0 = 0.0;
    //this->m_modelMatrix.c1 = 0.0;
    //this->m_modelMatrix.d0 = 0.0;
    //this->m_modelMatrix.d1 = 0.0;
    //this->m_modelMatrix.d2 = 0.0;
    //this->m_modelMatrix.b1 = 1.0;
    //this->m_modelMatrix.c2 = 1.0;
    this->m_targetGUID.guid_low = 0;
    this->m_targetGUID.guid_high = 0;
    //this->unk_00A8 = 0.0;
    //this->guid_0090.guid_low = 0;
    //this->guid_0090.guid_high = 0;
    this->m_state = 0;
    this->m_flags = 0;
    this->m_relativeTo.guid_low = 0;
    this->m_relativeTo.guid_high = 0;
    //this->unk_00AC = 0;
    //this->unk_00B0 = 0;
    //this->unk_00B4 = s_cvCameraView->m_intValue;
    //this->m_distance = SStrToFloat((&off_AD1BA8)[3 * this->unk_00B4]);
    this->m_yaw = 0.0;
    //this->m_pitch = SStrToFloat((&off_AD1BAC)[3 * this->unk_00B4]) * 0.017453292;
    this->m_roll = 0.0;
    this->m_height = 0.0;
    //this->unk_012C = 0.0;
    //this->m_targetOffset = 0.0;
    //this->m_groundTilt = 0.0;
    //this->m_targetFov = 0.0;
    this->m_flyingMountHeight = 0.0;
    //*&this->unk_0140 = 0.0;
    //*&this->unk_0144 = 0.0;
    //*&this->unk_0148 = 0.0;
    this->m_targetHeightNear = 0.0;
    this->m_heightChangedTimeMs = 0;
    this->m_targetHeightFar = 0.0;
    //this->unk_0160 = 0;
    this->m_targetHeightSwim = 0.0;
    this->m_mountHeight = 0.0;
    this->m_targetPosition.x = 0.0;
    this->m_targetPosition.y = 0.0;
    this->m_targetPosition.z = 0.0;
    this->unk_01D0 = 0.0;
    this->m_targetFacing = 0.0;
    //this->unk_01D8 = 0;
    //*&this->unk_01DC = 0.0;
    this->m_smoothDistance.startTimeMs = 0;
    this->m_smoothDistance.rate = 0.0;
    this->m_smoothGroundTilt.startTimeMs = 0;
    m_distance = this->m_distance;
    this->m_smoothHeight.startTimeMs = 0;
    this->m_smoothDistance.target = m_distance;
    this->m_smoothPitch.startTimeMs = 0;
    this->m_smoothTargetOffset.startTimeMs = 0;
    this->m_smoothDistance.startValue = 0.0;
    this->m_smoothYaw.startTimeMs = 0;
    this->m_smoothDistance.param3 = 0.0;
    this->m_smoothFoV.startTimeMs = 0;
    this->m_smoothDistance.param4 = 0.0;
    //this->unk_0288 = 0;
    this->m_smoothGroundTilt.rate = 0.0;
    this->m_smoothGroundTilt.target = 0.0;
    this->m_smoothGroundTilt.startValue = 0.0;
    this->m_smoothGroundTilt.param3 = 0.0;
    this->m_smoothGroundTilt.param4 = 0.0;
    this->m_smoothHeight.rate = 0.0;
    this->m_smoothHeight.target = 0.0;
    this->m_smoothHeight.startValue = 0.0;
    this->m_smoothHeight.param3 = 0.0;
    this->m_smoothHeight.param4 = 0.0;
    this->m_smoothPitch.rate = 0.0;
    this->m_smoothPitch.target = this->m_pitch;
    this->m_smoothPitch.startValue = 0.0;
    this->m_smoothPitch.param3 = 0.0;
    this->m_smoothPitch.param4 = 0.0;
    this->m_smoothTargetOffset.rate = 0.0;
    this->m_smoothTargetOffset.target = 0.0;
    this->m_smoothTargetOffset.startValue = 0.0;
    this->m_smoothTargetOffset.param3 = 0.0;
    this->m_smoothTargetOffset.param4 = 0.0;
    this->m_smoothYaw.rate = 0.0;
    this->m_smoothYaw.target = 0.0;
    this->m_smoothYaw.startValue = 0.0;
    this->m_smoothYaw.param3 = 0.0;
    this->m_smoothYaw.param4 = 0.0;
    this->m_smoothFoV.rate = 0.0;
    this->m_smoothFoV.target = 0.0;
    this->m_smoothFoV.startValue = 0.0;
    this->m_smoothFoV.param3 = 1.0;
    this->m_smoothFoV.param4 = 0.0;
    //*&this->unk_028C = 0.0;
    //*&this->unk_0290 = 0.0;
    //*&this->unk_0294 = 0.0;
    //*&this->unk_0298 = 0.0;
    this->m_smoothFlyingHeight.rate = 0.0;
    //this->unk_029C = 1;
    this->m_smoothFlyingHeight.target = 0.0;
    //this->unk_02A0 = 0;
    this->m_smoothFlyingHeight.startValue = 0.0;
    //this->unk_02A4 = 0;
    this->m_smoothFlyingHeight.param3 = 0.0;
    this->m_smoothFlyingHeight.startTimeMs = 0;
    this->m_smoothFlyingHeight.param4 = 0.0;
    //this->m_vehicleZoomEnabled = 0;
    //this->unk_02D4 = 0;
    //*&this->unk_02C0 = 1.0;
    //this->unk_02D8 = 0;
    //*&this->unk_02C8 = 0.0;
    //*&this->unk_02CC = 0.0;
    //*&this->unk_02D0 = 0.0;
    //*&this->unk_02DC = 0.0;
    //*&this->unk_02E0 = 0.0;
    //*&this->unk_02E4 = 0.0;
    //this->unk_02E8 = 0;
    //this->unk_02EC = 0;
    //*&this->unk_02F0 = 0.0;
    //*&this->unk_02F4 = 0.0;
    //*&this->unk_02F8 = 0.0;
    //this->unk_02FC = 0;
    //this->m_cameraShakeList.m_terminator.m_next = 0;
    //this->m_cameraShakeList.m_terminator.m_prevlink = &this->m_cameraShakeList.m_terminator;
    //this->m_cameraShakeList.m_linkoffset = 0;
    //this->m_cameraShakeList.m_terminator.m_next = (&this->m_cameraShakeList.m_terminator | 1);
    //this->m_vehicleCamera = 0;
    //memset(this->unk_00B8, 0, sizeof(this->unk_00B8));
    //this->unk_0164 = 0;
    //this->unk_0168 = 0;
    //this->unk_016C = 0;
    //this->unk_0170 = 0;
    //this->unk_0174 = 0;
    //this->unk_0178 = 0;
    //this->unk_017C = 0;
    //this->m_lastZoomTime = 0;
    //this->unk_0184 = 0;
    //this->unk_0188 = 0;
    //this->unk_018C = 0;
    //this->unk_0190 = 0;
    //this->unk_0194 = 0;
    //this->unk_0198 = 0;
    //this->unk_019C = 0;
    //this->unk_01A0 = 0;
    //this->unk_01A4 = 0;
    //this->unk_01A8 = 0;
    //sub_5FE510(this);
    this->SetTarget(nullptr, 0);
    //ConsoleCommandRegister("pitchLimit", bn_CGCamera_CCommand_PitchLimit, 4, 0);
    this->m_state |= 0x50u;
    //this->unk_030C = 2;
    //*&this->unk_0310 = 1.8315002;
    //*&this->unk_0314 = 1.8315002;
    //*&this->unk_0318 = 0.0;
    //if (g_cvAutoInteract->m_intValue)
    //    this->m_flags |= 1u;
    //else
    //    this->m_flags &= ~1u;
}

// OFFSET: 0x5FE880
void CGCamera::SetupWorldProjection(const CRect& projectionRect) {
    this->SetGxProjectionAndView(projectionRect);
}

// OFFSET: 0x607B00
// Active change:
// This was apparently a static function in the client
// But the other argument was not used and this is just cleaner
void CGCamera::UpdateCallback() {
    auto time = OsGetAsyncTimeMs();
    this->m_nearZ = CWorld::s_nearClip;
    this->m_farZ = CWorld::s_farClip;
    if (this->m_model) {
        this->CalcModelCamera(time);
        this->CheckUnderwater();
        return;
    }
    auto targetObj = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);
    if (targetObj) {
        this->CalcTargetCamera(targetObj, time);
        this->CheckUnderwater();
        return;
    }
    //auto v8 = ClntObjMgrObjectPtr<CGObject_C*>(*&this->unk_0090, TYPEMASK_OBJECT);
    //if (v8) {
    //    maybe_CGCamera__FaceTarget(this, v8, v4);
    //    bn_CGCamera_CheckUnderwater(this);
    //}
}

// OFFSET: 0x6066E0
void CGCamera::SetTarget(CGObject_C* target, bool a3) {
    //if ((this->m_state & 0x10) != 0)
    //    bn_CGWorldFrame_SetPlayerFadeCameraValue(CGWorldFrame::s_currentWorldFrame, 0xFFu);

    bool hadTarget = this->m_targetGUID != 0;
    bool isNewTarget = false;
    bool positionChanged = false;
    if (target) {
        if (target->m_obj->m_guid != this->m_targetGUID) {
            isNewTarget = true;
        }

        //m_vehicleCamera = this->m_vehicleCamera;
        //if (m_vehicleCamera) {
        //    if ((*(m_vehicleCamera + 8) & 0x40) == 0)
        //        CVehicleCamera_C::ComputeSafeCurWorldPos(this->m_vehicleCamera);
        //    v16 = *(m_vehicleCamera + 56);
        //    v17 = *(m_vehicleCamera + 60);
        //    guid_high = *(m_vehicleCamera + 64);
        //} else {
        C3Vector targetPos;
        target->GetPosition(targetPos);
        //}

        if (this->m_targetPosition != targetPos) {
            positionChanged = true;
        }
    }

    bool dontSmooth = false;
    if ((this->m_state & 0x10) == 0 || isNewTarget) {
        dontSmooth = true;
    }

    if ((this->m_state & 0x10) == 0 || isNewTarget || positionChanged) {
        //AsyncTimeMs = OsGetAsyncTimeMs();
        //bn_CGCamera_CalcTerrainTilt(this, *&a2, AsyncTimeMs);
        //bn_CGCamera_PerformTerrainTilt(this, a2, AsyncTimeMs, dontSmooth);
    }

    float drunknessTurnValue = 0.0f;
    //if (a2 && (a2->ObjectBase.m_obj->OBJECT_FIELD_TYPE & 0x10) != 0)
    //    DrunknessTurnValue = CGPlayer_C__GetDrunknessTurnValue(&a2[1]);

    //this->UpdateInebriation(drunknessTurnValue, dontSmooth);
    this->unk_01D0 = 0.0f;

    if (target) {
        this->m_targetGUID = target->m_obj->m_guid;
        if ((target->m_obj->m_type & TYPEMASK_UNIT) != 0) {
            this->m_targetFacing = target->GetRawFacing();
            //WGUID someGuid = target->GetCameraRelativeTo();
            //if (ClntObjMgrObjectPtr<CGObject_C*>(someGuid, TYPEMASK_OBJECT))
            //    this->MakeRelativeTo(this, someGuid);
            //else
            //    this->MakeRelativeTo(this, 0);
        } else {
            this->m_targetFacing = target->GetFacing();
            this->m_relativeTo = 0;
        }
        if (a3) {
            this->m_flags |= 2u;
            //this->SetModeFreeLook();
        } else {
            if ((m_flags & 2) != 0) {
                this->m_flags = this->m_flags & 0xFFFFFFFD;
                //this->SetModeNormal();
            }
        }
        //this->guid_0090 = 0;
        if (!this->FinishLoadingTarget(target))
            this->m_state &= ~4u;
    } else {
        this->m_targetGUID = 0;
        this->m_relativeTo = 0;
        //bn_TSList_CameraShake_Clear(&this->m_cameraShakeList.m_linkoffset);
        this->m_flags &= ~2u;
    }

    this->m_state &= ~0x800000u;

    //if (!hadTarget)
    //    this->PickVehicleCamera();
}

// OFFSET: 0x601E90
void CGCamera::CalcModelCamera(int32_t time) {
    //if ((this->m_state & 4) != 0 || CM2Model::IsLoaded(this->m_model, 0, 0) && CGCamera::FinishLoadingModel(this)) {
    //    AsyncTimeMs = OsGetAsyncTimeMs();
    //    CM2Model::SetAnimating(this->m_model, 1);
    //    v8 = AsyncTimeMs - this->m_someTime;
    //    Scene = CSimpleCamera::GetScene(this);
    //    CM2Scene::AdvanceTime(Scene, v8);
    //    v10.x = 0.0;
    //    v10.y = 0.0;
    //    v10.z = 0.0;
    //    v5 = CSimpleCamera::GetScene(this);
    //    CM2Scene::Animate(v5, &v10);
    //    *v11 = 0.0;
    //    *&v11[1] = 0.0;
    //    *&v11[2] = 0.0;
    //    *v12 = 0.0;
    //    *&v12[1] = 0.0;
    //    *&v12[2] = 0.0;
    //    this->m_someTime = AsyncTimeMs;
    //    DataMgrGetCoord();
    //    DataMgrGetCoord();
    //    Float = CameraGetFloat(v7, v6, this->unk_050, 5u);
    //    CGCamera::SetPositionAndTargetWithRoll(this, v11, v12, Float);
    //}
}

// OFFSET: 0x606F90
void CGCamera::CalcTargetCamera(CGObject_C* target, int32_t time) {
    if (!target) {
        //NOP("target");
        SErrSetLastError(87);
        return;
    }

    if ((this->m_state & 4) == 0 && !this->FinishLoadingTarget(target)) {
        return;
    }

    //####TESTING
    C3Vector targetPos;
    target->GetPosition(targetPos);

    float yaw = this->m_yaw /*+ this->unk_012C*/;
    //if (this->m_ignoreFacingRefs <= 0)
        yaw += target->GetFacing();

    C3Vector lookAt = targetPos;
    lookAt.z += this->m_height; // +0x128

    this->SetFacing(yaw, this->m_pitch, this->m_roll);
    this->m_position = lookAt - this->Forward() * this->m_distance;

    this->UpdateTargetHeight(target, time);
    this->m_targetPosition = targetPos;
}

// OFFSET: 0x5FE7B0
void CGCamera::CheckUnderwater() {
    //v2 = World::SceneCamLiquidStatus(&v4);
    //SI2::SetUnderwaterStatus(v2);
    //m_state = this->m_state;
    //if ((m_state & 2) == 0 || this->unk_02A0 != v2) {
    //    this->m_state = m_state | 2;
    //    this->unk_02A0 = v2;
    //}
}

// OFFSET: 0x604E00
bool CGCamera::FinishLoadingTarget(CGObject_C* target) {
    auto model = target->GetObjectModel();
    if (model && !model->IsLoaded(0, 0))
        return false;

    this->m_state |= 4u;
    this->m_targetHeightNear = 0.0f;
    this->m_targetHeightFar = 0.0f;
    this->m_targetHeightSwim = 0.0f;

    auto scale = target->GetScale() /** target->unk_009C*/;
    float maxCameraHeight = 15.0f;

    if ((target->m_obj->m_type & TYPEMASK_UNIT) != 0 && model) {
        auto unit = reinterpret_cast<CGUnit_C*>(target);

        //if (target->data98C)
        //    scale *= target->data990;
        //v10 = a2->ObjectBase.__vftable;
        //v58 = scale;
        //ukn78 = v10->ukn78;
        //m_worldModel = a2->ObjectBase.m_worldModel;
        //v56 = -1;
        //switch ((ukn78)(a2)) {
        //case 1:
        //    v60 = 97;
        //    break;
        //case 3:
        //    v60 = 100;
        //    break;
        //case 4:
        //    v60 = 102;
        //    break;
        //case 5:
        //    v60 = 103;
        //    break;
        //case 6:
        //    v60 = 104;
        //    break;
        //case 7:
        //    v60 = 6;
        //    break;
        //case 8:
        //    v60 = 115;
        //    break;
        //default:
        //    v60 = 0;
        //    break;
        //}
        //if (bnl_s_cvCameraHeightIgnoreStandState->m_intValue)
        //    v60 = 0;
        //if (a2->data98C) {
        //    v60 = 91;
        //    v56 = 0;
        //}
        //sub_52E570(v38);
        //sub_52E570(v45);
        //M2Model::sub_82CED0(m_worldModel, 0.0, 0, v38);
        //M2Model::sub_82CED0(m_worldModel, *&v60, 0, v45);
        //v39 = v39 * v59;
        //v40 = v40 * v59;
        //v41 = v41 * v59;
        //v42 = v42 * v59;
        //v43 = v43 * v59;
        //v13 = v44 * v59;
        //v44 = v13;
        //v46 = v46 * v59;
        //v47 = v47 * v59;
        //v48 = v48 * v59;
        //v49 = v49 * v59;
        //v50 = v50 * v59;
        //v14 = v13;
        //v15 = v59 * v51;
        //v51 = v15;
        //*&unk_009C = v14 - v15;
        //if (CM2Model::HasAttachment(m_worldModel, 0x11u)) {
        //    CM2Model__GetAttachmentTransform(m_worldModel, &v55.c.y, 0x11u);
        //    if (CGBarberShop::m_barberShopEnabled && CGUnit_C::IsActivePlayer(&a2->ObjectBase))
        //        v16 = (v55.r + 0.097222224 - *&unk_009C) * v59;
        //    else
        //        v16 = (v55.r + 0.097222224) * v59;
        //} else {
        //    if ((m_worldModel->m_shared->m_flags & 1) == 0)
        //        CM2Model::WaitForLoad(m_worldModel, 0);
        //    m_data = m_worldModel->m_shared->m_data;
        //    x_low = LODWORD(m_data->collisionBounds.extent.b.x);
        //    y_low = LODWORD(m_data->collisionBounds.extent.b.y);
        //    m_data = (m_data + 188);
        //    v53 = x_low;
        //    LODWORD(v55.c.x) = m_data->name.count;
        //    v20 = *&m_data->flags;
        //    v54 = y_low;
        //    v21 = *&m_data->name.offset;
        //    LODWORD(v55.r) = m_data->loops.count;
        //    v55.c.y = v21;
        //    v55.c.z = v20;
        //    v16 = (v55.r - v55.c.x) * v59 * 0.89999998;
        //}
        //this->m_targetHeightNear = this->m_targetHeightNear + v16;
        //this->m_targetHeightFar = this->m_targetHeightFar + v16;
        //this->m_targetHeightSwim = v16 + this->m_targetHeightSwim;
        //if (!CM2Model::HasSequence(m_worldModel, v60))
        //    v60 = 0;
        //if (CM2Model::HasSequence(m_worldModel, 0x2Au)) {
        //    sub_52E570(v35);
        //    sub_52E570(v32);
        //    M2Model::sub_82CED0(m_worldModel, 0.0, 0, v35);
        //    M2Model::sub_82CED0(m_worldModel, COERCE_FLOAT(42), 0, v32);
        //    sub_5FECB0(v36, v59);
        //    sub_5FECB0(v33, v59);
        //    this->m_targetHeightSwim = this->m_targetHeightSwim - (v37 - v34);
        //}
        //v22 = *&unk_009C;
        //this->m_targetHeightNear = this->m_targetHeightNear - *&unk_009C;
        //if (v60 == 91)
        //    this->m_targetHeightFar = this->m_targetHeightFar - v22;
        //if (v56 != -1) {
        //    v23 = v52;
        //    if (CM2Model::HasAttachment(v52, v56)) {
        //        CM2Model__GetAttachmentTransform(v23, &v55.c.y, v56);
        //        v24 = v55.r * v58;
        //        v61 = v24;
        //        if ((a2->m_unit->UNIT_FIELD_FLAGS & 0x100000) != 0) {
        //            if (CM2Model::HasSequence(v23, 0x87u)) {
        //                sub_52E570(v32);
        //                sub_52E570(v35);
        //                M2Model::sub_82CED0(v23, 0.0, 0, v32);
        //                M2Model::sub_82CED0(v23, COERCE_FLOAT(135), 0, v35);
        //                sub_5FECB0(v33, v58);
        //                sub_5FECB0(v36, v58);
        //                v24 = v61 - (v34 - v37);
        //            } else {
        //                v24 = v61;
        //            }
        //        }
        //        this->m_targetHeightNear = this->m_targetHeightNear + v24;
        //        this->m_targetHeightFar = this->m_targetHeightFar + v24;
        //        this->m_targetHeightSwim = v24 + this->m_targetHeightSwim;
        //    }
        //}
        maxCameraHeight = unit->GetMaxCameraHeight();
        if (maxCameraHeight <= 0.0f)
            maxCameraHeight = 15.0f;
        //if (CGBarberShop::m_barberShopEnabled)
        //    MaxCameraHeight = MaxCameraHeight * 1.2;
    } else if ((target->m_obj->m_type & TYPEMASK_DYNAMICOBJECT) == 0 || !model) {
        this->m_targetHeightSwim = scale + scale;
        this->m_targetHeightFar = scale + scale;
        this->m_targetHeightNear = scale + scale;
    } else {
        auto v55 = model->GetBoundingSphere();
        float finalScale = scale + scale;
        if (v55.r > 0.0099999998f) {
            finalScale = v55.r * 0.99000001f * scale;
        }
        this->m_targetHeightSwim = finalScale;
        this->m_targetHeightFar = finalScale;
        this->m_targetHeightNear = finalScale;
    }

    float minHeight = 0.83333331f;
    float heightNear = this->m_targetHeightNear;
    if (heightNear >= minHeight) {
        if (heightNear >= maxCameraHeight)
            heightNear = maxCameraHeight;
    } else {
        heightNear = minHeight;
    }
    this->m_targetHeightNear = heightNear;

    float heightFar = this->m_targetHeightFar;
    if (heightFar >= minHeight) {
        if (heightFar >= maxCameraHeight)
            heightFar = maxCameraHeight;
    } else {
        heightFar = minHeight;
    }
    this->m_targetHeightFar = heightFar;

    float heightSwim = this->m_targetHeightSwim;
    if (heightSwim >= minHeight) {
        if (heightSwim >= maxCameraHeight)
            heightSwim = maxCameraHeight;
    } else {
        heightSwim = minHeight;
    }
    this->m_targetHeightSwim = heightSwim;

    this->UpdateTargetHeight(target, OsGetAsyncTimeMs());
    //this->UpdateLiquidSurfaceStatus(target);
    return 1;
}

// OFFSET: 0x604640
void CGCamera::UpdateTargetHeight(CGObject_C* target, int32_t time) {
    float v3 = 1.0f;
    if (this->m_heightChangedTimeMs > 0) {
        if (OsGetAsyncTimeMs() - this->m_heightChangedTimeMs > 2999) {
            this->m_heightChangedTimeMs = 0;
        } else {
            v3 = 0.5f;
        }
    }
    if (target && (target->m_obj->m_type & TYPEMASK_UNIT) != 0 && (target->AsUnit()->m_passenger->m_flags & MOVEMENTFLAG_SWIMMING) != 0) {
        this->SmoothSetHeight(this->m_targetHeightSwim, 0.0f, v3, time);
    } else if (this->m_smoothDistance.target >= 1.8315002f /*|| CGBarberShop::m_barberShopEnabled*/) {
        this->SmoothSetHeight(this->m_targetHeightFar, 0.0f, v3, time);
        if (target && (target->m_obj->m_type & TYPEMASK_UNIT) != 0 && target->AsUnit()->GetCanFly()) {
            this->SmoothSetFlyingMountHeight(this->m_mountHeight, 0.0f, v3, time);
        } else {
            this->SmoothSetFlyingMountHeight(0.0f, 0.0f, v3, time);
        }
    } else {
        this->SmoothSetHeight(this->m_targetHeightNear, 0.0f, v3, time);
        this->SmoothSetFlyingMountHeight(0.0f, 0.0f, 1.0f, time);
    }
}

// OFFSET: 0x603230
bool CGCamera::SmoothSetHeight(float a2, float a3, float a4, int32_t time) {
    //#### TESTING
    this->m_height = a2;
    return false;
}

// OFFSET: 0x601550
bool CGCamera::SmoothSetFlyingMountHeight(float a2, float a3, float a4, int32_t time) {
    return false;
}
