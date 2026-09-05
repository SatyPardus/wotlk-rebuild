#include "gameui/camera/CGCamera.hpp"
#include "clientobject/Unit_C.hpp"
#include <common/time/Time.hpp>
#include <clientobject/ObjectMgrClient.hpp>
#include <world/CWorld.hpp>
#include "gameui/camera/CameraCVars.hpp"
#include <gameui/CGBarberShop.hpp>
#include <tempest/math/CMath.hpp>
#include <client/FrameTime.hpp>
#include <gameui/CGInputControl.hpp>
#include <gx/Coordinate.hpp>
#include <util/Unimplemented.hpp>

bool CGCamera::s_aboveFlightFloor;

// OFFSET: 0x5FD5C0
float UnwrapAngleToward(float target, float current, float lo, float hi) {
    while (target - current < lo) {
        current -= 6.2831855f;
    }

    if (hi >= target - current) {
        return current;
    }

    float unwrapped;

    while (1) {
        unwrapped = current + 6.2831855f;

        if (target - unwrapped <= hi) {
            break;
        }

        current = unwrapped;
    }

    return unwrapped;
}

// OFFSET: 0x5FECF0
float DistanceBetween(const C3Vector& a, const C3Vector& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;

    return sqrt(dx * dx + dz * dz + dy * dy);
}

// OFFSET: 0x8CA080
float OrganicSmooth(float from, float to, float t) {
    return from + (to - from) * ((1.0f - cos(t * 3.1415927f)) * 0.5f);
}

// OFFSET: 0x4C55B0
void BuildYawRotationMatrix(C33Matrix& matrix, float yaw) {
    float c = cos(yaw);
    float s = sin(yaw);

    C33Matrix rotation(c, 0.0f, -s, 0.0f, 1.0f, 0.0f, s, 0.0f, c);

    matrix = rotation * matrix;
}

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
    this->m_viewIndex = s_cvCameraView->m_intValue;
    //this->m_distance = SStrToFloat((&off_AD1BA8)[3 * this->m_viewIndex]);
    this->m_yaw = 0.0;
    //this->m_pitch = SStrToFloat((&off_AD1BAC)[3 * this->m_viewIndex]) * 0.017453292;
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
    memset(this->m_views, 0, sizeof(this->m_views));
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
        SErrSetLastError(87);
        return;
    }

    if (!(this->m_state & 0x4) && !this->FinishLoadingTarget(target)) {
        return;
    }

    this->UpdateVehicleTarget(FrameTime::s_curTimeMs);

    CGUnit_C* unit = nullptr;
    uint32_t isFlying = 0;
    int32_t splineActive = 0;
    int32_t autoTracking = 0;
    uint32_t swimming = 0;
    int32_t uncontrolled = 0;
    float groundPitch = 0.0f;

    if (target->m_obj->m_type & TYPEMASK_UNIT) {
        //unit = target->AsUnit();
        //
        //isFlying = (unit->m_unit->UNIT_FIELD_FLAGS >> 20) & 1;
        //autoTracking = unit->IsAutoTracking();
        //
        //if (sub_4F5290(&unit->m_unit) || (unit->m_passenger->m_flags & 0x2000000)) {
        //    uncontrolled = 1;
        //}
        //
        //uint32_t moveFlags = unit->m_passenger->m_flags;
        //auto spline = unit->m_passenger->m_spline;
        //
        //swimming = moveFlags & 0x200000;
        //
        //splineActive = (spline && !(spline->flags & 0x400)) ? 1 : 0;
        //
        //groundPitch = unit->GetPitch();
        //
        //this->UpdateMountHeightOrOffset(unit);
    }

    this->UpdateUncontrolledState((splineActive && !isFlying) ? 1 : 0);

    C3Vector safePos;
    this->GetSafeWorldPos(safePos, target);

    float flightFloor = -500.0f;
    World::GetFlightBoundsLower(safePos, &flightFloor);

    if (flightFloor <= safePos.z) {
        s_aboveFlightFloor = true;
    } else {
        safePos.x = this->m_targetPosition.x;
        safePos.y = this->m_targetPosition.y;
        safePos.z = flightFloor;

        if (s_aboveFlightFloor) {
            //CGameUI::PlaySound("SpaceDeathUniversal", 0, 0, 0);
        }

        s_aboveFlightFloor = false;
    }

    if (isFlying && !this->m_wasFlying) {
        this->SetDesiredYawAngle(0.0f, 0.0f, 1.0f, OsGetAsyncTimeMs());

        if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER) && (unit->AsPlayer()->m_player->PLAYER_FLAGS & 0x20000)) {
            this->SetDesiredPitchAngle(0.0f, 0.0f, 1.0f, OsGetAsyncTimeMs());
        }
    }

    if (this->m_motionFlags) {
        this->UpdateMotion(time);
    }

    this->UpdateTargetSmoothing(target, time);

    float yaw;
    float pitch;
    float roll;
    this->UpdateTargetFacing(target, &yaw, &pitch, &roll);

    C3Vector shake = { 0.0f, 0.0f, 0.0f };

    if (!uncontrolled && !swimming) {
        C3Vector eye = safePos;
        eye.z += this->m_height;

        C3Vector forward = this->Forward();

        C3Vector from;
        from.x = eye.x - forward.x * this->m_distance;
        from.y = eye.y - forward.y * this->m_distance;
        from.z = eye.z - forward.z * this->m_distance;

        //this->RunShakes(&from, &shake);
    }

    //this->RunBobs(&this->m_bobOffset);
    //
    //shake.x += this->m_bobOffset.x;
    //shake.y += this->m_bobOffset.y;
    //shake.z += this->m_bobOffset.z;

    uint32_t state = this->m_state;
    uint32_t wasSubmerged = state & 0x200000;
    uint32_t wasAtSurface = state & 0x100000;

    float liquid = 0;// this->UpdateLiquidSurfaceStatus(unit);

    auto input = CGInputControl::GetActive();

    float desiredPitch = pitch + this->m_targetOffset;

    //if (swimming) {
    //    bool drunkTurning = unit && (unit->m_obj->m_type & 0x10) && unit->AsPlayer()->GetDrunknessTurnValue() != 0.0f;
    //
    //    if (!drunkTurning) {
    //        uint32_t diveState = this->m_state;
    //        uint32_t atSurface = diveState & 0x100000;
    //
    //        if ((atSurface || (diveState & 0x200000)) && s_cvCameraDive->m_intValue && FloatNotEquals(groundPitch, 0.0f) && autoTracking == 0) {
    //
    //            float surfacePitch = s_cvCameraSurfacePitch->m_floatValue * 0.017453292f;
    //            float submergePitch = s_cvCameraSubmergePitch->m_floatValue * 0.017453292f;
    //
    //            bool levelPitch = false;
    //            bool decided = false;
    //
    //            if ((diveState & 0x200000) && surfacePitch < 0.0f) {
    //                if (!(input->m_flags & 0x2000001)) {
    //                    levelPitch = true;
    //                    decided = true;
    //                } else if (desiredPitch < 0.0f && desiredPitch > surfacePitch) {
    //                    levelPitch = true;
    //                    decided = true;
    //                }
    //            }
    //
    //            if (!decided && atSurface && submergePitch > 0.0f) {
    //                if (!(input->m_flags & 0x2000001)) {
    //                    levelPitch = true;
    //                } else if (desiredPitch > 0.0f && desiredPitch < submergePitch) {
    //                    levelPitch = true;
    //                }
    //            }
    //
    //            if (levelPitch) {
    //                input->CameraPitchPlayer(time, 0.0f);
    //            }
    //        }
    //    }
    //}

    if (s_cvCameraWaterCollision->m_intValue) {
        bool haveFinal = false;
        float target_ = 0.0f;

        if (wasSubmerged && (this->m_state & 0x100000)) {
            target_ = s_cvCameraSurfaceFinalPitch->m_floatValue;
            haveFinal = true;
        } else if (wasAtSurface && (this->m_state & 0x200000)) {
            target_ = s_cvCameraSubmergeFinalPitch->m_floatValue;
            haveFinal = true;
        }

        if (haveFinal) {
            float radians = target_ * 0.017453292f;

            if (radians != 0.0f) {
                if (this->m_state & 0x1) {
                    this->m_pitch = radians;
                    this->m_smoothPitch.target = radians;
                } else {
                    this->SetDesiredPitchAngle(radians, 0.0f, 1.0f, time);
                }
            }
        }
    }

    //if (this->unk_02D4 != this->unk_02D8) {
    //    sub_6006A0(this, time);
    //}

    this->m_state &= 0xFFFCFFFF;

    float distance = this->m_distance > this->m_smoothDistance.target
                         ? this->m_distance
                         : this->m_smoothDistance.target;
    float height = this->m_height > this->m_smoothHeight.target
                       ? this->m_height
                       : this->m_smoothHeight.target;

    if (this->m_state & 0x8) {
        distance = this->m_distance;
    }

    this->unk_02C0 = 1.0f;

    //uint32_t collideFlags = this->CollideCameraWithWorld(&safePos, &distance, &height, &shake, liquid, &this->m_collideExtent);

    //this->m_state |= collideFlags;

    C3Vector lookAt;
    lookAt.x = safePos.x + shake.x;
    lookAt.y = safePos.y + shake.y;
    lookAt.z = safePos.z + shake.z + height;

    state = this->m_state;

    if (!(state & 0x8) && !(state & 0x80000000)) {
        if (this->CanSmoothTarget()) {
            this->SetDesiredTargetOffset(0.0f, 0.0f, 1.0f, time);
        } else {
            //this->sub_5FEF10();
        }
    }

    state = this->m_state;

    if ((state & 0x8) && !(state & 0x40000000)) {
        distance = 0.0f;
        this->m_state = state & 0xFFFF7FFF;
    } else if (!(state & 0x80000000) || (state & 0x40000000)) {
        // fall through with the state unchanged
    } else {
        this->m_state = state & 0x7FFFFFFF;
    }

    C3Vector facing = { 0.0f, 0.0f, 0.0f };

    this->GetCameraPosition(&this->m_position, lookAt, distance, facing);
    //this->BumpCameraPositionFromLiquid(&this->m_position, &lookAt, distance, &this->m_position);

    //if (!this->m_relativeTo && s_cvCameraWaterCollision->m_intValue == 0 && this->unk_02E4 == 0.0f && (!unit || !unit->GetCanFly())) {
    //    facing.x = lookAt.x - this->m_position.x;
    //    facing.y = lookAt.y - this->m_position.y;
    //    facing.z = lookAt.z - this->m_position.z;
    //
    //    if (facing.x * facing.x + facing.y * facing.y + facing.z * facing.z > 0.1f) {
    //        facing.Normalize();
    //        this->SetFacing(facing);
    //    }
    //}

    //if (this->unk_02E8 != this->unk_02EC) {
    //    sub_6007B0(this, time);
    //}
    
    //if (this->unk_02F8 != 0.0f) {
    //    BuildYawRotationMatrix(this->m_facing, -this->unk_02F8);
    //}

    if (fabs(this->m_targetOffset) >= 0.001) {
        BuildYawRotationMatrix(this->m_facing, this->m_targetOffset);
    }

    if (this->m_distance - distance > 0.11111111f) {
        this->m_state |= 0x4000000;
        this->m_smoothDistance.startTimeMs = time;
        this->m_distance = distance + 0.11111207f;
        this->m_smoothDistance.startValue = distance + 0.11111207f;
        this->m_smoothDistance.rate = 2.0f;
    }

    if (this->m_height - height > 0.11111111f) {
        this->m_state |= 0x20000000;
        this->m_smoothHeight.startTimeMs = time;
        this->m_height = height + 0.11111207f;
        this->m_smoothHeight.startValue = height + 0.11111207f;
        this->m_smoothHeight.rate = 2.0f;
    }

    uint8_t fade = 0xFF;

    auto player = ClntObjMgrGetActivePlayerObj();

    bool sharedTransport = false;

    //if (player && unit) {
    //    auto seat = player->dataF00[24];
    //
    //    if (seat && *(seat + 20) != 0 && *(seat + 20) != 3) {
    //        if (player == unit) {
    //            sharedTransport = true;
    //        } else {
    //            sharedTransport = player->GetTransportGUID() == unit->m_obj->m_guid;
    //        }
    //    }
    //}

    CGUnit_C* fadeUnit = unit;

    //while (1) {
    //    float fadeFar = 1.8315002f;
    //    float fadeNear = 0.0027777778f;
    //
    //    if (fadeUnit) {
    //        auto model = fadeUnit->dataF00[23];
    //        auto seat = fadeUnit->dataF00[24];
    //
    //        bool onVehicle = (model && *(model + 12)) || (seat && *(seat + 20) == 3);
    //
    //        if (onVehicle && !sharedTransport) {
    //            if (!model || !*(model + 12)) {
    //                fadeUnit = ClntObjMgrObjectPtr<CGUnit_C*>(fadeUnit->GetTransportGUID(), TYPEMASK_UNIT);
    //            }
    //
    //            if (fadeUnit) {
    //                auto rec = fadeUnit->dataF00[23] ? *(fadeUnit->dataF00[23] + 12) : 0;
    //
    //                if (rec && (*(rec + 4) & 0x80000)) {
    //                    auto m2 = fadeUnit->GetObjectModel();
    //
    //                    if (m2 && m2->IsLoaded(0, 0)) {
    //                        float bounds[12];
    //                        sub_52E570(bounds);
    //                        M2Model::sub_82CED0(m2, 0.0f, 0, bounds);
    //
    //                        fadeNear = *(rec + 60) * bounds[12];
    //                        fadeFar = bounds[12] * *(rec + 64);
    //
    //                        C3Vector modelPos;
    //                        auto world = GetModelWorldTransform(m2, &modelPos);
    //
    //                        distance = DistanceBetween(this->m_position, *world);
    //                    }
    //                }
    //            }
    //        }
    //    }
    //
    //    if (CGBarberShop::m_barberShopEnabled) {
    //        fadeFar = fadeFar * 0.33333334f;
    //    }
    //
    //    if (distance < this->m_height && this->m_pitch < -1.1868238f) {
    //        float t = (-1.5707964f - this->m_pitch) * -2.604353f;
    //
    //        if (fabs(t) >= 0.00000023841858) {
    //            fadeFar = fadeFar / (t * t);
    //        }
    //    }
    //
    //    float near_ = distance - this->m_nearZ;
    //
    //    if (near_ < fadeFar) {
    //        if (fadeNear >= near_) {
    //            fade = 0;
    //            break;
    //        }
    //
    //        float t = (near_ - fadeNear) / (fadeFar - fadeNear);
    //        uint8_t scaled = static_cast<uint8_t>(static_cast<int32_t>(OrganicSmooth(0.0f, 255.0f, t)));
    //
    //        if (fade > scaled) {
    //            fade = scaled;
    //        }
    //    }
    //
    //    if (fadeUnit && fadeUnit->HasVehicleTransport() && !sharedTransport) {
    //        fadeUnit = ClntObjMgrObjectPtr<CGUnit_C*>(fadeUnit->GetTransportGUID(), TYPEMASK_UNIT);
    //
    //        if (fadeUnit) {
    //            continue;
    //        }
    //    }
    //
    //    break;
    //}
    //
    //CGWorldFrame::SetPlayerFadeCameraValue(CGWorldFrame::s_currentWorldFrame, fade);

    state = this->m_state;

    if (state & 0x40000000) {
        if ((state & 0x8000) || (state & 0x80000000)) {
            int32_t elapsed = time - this->m_smoothFoV.startTimeMs;

            if (elapsed >= 0) {
                float t = elapsed * 0.001f / this->m_smoothFoV.rate * 1.5f;

                if (t >= 1.0f) {
                    t = 1.0f;
                }

                float value = t * t;

                if (state & 0x8000) {
                    value = 1.0f - value;
                }

                uint8_t scaled = value * 255.0f;

                if (fade >= scaled) {
                    fade = scaled;
                }

                //CGWorldFrame::s_currentWorldFrame->SetPlayerFadeCameraValue(fade);
            }
        }
    }

    this->UpdateTargetHeight(target, time);

    C3Vector pos;
    target->GetPosition(pos);
    this->m_targetPosition = pos;

    this->m_wasFlying = isFlying;
}

// OFFSET: 0x600730
void CGCamera::RotateOffsetByRoll(C3Vector& offset) {
    if (this->unk_02E4 != 0.0f) {
        C44Matrix rotation;

        rotation.a0 = 1.0f;
        rotation.b1 = 1.0f;
        rotation.c2 = 1.0f;
        rotation.d3 = 1.0f;
        rotation.a1 = 0.0f;
        rotation.a2 = 0.0f;
        rotation.a3 = 0.0f;
        rotation.b0 = 0.0f;
        rotation.b2 = 0.0f;
        rotation.b3 = 0.0f;
        rotation.c0 = 0.0f;
        rotation.c1 = 0.0f;
        rotation.c3 = 0.0f;
        rotation.d0 = 0.0f;
        rotation.d1 = 0.0f;
        rotation.d2 = 0.0f;

        rotation.RotateAroundZ(this->unk_02E4);

        offset = offset * rotation;
    }
}

// OFFSET: 0x601D60
void CGCamera::GetCameraPosition(C3Vector* out, const C3Vector& lookAt, float distance, const C3Vector& offset) {
    *out = lookAt;

    C3Vector forward = this->Forward();

    this->RotateOffsetByRoll(forward);

    out->x -= forward.x * distance;
    out->y -= forward.y * distance;
    out->z -= forward.z * distance;

    if (distance > 0.0f && fabs(this->m_flyingMountHeight) >= 0.00000023841858) {
        float limit = this->m_distance >= this->m_smoothDistance.target
                          ? this->m_distance
                          : this->m_smoothDistance.target;

        if (limit > 0.0f) {
            float scale = distance * (this->unk_02C0 * this->m_flyingMountHeight) / limit;

            C3Vector up = this->Up();

            out->x += up.x * scale;
            out->y += up.y * scale;
            out->z += up.z * scale;
        }
    }

    out->x += offset.x;
    out->y += offset.y;
    out->z += offset.z;
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
        if (CGBarberShop::m_barberShopEnabled)
            maxCameraHeight = maxCameraHeight * 1.2;
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

// OFFSET: 0x604490
void CGCamera::UpdateTargetFacing(CGObject_C* target, float* yaw, float* pitch, float* roll) {
    *yaw = this->m_yaw;
    *pitch = this->m_pitch;
    *roll = this->m_roll;

    if (!target) {
        return;
    }

    C3Vector position;

    // if (this->m_vehicleCamera) {
    //     if (!(this->m_vehicleCamera->m_flags & 0x40)) {
    //         this->m_vehicleCamera->ComputeSafeCurWorldPos();
    //     }
    //     position = this->m_vehicleCamera->m_position;
    // } else {
    target->GetPosition(position);
    // }

    int32_t moved = this->m_targetPosition.x != position.x || this->m_targetPosition.y != position.y || this->m_targetPosition.z != position.z;

    float facing;

    if (target->m_obj->m_type & TYPEMASK_UNIT) {
        // CGUnit_C::sub_735F60(target, 0);

        // if (this->m_vehicleCamera) {
        //     facing = this->m_vehicleCamera->m_facing;
        // } else {
        //     facing = target->dataA34[27];
        // }
        //###TESTING
        facing = target->GetFacing();
        //######

        // if (target->m_obj->OBJECT_FIELD_GUID != CGUnit_C::m_activeMover && !this->m_vehicleCamera) {
        //     facing = this->SmoothTargetFacing(facing, moved);
        // }
    } else {
        facing = target->GetFacing();
    }

    this->m_targetFacing = facing;

    *yaw += this->m_yawOffset;

    if (this->m_ignoreFacingRefs <= 0) {
        *yaw += facing;
    }

    if (this->unk_00B0 <= 0) {
        *pitch += this->m_groundTilt;
    }

    *yaw = CMath::normalizeangle0to2pi(*yaw);

    if (*pitch < MIN_PITCH_ANGLE) {
        *pitch = MIN_PITCH_ANGLE;
    } else if (*pitch >= MAX_PITCH_ANGLE) {
        *pitch = MAX_PITCH_ANGLE;
    }

    this->SetFacing(*yaw, *pitch, *roll);
}

// OFFSET: 0x6010E0
bool CGCamera::StartSmoothHeight(float height, float rate, int32_t time) {
    if (fabs(this->m_height - height) < 0.001) {
        return false;
    }

    this->m_state |= 0x20000000;

    this->m_smoothHeight.startValue = this->m_height;
    this->m_smoothHeight.startTimeMs = time;
    this->m_smoothHeight.target = height;
    this->m_smoothHeight.rate = rate;

    if (!(this->m_state & 0x80)) {
        this->m_height = height;
        this->m_smoothHeight.target = height;
        this->m_state = (this->m_state & 0xDFFFFF7F) | 0x80;
        this->m_smoothHeight.rate = 0.0f;
        this->m_smoothHeight.startTimeMs = 0;
    }

    return true;
}

// OFFSET: 0x603230
bool CGCamera::SmoothSetHeight(float height, float delay, float rateScale, int32_t time) {
    if ((this->m_state & 0x20000000) && fabs(this->m_smoothHeight.target - height) < 0.001 && fabs(this->m_smoothHeight.param4 - delay) < 0.001 && fabs(this->m_smoothHeight.param3 - rateScale) < 0.001) {
        return true;
    }

    if (fabs(this->m_height - height) < 0.001) {
        return false;
    }

    this->m_smoothHeight.param4 = delay;
    this->m_smoothHeight.param3 = rateScale;

    float rate = fabs(height - this->m_height) / s_cvCameraHeightSmoothSpeed->m_floatValue * rateScale;

    return this->StartSmoothHeight(height, rate, time + delay * 1000.0f);
}

// OFFSET: 0x5FEBF0
bool CGCamera::StartSmoothFlyingMountHeight(float height, float rate, int32_t time) {
    if (fabs(this->m_flyingMountHeight - height) < 0.001) {
        return false;
    }

    this->m_state |= 0x400000;

    this->m_smoothFlyingHeight.startValue = this->m_flyingMountHeight;
    this->m_smoothFlyingHeight.startTimeMs = time;
    this->m_smoothFlyingHeight.target = height;
    this->m_smoothFlyingHeight.rate = rate;

    return true;
}

// OFFSET: 0x601550
bool CGCamera::SmoothSetFlyingMountHeight(float height, float delay, float rateScale, int32_t time) {
    if ((this->m_state & 0x400000) && fabs(this->m_smoothFlyingHeight.target - height) < 0.001 && fabs(this->m_smoothFlyingHeight.param4 - delay) < 0.001 && fabs(this->m_smoothFlyingHeight.param3 - rateScale) < 0.001) {
        return true;
    }

    if (fabs(this->m_flyingMountHeight - height) < 0.001) {
        return false;
    }

    this->m_smoothFlyingHeight.param4 = delay;
    this->m_smoothFlyingHeight.param3 = rateScale;

    float rate = fabs(height - this->m_flyingMountHeight) / s_cvCameraFlyingMountHeightSmoothSpeed->m_floatValue * rateScale;

    return this->StartSmoothFlyingMountHeight(height, rate, time + delay * 1000.0f);
}

// OFFSET: 0x5FEA40
bool CGCamera::StartSmoothPitch(float pitch, float rate, int32_t time) {
    float current = UnwrapAngleToward(pitch, this->m_pitch, -3.1415927f, 3.1415927f);

    this->m_pitch = current;

    if (fabs(current - pitch) < 0.001) {
        return false;
    }

    this->m_state |= 0x2000000;

    this->m_smoothPitch.startValue = current;
    this->m_smoothPitch.startTimeMs = time;
    this->m_smoothPitch.target = pitch;
    this->m_smoothPitch.rate = rate;

    return true;
}

// OFFSET: 0x601190
bool CGCamera::SetDesiredPitchAngle(float pitch, float delay, float rateScale, int32_t time) {
    float current = UnwrapAngleToward(pitch, this->m_pitch, -3.1415927f, 3.1415927f);

    this->m_pitch = current;

    if ((this->m_state & 0x2000000) && fabs(this->m_smoothPitch.target - pitch) < 0.001 && fabs(this->m_smoothPitch.param4 - delay) < 0.001 && fabs(this->m_smoothPitch.param3 - rateScale) < 0.001) {
        return true;
    }

    if (fabs(current - pitch) < 0.001) {
        return false;
    }

    this->m_smoothPitch.param4 = delay;
    this->m_smoothPitch.param3 = rateScale;

    float rate = rateScale * (fabs(pitch - current) / (s_cvCameraPitchSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothPitch(pitch, rate, time + delay * 1000.0f);
}

// OFFSET: 0x5FEB60
bool CGCamera::StartSmoothYaw(float yaw, float rate, int32_t time) {
    float current = UnwrapAngleToward(yaw, this->m_yaw, -3.1415927f, 3.1415927f);

    this->m_yaw = current;

    if (fabs(current - yaw) < 0.001) {
        return false;
    }

    this->m_state |= 0x1000000;

    this->m_smoothYaw.startValue = current;
    this->m_smoothYaw.startTimeMs = time;
    this->m_smoothYaw.target = yaw;
    this->m_smoothYaw.rate = rate;

    return true;
}

// OFFSET: 0x601410
bool CGCamera::SetDesiredYawAngle(float yaw, float delay, float rateScale, int32_t time) {
    float current = UnwrapAngleToward(yaw, this->m_yaw, -3.1415927f, 3.1415927f);

    this->m_yaw = current;

    if ((this->m_state & 0x1000000) && fabs(this->m_smoothYaw.target - yaw) < 0.001 && fabs(this->m_smoothYaw.param4 - delay) < 0.001 && fabs(this->m_smoothYaw.param3 - rateScale) < 0.001) {
        return true;
    }

    if (fabs(current - yaw) < 0.001) {
        return false;
    }

    this->m_smoothYaw.param4 = delay;
    this->m_smoothYaw.param3 = rateScale;

    float rate = rateScale * (fabs(yaw - current) / (s_cvCameraYawSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothYaw(yaw, rate, time + delay * 1000.0f);
}

// OFFSET: 0x5FEAD0
bool CGCamera::StartSmoothTargetOffset(float offset, float rate, int32_t time) {
    float current = UnwrapAngleToward(offset, this->m_targetOffset, -3.1415927f, 3.1415927f);

    this->m_targetOffset = current;

    if (fabs(current - offset) < 0.001) {
        return false;
    }

    this->m_state |= 0x8000000;

    this->m_smoothTargetOffset.startValue = current;
    this->m_smoothTargetOffset.startTimeMs = time;
    this->m_smoothTargetOffset.target = offset;
    this->m_smoothTargetOffset.rate = rate;

    return true;
}

// OFFSET: 0x6012D0
bool CGCamera::SetDesiredTargetOffset(float offset, float delay, float rateScale, int32_t time) {
    float current = UnwrapAngleToward(offset, this->m_targetOffset, -3.1415927f, 3.1415927f);

    this->m_targetOffset = current;

    if ((this->m_state & 0x8000000) && fabs(this->m_smoothTargetOffset.target - offset) < 0.001 && fabs(this->m_smoothTargetOffset.param4 - delay) < 0.001 && fabs(this->m_smoothTargetOffset.param3 - rateScale) < 0.001) {
        return true;
    }

    if (fabs(current - offset) < 0.001) {
        return false;
    }

    this->m_smoothTargetOffset.param4 = delay;
    this->m_smoothTargetOffset.param3 = rateScale;

    float rate = rateScale * (fabs(offset - current) / (s_cvCameraTargetSmoothSpeed->m_floatValue * 0.017453292f));

    return this->StartSmoothTargetOffset(offset, rate, time + delay * 1000.0f);
}

// OFFSET: 0x6000E0
void CGCamera::UpdateMotion(uint32_t time) {
    float distanceMin = 0.0f;

    if (this->m_flags & 0x8) {
        distanceMin = this->m_overrideDistanceMin;
    } else if (CGBarberShop::m_barberShopEnabled) {
        distanceMin = CGBarberShop::GetMinCameraDistance();
    }

    float distanceMax;

    if (this->m_flags & 0x10) {
        distanceMax = this->m_overrideDistanceMax;
    } else if (this->m_vehicleZoomEnabled) {
        distanceMax = MAX_CAMERA_DISTANCE;
    } else {
        distanceMax = s_cvCameraDistanceMaxFactor->m_floatValue * s_cvCameraDistanceMax->m_floatValue;

        if (distanceMax < 0.0f) {
            distanceMax = 0.0f;
        } else if (distanceMax >= MAX_CAMERA_DISTANCE) {
            distanceMax = MAX_CAMERA_DISTANCE;
        }
    }

    this->m_state |= 0x40;

    for (int32_t motion = 0; motion < NUM_CAMERA_MOTIONS; motion++) {
        int32_t activeBit = 2 * motion;

        if (!((1 << activeBit) & this->m_motionFlags)) {
            continue;
        }

        uint32_t deadline = this->m_motionDeadline[motion];

        if (deadline && static_cast<int32_t>(time - deadline) >= 0) {
            this->m_motionEndTime[motion] = deadline;
            this->m_motionFlags |= 1 << (activeBit + 1);
        }

        int32_t elapsed;

        if ((1 << (activeBit + 1)) & this->m_motionFlags) {
            elapsed = this->m_motionEndTime[motion] - this->m_motionTime[motion];

            if (elapsed < 0) {
                elapsed = 0;
            }

            this->m_motionFlags &= ~(3 << activeBit);

            if (!(this->m_motionFlags & (1 << activeBit))) {
                this->m_flags &= ~0x40;
            }
        } else {
            elapsed = time - this->m_motionTime[motion];

            if (elapsed < 0) {
                continue;
            }

            this->m_motionTime[motion] = time;
        }

        switch (motion) {
        case CAMERA_MOTION_ZOOM_IN:
            this->m_smoothDistance.target -= s_cvCameraDistanceMoveSpeed->m_floatValue * this->m_motionSpeed[motion] * elapsed * 0.001f;

            if (this->m_smoothDistance.target < distanceMin) {
                this->m_smoothDistance.target = distanceMin;
            }

            if (!(this->m_state & 0x4000000)) {
                this->m_distance = this->m_smoothDistance.target;
            }

            break;

        case CAMERA_MOTION_ZOOM_OUT:
            this->m_smoothDistance.target += s_cvCameraDistanceMoveSpeed->m_floatValue * this->m_motionSpeed[motion] * elapsed * 0.001f;

            if (this->m_smoothDistance.target > distanceMax) {
                this->m_smoothDistance.target = distanceMax;
            }

            if (!(this->m_state & 0x4000000)) {
                this->m_distance = this->m_smoothDistance.target;
            }

            break;

        case CAMERA_MOTION_YAW_LEFT:
            this->m_smoothYaw.target += s_cvCameraYawMoveSpeed->m_floatValue * this->m_motionSpeed[motion] * elapsed * 0.001f * 0.017453292f;

            while (this->m_smoothYaw.target > 6.2831855f) {
                this->m_smoothYaw.target -= 6.2831855f;
            }

            if (!(this->m_state & 0x1000000)) {
                this->m_yaw = this->m_smoothYaw.target;
            }

            break;

        case CAMERA_MOTION_YAW_RIGHT:
            this->m_smoothYaw.target -= s_cvCameraYawMoveSpeed->m_floatValue * this->m_motionSpeed[motion] * elapsed * 0.001f * 0.017453292f;

            while (this->m_smoothYaw.target < 0.0f) {
                this->m_smoothYaw.target += 6.2831855f;
            }

            if (!(this->m_state & 0x1000000)) {
                this->m_yaw = this->m_smoothYaw.target;
            }

            break;

        case CAMERA_MOTION_PITCH_UP:
            this->m_smoothPitch.target += s_cvCameraPitchMoveSpeed->m_floatValue * this->m_motionSpeed[motion] * elapsed * 0.001f * 0.017453292f;

            if (this->m_smoothPitch.target > MAX_PITCH_ANGLE) {
                this->m_smoothPitch.target = MAX_PITCH_ANGLE;
            }

            if (!(this->m_state & 0x2000000)) {
                this->m_pitch = this->m_smoothPitch.target;
            }

            break;

        case CAMERA_MOTION_PITCH_DOWN:
            this->m_smoothPitch.target -= s_cvCameraPitchMoveSpeed->m_floatValue * this->m_motionSpeed[motion] * elapsed * 0.001f * 0.017453292f;

            if (this->m_smoothPitch.target < MIN_PITCH_ANGLE) {
                this->m_smoothPitch.target = MIN_PITCH_ANGLE;
            }

            if (!(this->m_state & 0x2000000)) {
                this->m_pitch = this->m_smoothPitch.target;
            }

            break;

        default:
            break;
        }
    }
}

// OFFSET: 0x5FE580
void CGCamera::StartMotion(CAMERA_MOTION motion, uint32_t time, uint32_t duration, float speed) {
    if (this->m_flags & 0x40) {
        return;
    }

    uint32_t activeBit = 1 << (2 * motion);

    if (!(this->m_motionFlags & activeBit)) {
        this->m_motionFlags |= activeBit;
        this->m_motionTime[motion] = time;
    }

    this->m_motionSpeed[motion] = speed;

    if (duration) {
        this->m_motionDeadline[motion] = duration + time;
    } else {
        this->m_motionDeadline[motion] = 0;
    }
}

// OFFSET: 0x5FF950
void CGCamera::ZoomIn(float distance, int32_t time, float duration) {
    if (this->m_flags & 0x40) {
        return;
    }

    float speed = 1.0f;
    int32_t motionDuration;

    if (duration > 0.0f) {
        motionDuration = duration * 1000.0f;
        speed = (distance * 1000.0f) / (1000.0f * (duration * s_cvCameraDistanceMoveSpeed->m_floatValue));
    } else {
        motionDuration = distance / s_cvCameraDistanceMoveSpeed->m_floatValue * 1000.0f;
    }

    if (this->m_motionFlags & (1 << (2 * CAMERA_MOTION_ZOOM_OUT))) {
        this->m_motionEndTime[CAMERA_MOTION_ZOOM_OUT] = time;
        this->m_motionFlags |= 1 << (2 * CAMERA_MOTION_ZOOM_OUT + 1);
    }

    uint32_t deadline = this->m_motionDeadline[CAMERA_MOTION_ZOOM_IN];

    if (deadline && (this->m_motionFlags & (1 << (2 * CAMERA_MOTION_ZOOM_IN)))) {
        this->m_motionDeadline[CAMERA_MOTION_ZOOM_IN] = motionDuration + deadline;
    } else {
        this->StartMotion(CAMERA_MOTION_ZOOM_IN, time, motionDuration, speed);
    }
}

// OFFSET: 0x5FFA60
void CGCamera::ZoomOut(float distance, int32_t time, float duration) {
    if (this->m_flags & 0x40) {
        return;
    }

    float speed = 1.0f;
    int32_t motionDuration;

    if (duration > 0.0f) {
        motionDuration = duration * 1000.0f;
        speed = (distance * 1000.0f) / (1000.0f * (duration * s_cvCameraDistanceMoveSpeed->m_floatValue));
    } else {
        motionDuration = distance / s_cvCameraDistanceMoveSpeed->m_floatValue * 1000.0f;
    }

    if (this->m_motionFlags & (1 << (2 * CAMERA_MOTION_ZOOM_IN))) {
        this->m_motionEndTime[CAMERA_MOTION_ZOOM_IN] = time;
        this->m_motionFlags |= 1 << (2 * CAMERA_MOTION_ZOOM_IN + 1);
    }

    uint32_t deadline = this->m_motionDeadline[CAMERA_MOTION_ZOOM_OUT];

    if (deadline && (this->m_motionFlags & (1 << (2 * CAMERA_MOTION_ZOOM_OUT)))) {
        this->m_motionDeadline[CAMERA_MOTION_ZOOM_OUT] = motionDuration + deadline;
    } else {
        this->StartMotion(CAMERA_MOTION_ZOOM_OUT, time, motionDuration, speed);
    }
}

// OFFSET: 0x603090
void CGCamera::GetSafeWorldPos(C3Vector& pos, CGObject_C* target) {
    // if (this->m_vehicleCamera) {
    //     if (!(this->m_vehicleCamera->m_flags & 0x40)) {
    //         this->m_vehicleCamera->ComputeSafeCurWorldPos();
    //     }
    //     pos = this->m_vehicleCamera->m_position;
    //     return;
    // }

    target->GetPosition(pos);
}

// OFFSET: 0x600970
void CGCamera::UpdateVehicleTarget(int32_t a2) {
    // if (this->m_vehicleCamera) {
    //     this->m_vehicleCamera->Update(a2);
    //
    //     if (this->m_vehicleCamera) {
    //         WGUID relativeTo = this->m_vehicleCamera->GetRelativeTo();
    //
    //         if (!relativeTo || ClntObjMgrObjectPtr(relativeTo, TYPEMASK_OBJECT)) {
    //             this->m_relativeTo = relativeTo;
    //         }
    //     }
    // }
}

// OFFSET: 0x5FFDE0
int32_t CGCamera::CanSmoothTargetFacing(CGObject_C* target) {
    if (!target) {
        return 0;
    }

    if (!(target->m_obj->m_type & TYPEMASK_UNIT)) {
        return 0;
    }

    if (!s_cvCameraPivot->m_intValue) {
        return 0;
    }

    uint32_t flags = target->AsUnit()->m_passenger->m_flags;

    if (flags & 0xF) {
        return 0;
    }

    if (this->m_pitch > 0.0f) {
        return 0;
    }

    uint32_t state = this->m_state;

    if (state & 0x8) {
        return 0;
    }

    if (flags & 0x2000000) {
        return state & 0x10000;
    }

    return state & 0x30000;
}

// OFFSET: 0x602600
int32_t CGCamera::CanSmoothTarget() {
    if (fabs(this->m_targetOffset) < 0.001) {
        return 0;
    }

    if (this->m_state & 0x100) {
        if (this->unk_02FC == 1) {
            return 0;
        }

        if (this->unk_02FC != 2) {
            if (!s_cvCameraSmoothTrackingStyle->m_intValue /* || !CGUnit_C::GetTrackingType() */) {
                return 0;
            }
        }
    }

    auto target = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

    return this->CanSmoothTargetFacing(target) == 0;
}

// OFFSET: 0x6019B0
void CGCamera::IncIgnoreFacing() {
    int32_t refs = this->m_ignoreFacingRefs;

    this->m_ignoreFacingRefs = refs + 1;

    if (refs) {
        return;
    }

    CGObject_C* target = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

    if (!target) {
        this->UpdateYaw(0.0f);
        return;
    }

    //if ((target->m_obj->m_type & TYPEMASK_UNIT) != 0) {
    //    if (this->m_vehicleCamera) {
    //        this->UpdateYaw(this->m_vehicleCamera->m_facing);
    //    } else {
    //        this->UpdateYaw(target->AsUnit()->dataAA0);
    //    }
    //} else {
        this->UpdateYaw(target->GetFacing());
    //}
}

// OFFSET: 0x601A70
void CGCamera::DecIgnoreFacing() {
    if (this->m_ignoreFacingRefs-- != 1) {
        return;
    }

    CGObject_C* target = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

    if (!target) {
        this->UpdateYaw(-0.0f);
        return;
    }

    if ((target->m_obj->m_type & TYPEMASK_UNIT) == 0) {
        this->UpdateYaw(-target->GetFacing());
        return;
    }

    CGUnit_C* unit = target->AsUnit();

    //if (this->m_vehicleCamera && this->m_vehicleCamera->IsHierarchyChasingFacing()) {
    //    this->UpdateYaw(-this->sub_6009E0(unit));
    //    return;
    //}
    //
    //if ((this->m_state & 0x4000) != 0) {
    //    this->UpdateYaw(-this->unk_01DC);
    //    return;
    //}
    //
    //if (this->m_vehicleCamera) {
    //    this->UpdateYaw(-this->sub_6009E0(unit));
    //} else {
        this->UpdateYaw(-unit->GetRawFacing());
    //}
}

// OFFSET: 0x5FFC20
void CGCamera::ClampPitchToLimits(float delta) {
    float target = delta + this->m_smoothPitch.target;

    if (target < MIN_PITCH_ANGLE) {
        target = MIN_PITCH_ANGLE;
    } else if (target >= MAX_PITCH_ANGLE) {
        target = MAX_PITCH_ANGLE;
    }

    this->m_smoothPitch.target = target;

    float start = delta + this->m_smoothPitch.startValue;

    if (start < MIN_PITCH_ANGLE) {
        start = MIN_PITCH_ANGLE;
    } else if (start >= MAX_PITCH_ANGLE) {
        start = MAX_PITCH_ANGLE;
    }

    this->m_smoothPitch.startValue = UnwrapAngleToward(target, start, -3.1415927f, 3.1415927f);

    float pitch = this->m_pitch + delta;

    if (pitch < MIN_PITCH_ANGLE) {
        pitch = MIN_PITCH_ANGLE;
    } else if (pitch >= MAX_PITCH_ANGLE) {
        pitch = MAX_PITCH_ANGLE;
    }

    this->m_pitch = UnwrapAngleToward(target, pitch, -3.1415927f, 3.1415927f);
}

// OFFSET: 0x5FF530
void CGCamera::ClampPitchAndNormalize() {
    float pitch = this->m_pitch;

    if (pitch < MIN_PITCH_ANGLE) {
        pitch = MIN_PITCH_ANGLE;
    } else if (pitch >= MAX_PITCH_ANGLE) {
        pitch = MAX_PITCH_ANGLE;
    }

    this->m_pitch = pitch;

    this->m_yaw = CMath::normalizeangle0to2pi(this->m_yaw);
    this->m_yawOffset = CMath::normalizeangle0to2pi(this->m_yawOffset);
}

// OFFSET: 0x5FE5F0
void CGCamera::UpdateYaw(float delta) {
    this->m_smoothYaw.target = CMath::normalizeangle0to2pi(delta + this->m_smoothYaw.target);

    float start = CMath::normalizeangle0to2pi(delta + this->m_smoothYaw.startValue);

    this->m_smoothYaw.startValue = UnwrapAngleToward(this->m_smoothYaw.target, start, -3.1415927f, 3.1415927f);

    float yaw = CMath::normalizeangle0to2pi(this->m_yaw + delta);

    this->m_yaw = UnwrapAngleToward(this->m_smoothYaw.target, yaw, -3.1415927f, 3.1415927f);
}

// OFFSET: 0x601FF0
void CGCamera::SetModeFreeLook() {
    uint32_t state = this->m_state;

    if ((state & 0x1) != 0) {
        return;
    }

    this->m_state = state | 0x1;

    this->IncIgnoreFacing();

    int32_t refs = this->unk_00B0;

    this->unk_00B0 = refs + 1;

    if (!refs) {
        this->ClampPitchToLimits(this->m_groundTilt);
    }

    this->m_state &= 0xF4FFBFFF;
    this->m_flags &= ~0x4u;

    this->m_smoothPitch.target = this->m_pitch;
    this->m_smoothPitch.startTimeMs = 0;
    this->m_smoothPitch.rate = 0.0f;

    this->m_smoothTargetOffset.target = this->m_targetOffset;
    this->m_smoothTargetOffset.startTimeMs = 0;
    this->m_smoothTargetOffset.rate = 0.0f;

    this->m_smoothYaw.target = this->m_yaw;
    this->m_smoothYaw.startTimeMs = 0;
    this->m_smoothYaw.rate = 0.0f;
}

// OFFSET: 0x601F70
void CGCamera::SetModeNormal() {
    uint32_t state = this->m_state;

    if ((state & 0x1) == 0 || (this->m_flags & 0x2) != 0) {
        return;
    }

    state &= ~0x1u;
    this->m_state = state;

    //if (this->m_vehicleCamera && (this->m_vehicleCamera->m_flags & 0x10) != 0) {
    //    this->m_state = state & ~0x4000u;
    //}

    this->DecIgnoreFacing();

    if (--this->unk_00B0 == 0) {
        this->ClampPitchToLimits(-this->m_groundTilt);
    }

    this->m_state &= ~0x4000u;
}

// OFFSET: 0x6047E0
void CGCamera::EnableFreeLook() {
    CGObject_C* target = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

    if (target && (target->m_obj->m_type & TYPEMASK_PLAYER) != 0) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(target);

        if ((player->m_unit->UNIT_FIELD_FLAGS & 0x100000) != 0 && (player->m_player->PLAYER_FLAGS & 0x20000) != 0) {
            return;
        }
    }

    this->SetModeFreeLook();
}

// OFFSET: 0x604850
void CGCamera::DisableFreeLook(int32_t a2) {
    this->SetModeNormal();

    if (a2) {
        this->m_state |= 0x20u;
    } else {
        this->m_state &= ~0x20u;
    }

    this->m_smoothPitch.target = this->m_pitch;
    this->m_smoothYaw.target = this->m_yaw;
    this->m_smoothTargetOffset.target = this->m_targetOffset;
}

// OFFSET: 0x6020B0
void CGCamera::UpdateFreeLookFacing(float dx, float dy, float* outPitch) {
    if (outPitch) {
        *outPitch = 0.0f;
    }

    if ((this->m_state & 0x8000) != 0) {
        return;
    }

    CGObject_C* target = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

    if (target && (target->m_obj->m_type & TYPEMASK_PLAYER) != 0) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(target);

        if ((player->m_unit->UNIT_FIELD_FLAGS & 0x100000) != 0 && (player->m_player->PLAYER_FLAGS & 0x20000) != 0) {
            return;
        }
    }

    this->m_state |= 0x40u;

    float pitchSpeed = s_cvCameraPitchMoveSpeed->m_floatValue;
    float yawSpeed = s_cvCameraYawMoveSpeed->m_floatValue;
    float pivotDXMax = s_cvCameraPivotDXMax->m_floatValue;
    float pivotDYMin = s_cvCameraPivotDYMin->m_floatValue;

    DDCToNDC(dx, dy, &dx, &dy);

    float dyaw = yawSpeed * 0.017453292f * (dx * 0.00125f);
    float dpitch = 0.017453292f * pitchSpeed * (dy * 0.0016666667f);

    float pitchSign = s_cvMouseInvertPitch->m_intValue ? -1.0f : 1.0f;
    float yawSign = s_cvMouseInvertYaw->m_intValue ? -1.0f : 1.0f;

    if (outPitch) {
        *outPitch = dpitch * pitchSign;
        dpitch = 0.0f;
    }

    int32_t canSmooth = this->CanSmoothTargetFacing(target);

    uint32_t state = this->m_state;

    int32_t usePivot = 0;

    if ((state & 0x8000000) == 0 && fabs(this->m_targetOffset) >= 0.001f) {
        usePivot = 1;
    }

    if (canSmooth) {
        if (fabs(dpitch) > pivotDYMin && fabs(dyaw) < pivotDXMax) {
            usePivot = 1;
        }

        if (dpitch > 0.0f) {
            bool offsetIsZero = fabs(this->m_targetOffset) < 0.00000023841858f;

            if (this->m_targetOffset < 0.0f && dpitch * pitchSign + this->m_targetOffset > 0.0f) {
                dpitch += this->m_targetOffset;
                usePivot = 0;
                this->m_targetOffset = 0.0f;
                this->m_smoothTargetOffset.target = 0.0f;
            } else if (offsetIsZero) {
                usePivot = 0;
                this->m_targetOffset = 0.0f;
                this->m_smoothTargetOffset.target = 0.0f;
            }
        }
    }

    int32_t reset = usePivot == 0;

    if (usePivot) {
        this->m_targetOffset = dpitch * pitchSign + this->m_targetOffset;

        if (this->m_pitch >= 0.0f) {
            reset = 1;
        } else {
            float floorOffset = MIN_PITCH_ANGLE - this->m_pitch;

            if (this->m_targetOffset < floorOffset) {
                this->m_targetOffset = floorOffset;
            }
        }

        if (this->m_targetOffset > 0.0f) {
            reset = 1;
        }
    }

    if (reset) {
        this->SetDesiredTargetOffset(0.0f, 0.0f, 1.0f, OsGetAsyncTimeMs());
        this->ClampPitchToLimits(pitchSign * dpitch);
    } else {
        this->m_smoothTargetOffset.target = this->m_targetOffset;
        this->m_smoothTargetOffset.startTimeMs = 0;
        this->m_smoothTargetOffset.rate = 0.0f;
        this->m_state = state & 0xF7FFFFFF;
    }

    this->UpdateYaw(-(yawSign * dyaw));

    this->ClampPitchAndNormalize();
}

// OFFSET: 0x5FFEB0
bool CGCamera::ShouldSmoothPitch(float pitchMin, float pitchMax) {
    CGUnit_C* mover = ClntObjMgrObjectPtr<CGUnit_C*>(this->m_targetGUID, TYPEMASK_UNIT);

    return mover
        && (this->m_state & 0x1) == 0
        && s_cvCameraSmoothPitch->m_intValue
        && (mover->m_passenger->m_flags & 0x2200000) == 0
        && (pitchMin > this->m_pitch || pitchMax < this->m_pitch);
}

// OFFSET: 0x602680
bool CGCamera::CanSmoothYaw(float yawMin, float yawMax) {
    CGObject_C* target = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

    if (!target) {
        return false;
    }

    uint32_t state = this->m_state;

    if ((state & 0x1) != 0 || !s_cvCameraSmoothYaw->m_intValue) {
        return false;
    }

    if (this->m_ignoreFacingRefs <= 0) {
        return yawMin > this->m_yaw || yawMax < this->m_yaw;
    }

    float facing;

    //if ((target->m_obj->m_type & TYPEMASK_UNIT) != 0) {
    //    if ((state & 0x100) != 0) {
    //        facing = CGUnit_C::GetTrackingTurn();
    //    } else {
    //        static_cast<CGUnit_C*>(target)->UpdateSmoothFacing(0);
    //        facing = this->GetChaseFacing(target);
    //    }
    //} else {
        facing = target->GetFacing();
    //}

    return CMath::fnotequal(this->m_yaw, facing);
}

// OFFSET: 0x602760
void CGCamera::SmoothFreeLook(CGInputControl* input, int32_t settle) {
    if (!input) {
        return;
    }

    uint32_t state = this->m_state;
    uint32_t flags = input->m_flags;

    uint32_t reasons = 0;

    if ((state & 0x1000) != 0) {
        reasons |= 0x40;
    }

    if ((flags & 0x300) != 0 || (flags & 0x2000001) != 0) {
        reasons |= 0x20;
    }

    if ((flags & 0xC0) != 0 || ((flags & 0x2000001) != 0 && (flags & 0x300) != 0)) {
        reasons |= 0x10;
    }

    if ((flags & 0x1030) != 0 || ((flags & 0x1) != 0 && (flags & 0x2) != 0)) {
        reasons |= 0x08;
    }

    if ((state & 0x100) != 0) {
        if (CGUnit_C::GetTrackingType() == 3 && (this->m_flags & 0x1) == 0) {
            reasons |= 0x08;
        } else {
            reasons |= 0x04;
        }
    }

    if (settle) {
        reasons |= 0x02;
    }

    if ((flags & 0x1030) == 0 && (flags & 0xC0) == 0 && ((flags & 0x2000001) == 0 || (flags & 0x300) == 0) && ((flags & 0x300) == 0 || (flags & 0x2000001) != 0) && (flags & 0x1E00000) == 0) {
        reasons |= 0x01;
    }

    this->m_state &= ~0x4000u;

    reasons &= 0x7F;

    if (!reasons || !this->IsCustomViewSmoothingActive()) {
        return;
    }

    int32_t now = OsGetAsyncTimeMs();

    uint32_t style;

    if ((reasons & 0x44) != 0) {
        if (this->unk_02FC == 1) {
            style = 0;
        } else if (this->unk_02FC == 2) {
            style = 3;
        } else {
            style = s_cvCameraSmoothTrackingStyle->m_intValue;
        }
    } else {
        style = s_cvCameraSmoothStyle->m_intValue;
    }

    float delayOffset = 0.0f;
    float rateScale = 0.0f;

    int32_t bit = 7;

    while (--bit >= 0) {
        if (((1 << bit) & reasons) != 0 && style < 5) {
            delayOffset = s_cvCameraSmoothState[style][bit][0]->m_floatValue;

            if (delayOffset < 0.0f) {
                delayOffset = 0.0f;
            } else if (delayOffset >= 100.0f) {
                delayOffset = 99.0f;
            }

            rateScale = s_cvCameraSmoothState[style][bit][1]->m_floatValue;

            if (rateScale < 0.0f) {
                rateScale = 0.0f;
            } else if (rateScale >= 100.0f) {
                rateScale = 99.0f;
            }

            break;
        }
    }

    float axis[6];

    for (int32_t row = 2; row >= 0; row--) {
        if (style >= 5) {
            continue;
        }

        for (int32_t col = 1; col >= 0; col--) {
            float value = s_cvCameraSmoothViewData[style][row][col]->m_floatValue;

            axis[2 * row + col] = value;

            if (col == 1) {
                axis[2 * row + 1] = value * rateScale;
            } else {
                axis[2 * row] = value + delayOffset;
            }

            if (axis[2 * row + col] < 0.0f) {
                axis[2 * row + col] = 0.0f;
            }

            if (axis[2 * row + col] >= 100.0f) {
                axis[2 * row + col] = 99.0f;
            }
        }
    }

    float pitchMin = s_cvCameraPitchSmoothMin->m_floatValue * 0.017453292f;
    float pitchMax = s_cvCameraPitchSmoothMax->m_floatValue * 0.017453292f;
    float yawMin = s_cvCameraYawSmoothMin->m_floatValue * 0.017453292f;
    float yawMax = 0.017453292f * s_cvCameraYawSmoothMax->m_floatValue;

    bool doPitch = this->ShouldSmoothPitch(pitchMin, pitchMax);
    bool doTarget = this->CanSmoothTarget();
    bool doYaw = this->CanSmoothYaw(yawMin, yawMax);

    float maxRate = 0.0f;

    if (doPitch) {
        if (axis[3] == 0.0f) {
            this->CancelSmoothPitch();
            doPitch = false;
        } else {
            float target = this->m_views[this->m_viewIndex].pitch;

            if (pitchMin > this->m_pitch) {
                target = pitchMin;
            }

            if (pitchMax < this->m_pitch) {
                target = pitchMax;
            }

            doPitch = this->SetDesiredPitchAngle(target, axis[2], axis[3], now);

            if (doPitch && this->m_smoothPitch.rate >= 0.0f) {
                maxRate = this->m_smoothPitch.rate;
            }
        }
    }

    if (doTarget) {
        if (rateScale == 0.0f) {
            this->CancelSmoothTargetOffset();
            doTarget = false;
        } else {
            doTarget = this->SetDesiredTargetOffset(0.0f, delayOffset, rateScale, now);

            if (doTarget && maxRate <= this->m_smoothTargetOffset.rate) {
                maxRate = this->m_smoothTargetOffset.rate;
            }
        }
    }

    if (doYaw) {
        if (axis[5] == 0.0f) {
            this->CancelSmoothYaw();
            doYaw = false;
        } else {
            float base = this->m_views[this->m_viewIndex].yaw;
            float target = base;

            if (this->m_ignoreFacingRefs <= 0) {
                if (yawMin > this->m_yaw) {
                    target = yawMin;
                }

                if (yawMax < this->m_yaw) {
                    target = yawMax;
                }
            } else {
                CGObject_C* obj = ClntObjMgrObjectPtr<CGObject_C*>(this->m_targetGUID, TYPEMASK_OBJECT);

                //if (obj && (obj->m_obj->m_type & TYPEMASK_UNIT) != 0) {
                //    if ((this->m_state & 0x100) != 0) {
                //        target = CMath::normalizeangle0to2pi(CGUnit_C::GetTrackingTurn() + base);
                //    } else {
                //        obj->AsUnit()->UpdateSmoothFacing(0);
                //
                //        if (input->m_facingOverrideActive) {
                //            target = CMath::normalizeangle0to2pi(input->m_facingOverride + base);
                //        } else {
                //            target = CMath::normalizeangle0to2pi(this->GetChaseFacing(obj) + base);
                //        }
                //    }
                //} else {
                    target = CMath::normalizeangle0to2pi(obj->GetFacing() + base);
                //}
            }

            doYaw = this->SetDesiredYawAngle(target, axis[4], axis[5], now);

            if (doYaw && maxRate <= this->m_smoothYaw.rate) {
                maxRate = this->m_smoothYaw.rate;
            }
        }
    }

    float duration = maxRate;

    if (duration < s_cvCameraSmoothTimeMin->m_floatValue) {
        duration = s_cvCameraSmoothTimeMin->m_floatValue;
    }

    if (duration > s_cvCameraSmoothTimeMax->m_floatValue) {
        duration = s_cvCameraSmoothTimeMax->m_floatValue;
    }

    if (doYaw) {
        this->m_smoothYaw.rate = duration;
    }

    if (doPitch) {
        this->m_smoothPitch.rate = duration;
    }

    if (doTarget) {
        this->m_smoothTargetOffset.rate = duration;
    }
}

// OFFSET: 0x6009E0
float CGCamera::GetChaseFacing(CGObject_C* target) {
    //m_vehicleCamera = this->m_vehicleCamera;
    //if (m_vehicleCamera)
    //    return *(m_vehicleCamera + 132);
    //else
    //    return target->dataAA0;
    WHOA_UNIMPLEMENTED(0.0f);
}

// OFFSET: 0x5FFF40
bool CGCamera::IsCustomViewSmoothingActive() {
    return true;
    return (this->m_state & 0x20) == 0 && (s_cvCameraCustomViewSmoothing->m_intValue || this->CheckViewSmoothingCVarsChanged(this->m_viewIndex)) && (this->m_state & 0x8000) == 0;
}

// OFFSET: 0x5FEF10
void CGCamera::CancelSmoothTargetOffset() {
    this->m_state &= ~0x8000000u;
    this->m_smoothTargetOffset.target = this->m_targetOffset;
    this->m_smoothTargetOffset.startTimeMs = 0;
    this->m_smoothTargetOffset.rate = 0.0;
}

// OFFSET: 0x5FEF40
void CGCamera::CancelSmoothYaw() {
    this->m_state &= ~0x1000000u;
    this->m_smoothYaw.target = this->m_yaw;
    this->m_smoothYaw.startTimeMs = 0;
    this->m_smoothYaw.rate = 0.0;
}

// OFFSET: 0x5FEEE0
void CGCamera::CancelSmoothPitch() {
    this->m_state &= ~0x2000000u;
    this->m_smoothPitch.target = this->m_pitch;
    this->m_smoothPitch.startTimeMs = 0;
    this->m_smoothPitch.rate = 0.0;
}

// OFFSET: 0x602EB0
void CGCamera::UpdateUncontrolledState(bool a2) {
    if (a2) {
        if ((this->m_state & 0x1000) == 0) {
            this->IncIgnoreFacing();
            this->m_state = this->m_state & 0xFFFFAFFF | 0x1000;
        }
        this->SmoothFreeLook(CGInputControl::GetActive(), 0);
    }
    if ((this->m_state & 0x1000) != 0 && !a2) {
        this->DecIgnoreFacing();
        this->m_state &= 0xFFFFAFFF;
        this->SmoothFreeLook(CGInputControl::GetActive(), 0);
    }
}

// OFFSET: 0x603D30
void CGCamera::UpdateTargetSmoothing(CGObject_C* target, int32_t time) {
    //if (this->HasTargetOffset()) {
    //    float t = (time - this->unk_0288) * 0.001f / this->unk_028C;
    //
    //    if (t < 1.0f) {
    //        this->m_bobOffset.x = OrganicSmooth(this->unk_0290.x, 0.0f, t);
    //        this->m_bobOffset.y = OrganicSmooth(this->unk_0290.y, 0.0f, t);
    //        this->m_bobOffset.z = OrganicSmooth(this->unk_0290.z, 0.0f, t);
    //    } else {
    //        this->m_bobOffset.x = 0.0f;
    //        this->m_bobOffset.y = 0.0f;
    //        this->m_bobOffset.z = 0.0f;
    //    }
    //}

    //if (fabs(this->m_smoothFoV.target - this->m_targetFov) < 0.00000023841858f) {
    //    this->m_state &= ~0x40000000u;
    //    this->m_smoothFoV.target = this->m_targetFov;
    //    this->m_smoothFoV.startTimeMs = 0;
    //    this->m_smoothFoV.rate = 0.0f;
    //} else if ((this->m_state & 0x40000000) != 0 && time - this->m_smoothFoV.startTimeMs >= 0) {
    //    float t = (time - this->m_smoothFoV.startTimeMs) * 0.001f / this->m_smoothFoV.rate;
    //
    //    if (t < 1.0f) {
    //        this->m_targetFov = OrganicSmooth(this->m_smoothFoV.startValue, this->m_smoothFoV.target, t);
    //    } else {
    //        this->m_targetFov = this->m_smoothFoV.target;
    //    }
    //}

    if (fabs(this->m_smoothDistance.target - this->m_distance) < 0.00000023841858f) {
        this->m_state &= ~0x4000000u;
        this->m_smoothDistance.target = this->m_distance;
        this->m_smoothDistance.startTimeMs = 0;
        this->m_smoothDistance.rate = 0.0f;
    } else if ((this->m_state & 0x4000000) != 0 && time - this->m_smoothDistance.startTimeMs >= 0) {
        float t = (time - this->m_smoothDistance.startTimeMs) * 0.001f / this->m_smoothDistance.rate;

        if (t < 1.0f) {
            this->m_distance = OrganicSmooth(this->m_smoothDistance.startValue, this->m_smoothDistance.target, t);
        } else {
            this->m_distance = this->m_smoothDistance.target;
        }
    }

    if (fabs(this->m_smoothHeight.target - this->m_height) < 0.00000023841858f) {
        this->m_state &= ~0x20000000u;
        this->m_smoothHeight.target = this->m_height;
        this->m_smoothHeight.startTimeMs = 0;
        this->m_smoothHeight.rate = 0.0f;
    } else if ((this->m_state & 0x20000000) != 0 && time - this->m_smoothHeight.startTimeMs >= 0) {
        float t = (time - this->m_smoothHeight.startTimeMs) * 0.001f / this->m_smoothHeight.rate;

        if (t < 1.0f) {
            this->m_height = OrganicSmooth(this->m_smoothHeight.startValue, this->m_smoothHeight.target, t);
        } else {
            this->m_height = this->m_smoothHeight.target;
        }
    }

    if (fabs(this->m_smoothGroundTilt.target - this->m_groundTilt) < 0.00000023841858f) {
        this->m_state &= ~0x10000000u;
        this->m_smoothGroundTilt.target = this->m_groundTilt;
        this->m_smoothGroundTilt.startTimeMs = 0;
        this->m_smoothGroundTilt.rate = 0.0f;
    } else if ((this->m_state & 0x10000000) != 0 && time - this->m_smoothGroundTilt.startTimeMs >= 0) {
        float t = (time - this->m_smoothGroundTilt.startTimeMs) * 0.001f / this->m_smoothGroundTilt.rate;

        if (t < 1.0f) {
            this->m_groundTilt = OrganicSmooth(this->m_smoothGroundTilt.startValue, this->m_smoothGroundTilt.target, t);
        } else {
            this->m_groundTilt = this->m_smoothGroundTilt.target;
        }
    }

    if (fabs(this->m_smoothTargetOffset.target - this->m_targetOffset) < 0.00000023841858f) {
        uint32_t state = this->m_state;

        if ((state & 0x80000000) == 0) {
            this->m_smoothTargetOffset.target = this->m_targetOffset;
            this->m_smoothTargetOffset.startTimeMs = 0;
            this->m_state = state & 0xF7FFFFFF;
            this->m_smoothTargetOffset.rate = 0.0f;
        }
    } else if ((this->m_state & 0x8000000) != 0 && time - this->m_smoothTargetOffset.startTimeMs >= 0) {
        float t = (time - this->m_smoothTargetOffset.startTimeMs) * 0.001f / this->m_smoothTargetOffset.rate;

        if (t < 1.0f) {
            this->m_targetOffset = OrganicSmooth(this->m_smoothTargetOffset.startValue, this->m_smoothTargetOffset.target, t);
        } else {
            this->m_targetOffset = this->m_smoothTargetOffset.target;
        }
    }

    if (fabs(this->m_smoothPitch.target - this->m_pitch) < 0.00000023841858f) {
        this->m_state &= ~0x2000000u;
        this->m_smoothPitch.target = this->m_pitch;
        this->m_smoothPitch.startTimeMs = 0;
        this->m_smoothPitch.rate = 0.0f;
    } else if ((this->m_state & 0x2000000) != 0 && time - this->m_smoothPitch.startTimeMs >= 0) {
        float t = (time - this->m_smoothPitch.startTimeMs) * 0.001f / this->m_smoothPitch.rate;

        if (t < 1.0f) {
            this->m_pitch = OrganicSmooth(this->m_smoothPitch.startValue, this->m_smoothPitch.target, t);
        } else {
            this->m_pitch = this->m_smoothPitch.target;
        }
    }

    uint32_t state = this->m_state;

    if ((state & 0x8) != 0 && (state & 0x1000000) != 0) {
        this->m_smoothYaw.startTimeMs = time;
    }

    if ((state & 0x1) == 0 && (state & 0x8) == 0) {
        if (fabs(this->m_smoothYaw.target - this->m_yaw) < 0.00000023841858f) {
            this->m_smoothYaw.target = this->m_yaw;
            this->m_smoothYaw.startTimeMs = 0;
            this->m_state = state & 0xFEFFFFFF;
            this->m_smoothYaw.rate = 0.0f;
        } else if ((state & 0x1000000) != 0 && time - this->m_smoothYaw.startTimeMs >= 0) {
            float t = (time - this->m_smoothYaw.startTimeMs) * 0.001f / this->m_smoothYaw.rate;

            if (t < 1.0f) {
                this->m_yaw = OrganicSmooth(this->m_smoothYaw.startValue, this->m_smoothYaw.target, t);
            } else {
                this->m_yaw = this->m_smoothYaw.target;
            }
        }
    }

    this->ClampPitchAndNormalize();

    if (fabs(this->m_smoothFlyingHeight.target - this->m_flyingMountHeight) < 0.00000023841858f) {
        this->m_state &= ~0x400000u;
        this->m_smoothFlyingHeight.target = this->m_flyingMountHeight;
        this->m_smoothFlyingHeight.startTimeMs = 0;
        this->m_smoothFlyingHeight.rate = 0.0f;
    } else if ((this->m_state & 0x400000) != 0 && time - this->m_smoothFlyingHeight.startTimeMs >= 0) {
        float t = (time - this->m_smoothFlyingHeight.startTimeMs) * 0.001f / this->m_smoothFlyingHeight.rate;

        if (t < 1.0f) {
            this->m_flyingMountHeight = OrganicSmooth(this->m_smoothFlyingHeight.startValue, this->m_smoothFlyingHeight.target, t);
        } else {
            this->m_flyingMountHeight = this->m_smoothFlyingHeight.target;
        }
    }

    //this->CalcTerrainTilt(target, time);
    //this->PerformTerrainTilt(target, time, 0);
}
