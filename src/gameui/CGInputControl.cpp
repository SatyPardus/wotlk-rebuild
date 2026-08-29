#include "gameui/CGInputControl.hpp"
#include "console/DebugScreen.hpp"
#include <common/time/Time.hpp>
#include <clientobject/Unit_C.hpp>
#include <clientobject/ObjectMgrClient.hpp>
#include <clientobject/Movement.hpp>

CGInputControl* CGInputControl::s_inputControl;

// OFFSET: 0x5FD2C0
void CGInputControl::Initialize() {
    //CVar::Register("Joystick", "enable joystick control", 0, "0", bn_JoystickCallback, 5, 0, 0, 0);
    //bnl_s_cvCinematicJoystick = CVar::Register("CinematicJoystick", "enable cinematic joystick control", 0, "0", 0, 5, 0, 0, 0);
    CGInputControl::s_inputControl = new (STORM_ALLOC(sizeof(CGInputControl))) CGInputControl();
    //s_cvEnableWowMouse = CVar::Register("enableWowMouse", "Enable Steelseries World of Warcraft Mouse", 1, "0", bn_WowMouseCVarCallback, 5, 0, 0, 0);
}

CGInputControl::CGInputControl() {
    this->m_flags = 0;
    //this->unk_0014 = 0;
    //this->unk_0018 = 0;
    //TSHashTable_MOUSELOOKBINDING::ctor(&this->m_mouseLookBindings.__vftable);
    //*&this->unk_0048 = 0.0;
    //*&this->unk_0050 = 0.0;
    //this->unk_0044 = 0;
    //this->unk_004C = 0;
    //this->unk_0058 = 3;
    //this->unk_005C = 0;
    //this->unk_0060 = 3;
    //*&this->unk_0064 = 0.0;
    //*&this->unk_0068 = 0.0;
    //this->unk_006C = 0;
    this->m_time = OsGetAsyncTimeMs();
}

// OFFSET: 0x5FBBC0
void CGInputControl::UpdatePlayer(int32_t eventTime, bool a3) {
    CGUnit_C* mover = ClntObjMgrObjectPtr<CGUnit_C*>(CGUnit_C::s_activeMover, TYPEMASK_UNIT);
    if (!mover)
        return;

    //if ((mover->m_obj->m_type == TYPE_PLAYER) != 0 && mover->IsCommentatorUberOrInArena()) {
    //    CGCommentator::UpdateCameraVelocity(this->m_flags, a3);
    //    return;
    //}

    uint32_t lastUpdateTime;
    if (MovementGetLastUpdateTime(&lastUpdateTime) && eventTime - lastUpdateTime < 0)
        eventTime = lastUpdateTime;

    bool canTurn = false;
    if (a3 && this->CanTurn(mover))
        canTurn = true;

    if (a3 && this->CanMove(mover)) {
        this->MovePlayer(eventTime, mover);
        this->StrafePlayer(eventTime, mover);
        this->AscendDescendPlayer(eventTime, mover);
    } else {
        //if (!canTurn)
        //    mover->ClearTrackingTarget(v6, 0, 1);
        if ((this->m_flags & 0x10000) != 0) {
            if (!mover->m_passenger->IsOnSpline())
                mover->OnMoveStopLocal(eventTime);
            this->m_flags &= ~0x10000u;
        }
        if ((this->m_flags & 0x20000) != 0) {
            if (!mover->m_passenger->IsOnSpline())
                mover->OnStrafeStopLocal(eventTime);
            this->m_flags &= ~0x20000u;
        }
        m_flags = this->m_flags;
        if ((m_flags & 0x1000) != 0)
            this->m_flags = m_flags & 0xFFFFEFFF;
        if ((this->m_flags & 0x100000) != 0) {
            if (!mover->m_passenger->IsOnSpline())
                mover->OnAscendDescendStopLocal(eventTime);
            this->m_flags &= ~0x100000u;
        }
    }

    if (canTurn) {
        this->TurnPlayer(eventTime, mover);
        if ((mover->m_passenger->m_flags & 0x2200000) != 0 || (mover->m_passenger->m_flags2 & 0x20) != 0)
            this->PitchPlayer(eventTime, mover);
    } else {
        if ((this->m_flags & 0x40000) != 0) {
            if (!mover->m_passenger->IsOnSpline())
                mover->OnTurnStopLocal(eventTime);
            this->m_flags &= ~0x40000u;
        }
        if ((this->m_flags & 0x80000) != 0) {
            if (!mover->m_passenger->IsOnSpline() && (mover->m_passenger->m_flags & 0x2200000) != 0)
                mover->OnPitchStopLocal(eventTime);
            this->m_flags &= ~0x80000u;
        }
    }
}

// OFFSET: 0x5FA170
bool CGInputControl::SetControlBit(uint32_t controlBit, int32_t eventTime) {
    if ((controlBit & this->m_flags) != 0)
        return false;

    this->m_flags |= controlBit;
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //m_flags = this->m_flags;
    //v22 = (m_flags & 0x1030) == 0 && (m_flags & 0xC0) == 0 && ((m_flags & 0x2000001) == 0 || (m_flags & 0x300) == 0) && ((m_flags & 0x300) == 0 || (m_flags & 0x2000001) != 0) && (m_flags & 0x1E00000) == 0;
    //v6 = m_flags & 0x6000003;
    //v21 = this->m_flags & 3;
    //if ((m_flags & 1) == 0 || (v24 = 1, (m_flags & 2) == 0))
    //    v24 = 0;
    //v20 = m_flags & 0x4000002;
    //v19 = m_flags & 0x2000001;
    //if ((m_flags & 0x2000001) == 0 || (v23 = 1, (m_flags & 0x300) == 0))
    //    v23 = 0;
    //
    //if (!v6 && (v7 & 0x6000003) != 0) {
    //    *&this->unk_0008 = 0.0;
    //    *&this->unk_000C = 0.0;
    //    this->unk_0014 = a3;
    //    CGCamera::EnableFreeLook(ActiveCamera);
    //}
    //if (!v21 && (this->m_flags & 3) != 0)
    //    CGGameUI::OnMouseModeRelative();
    //if (!v24 && (v8 = this->m_flags, (v8 & 1) != 0) && (v8 & 2) != 0) {
    //    v9 = ClntObjMgrObjectPtr(CGUnit_C::m_activeMover, TYPEMASK_UNIT);
    //    if (CGInputControl::CanSyncFreeLookFacing(this, v9)) {
    //        CGCamera::SyncFreeLookFacing(ActiveCamera);
    //        this->unk_0054 = 1;
    //    }
    //} else {
    //    v10 = this->m_flags;
    //    if ((v10 & 1) == 0 || (v10 & 2) == 0)
    //        this->unk_0054 = 0;
    //}
    //if ((controlBit & 0x1E00000) != 0)
    //    CGCamera::UpdateTrackingState(ActiveCamera, this->m_flags & 0x1E00000);
    //if ((controlBit & 0xA010F0) != 0 || !v24 && (v11 = this->m_flags, (v11 & 1) != 0) && (v11 & 2) != 0 || !v23 && (v12 = this->m_flags, (v12 & 0x2000001) != 0) && (v12 & 0x300) != 0) {
    //    v13 = this->m_flags;
    //    v14 = (v13 & 0xA010F0) != 0 || (v13 & 1) != 0 && (v13 & 2) != 0 || (v13 & 0x2000001) != 0 && (v13 & 0x300) != 0;
    //    CGCamera::UpdateBobbingState(ActiveCamera, v14);
    //}
    //if (this->unk_0044 && (controlBit & 0x300) != 0) {
    //    this->unk_0044 = 0;
    //    v15 = CGWorldFrame::GetActiveCamera();
    //    CGCamera::DecIgnoreFacing(v15);
    //}
    //if ((controlBit & 0x13F0) != 0 || !v20 && (this->m_flags & 0x4000002) != 0 || !v19 && (this->m_flags & 0x2000001) != 0 || !v23 && (v16 = this->m_flags, (v16 & 0x2000001) != 0) && (v16 & 0x300) != 0) {
    //    v17 = !v22 && CGInputControl::sub_5F9850(this);
    //    CGCamera::SmoothFreeLook(ActiveCamera, this, v17);
    //}
    //if ((controlBit & 0x30) != 0)
    //    this->m_flags &= ~0x1000u;
    //if (!v24) {
    //    v18 = this->m_flags;
    //    if ((v18 & 1) != 0 && (v18 & 2) != 0)
    //        this->m_flags = v18 & 0xFFFFEFFF;
    //}
    //if ((controlBit & 3) != 0 && (this->m_flags & 3) != controlBit)
    //    this->unk_0018 = 0;
    return 1;
}

// OFFSET: 0x5FA450
bool CGInputControl::UnsetControlBit(uint32_t controlBit, int32_t eventTime, uint32_t a4) {
    if ((controlBit & this->m_flags) == 0)
        return 0;

    this->m_flags &= ~controlBit;

    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //m_flags = this->m_flags;
    //v8 = m_flags & 0x6000003;
    //v26 = m_flags & 3;
    //v27 = (m_flags & 0x1030) == 0 && (m_flags & 0xC0) == 0 && ((m_flags & 0x2000001) == 0 || (m_flags & 0x300) == 0) && ((m_flags & 0x300) == 0 || (m_flags & 0x2000001) != 0) && (m_flags & 0x1E00000) == 0;
    //if ((m_flags & 1) == 0 || (v28 = 1, (m_flags & 2) == 0))
    //    v28 = 0;
    //v24 = m_flags & 0x4000002;
    //v23 = m_flags & 0x2000001;
    //if ((m_flags & 0x2000001) == 0 || (v29 = 1, (m_flags & 0x300) == 0))
    //    v29 = 0;
    //v9 = a2;
    //v10 = a2;
    //v11 = a2;
    //v12 = m_flags & ~a2;
    //v13 = this;
    //v14 = v9 & 0xA010F0;
    //v15 = v11 & 0x1E00000;
    //v25 = v10 & 0x13F0;
    //
    //if (v8 && (v12 & 0x6000003) == 0) {
    //    CGCamera::DisableFreeLook(ActiveCamera, a4);
    //    if (this->unk_0044 && (this->m_flags & 0x300) != 0) {
    //        CGInputControl::OnTurnToAngleStop(this);
    //        v13 = this;
    //    } else {
    //        v13 = this;
    //    }
    //}
    //if (v26 && (v13->m_flags & 3) == 0) {
    //    maybe_CGGameUI__OnMouseModeNormal();
    //    v13 = this;
    //}
    //if (v15) {
    //    CGCamera::UpdateTrackingState(ActiveCamera, v13->m_flags & 0x1E00000);
    //    v13 = this;
    //}
    //if (v14 || v28 && ((v16 = v13->m_flags, (v16 & 1) == 0) || (v16 & 2) == 0) || v29 && ((v17 = v13->m_flags, (v17 & 0x2000001) == 0) || (v17 & 0x300) == 0)) {
    //    v18 = v13->m_flags;
    //    v19 = (v18 & 0xA010F0) != 0 || (v18 & 1) != 0 && (v18 & 2) != 0 || (v18 & 0x2000001) != 0 && (v18 & 0x300) != 0;
    //    CGCamera::UpdateBobbingState(ActiveCamera, v19);
    //    v13 = this;
    //}
    //if (v25 || v24 && (v13->m_flags & 0x4000002) == 0 || v23 && (v13->m_flags & 0x2000001) == 0 || v29 && ((v20 = v13->m_flags, (v20 & 0x2000001) == 0) || (v20 & 0x300) == 0)) {
    //    v22 = 0;
    //    if (!v27) {
    //        v21 = !CGInputControl::sub_5F9850(v13);
    //        v13 = this;
    //        if (!v21)
    //            v22 = 1;
    //    }
    //    CGCamera::SmoothFreeLook(ActiveCamera, v13, v22);
    //    v13 = this;
    //}
    //if (v26 && (v13->m_flags & 3) == 0) {
    //    if (!CGInputControl::sub_5F9600(v13, a3)) {
    //        if (this->unk_0018 == 1) {
    //            CGWorldFrame::PerformDefaultAction(CGWorldFrame::s_currentWorldFrame, 1);
    //        } else if (this->unk_0018 == 2) {
    //            CGWorldFrame::PerformDefaultAction(CGWorldFrame::s_currentWorldFrame, 4);
    //        }
    //    }
    //    this->unk_0018 = 0;
    //}
    return 1;
}

// OFFSET: 0x5FAC90
bool CGInputControl::CanMove(CGUnit_C* unit) {
    if (!this->CanControl(unit))
        return 0;
    //m_unit = a1->m_unit;
    //return m_unit->UNIT_FIELD_HEALTH > 0 && (a1->m_passenger->m_flags & 0x100A00) == 0 && LOBYTE(m_unit->UNIT_FIELD_BYTES_1) != 7 && !CGUnit_C::HasVehicleTransport(a1);
    return unit->m_unit->UNIT_FIELD_HEALTH > 0;
}

// OFFSET: 0x5FA0D0
bool CGInputControl::CanTurn(CGUnit_C* unit) {
    //return CGInputControl::CanControl(a1) && (a1->m_unit->UNIT_FIELD_FLAGS & 0x40000) == 0 && !CGUnit_C::IsVehiclePreventingTurning(a1);
    return this->CanControl(unit) && (unit->m_unit->UNIT_FIELD_FLAGS & 0x40000) == 0;
}

// OFFSET: 0x5FA060
bool CGInputControl::CanControl(CGUnit_C* unit) {
    if (!unit || unit->m_unit->UNIT_FIELD_HEALTH <= 0)
        return false;

    if (unit->m_passenger->m_spline && (unit->m_passenger->m_spline->flags & 0x400) == 0)
        return false;

    //return !CGUnit_C::AnimSuppressesMovement(a1) && (!CGUnit_C::IsActivePlayer(&a1->ObjectBase) || (a1->unk_1020[1] & 1) == 0) && !CGUnit_C::IsAlteredFormTransitionPreventingMovement(a1);
    return true;
}

// OFFSET: 0x5FAE70
void CGInputControl::MovePlayer(int32_t eventTime, CGUnit_C* unit) {
    int32_t dir = 0;

    //if ((unit->m_unit->UNIT_FIELD_FLAGS_2 & 0x40) != 0 || unit->IsVehicleCurrentlyUnstoppable()) {
    //    dir = 1;
    //} else {
        if ((this->m_flags & CONTROL_AUTORUN) != 0)
            dir = 1;
        if ((this->m_flags & CONTROL_MOVEFORWARD) != 0)
            dir++;
        if ((this->m_flags & CONTROL_TURNORACTION) != 0 && (this->m_flags & CONTROL_CAMERAORSELECTORMOVE) != 0)
            dir++;
        if ((this->m_flags & CONTROL_MOVEBACKWARD) != 0)
            dir--;

        if (!dir) {
            if ((this->m_flags & CONTROL_MOVE_SENT) != 0) {
                unit->OnMoveStopLocal(eventTime);
                this->m_flags &= ~CONTROL_MOVE_SENT;
            }
            return;
        }
    //}

    //if ((a3->ObjectBase.ukn78)(a3)) {
    //    CGUnit_C::TryChangeStandState(0);
    //    ActiveCamera = CGWorldFrame::GetActiveCamera();
    //    if (CGInputControl::CanSyncFreeLookFacing(this, a3))
    //        CGCamera::SyncFreeLookFacing(ActiveCamera);
    //}

    //CGPlayer_C* activePlayer = ClntObjMgrGetActivePlayerObj();
    //if (activePlayer)
    //    activePlayer->ClearAFK(0);

    if (dir <= 0) {
        if ((this->m_flags & CONTROL_MOVE_SENT) == 0 || (unit->m_passenger->m_flags & 0x1) != 0) {
            unit->OnMoveStartLocal(eventTime, 0);
            this->m_flags |= CONTROL_MOVE_SENT;
        }
    } else if ((this->m_flags & CONTROL_MOVE_SENT) == 0 || (unit->m_passenger->m_flags & 0x2) != 0) {
        unit->OnMoveStartLocal(eventTime, 1);
        this->m_flags |= CONTROL_MOVE_SENT;
    }
}

// OFFSET: 0x5FAFB0
void CGInputControl::StrafePlayer(int32_t eventTime, CGUnit_C* unit) {
    int32_t dir = 0;
    if ((this->m_flags & CONTROL_STRAFELEFT) != 0)
        dir++;
    if ((this->m_flags & (CONTROL_TRACK_MOVE | CONTROL_TURNORACTION)) != 0 && (this->m_flags & CONTROL_TURNLEFT) != 0)
        dir++;
    if ((this->m_flags & CONTROL_STRAFERIGHT) != 0)
        dir--;
    if ((this->m_flags & (CONTROL_TRACK_MOVE | CONTROL_TURNORACTION)) != 0 && (this->m_flags & CONTROL_TURNRIGHT) != 0)
        dir--;

    if (dir) {
        //if ((unit->ObjectBase.ukn78)(a3))
        //    CGUnit_C::TryChangeStandState(0);

        //CGPlayer_C* activePlayer = ClntObjMgrGetActivePlayerObj();
        //if (activePlayer)
        //    activePlayer->ClearAFK(0);

        if (dir <= 0) {
            if ((this->m_flags & CONTROL_STRAFE_SENT) == 0) {
                unit->OnStrafeStartLocal(eventTime, 0);
                this->m_flags |= CONTROL_STRAFE_SENT;
            }
        } else if ((this->m_flags & CONTROL_STRAFE_SENT) == 0) {
            unit->OnStrafeStartLocal(eventTime, 1);
            this->m_flags |= CONTROL_STRAFE_SENT;
        }
    } else if ((m_flags & CONTROL_STRAFE_SENT) != 0) {
        unit->OnStrafeStopLocal(eventTime);
        this->m_flags &= ~CONTROL_STRAFE_SENT;
    }
}

// OFFSET: 0x5FACE0
void CGInputControl::AscendDescendPlayer(int32_t eventTime, CGUnit_C* unit) {
    if ((this->m_flags & (CONTROL_VIRTUAL_TURNORACTION | CONTROL_TRACK_MOVE)) == 0) {
        this->m_flags &= ~CONTROL_ASCENDDESCEND_SENT;
        return;
    }

    int32_t dir = 0;
    if ((this->m_flags & CONTROL_ASCEND) != 0)
        dir++;
    if ((this->m_flags & CONTROL_DESCEND) != 0)
        dir--;

    //if ((m_passenger->m_flags2 & 0x10) != 0) {
    //    if (v7 <= 0) {
    //        if (v7 >= 0) {
    //            if ((v6 & 0x100000) != 0) {
    //                CGUnit_C::OnPitchStopLocal(a3, a2);
    //                this->m_flags &= ~0x100000u;
    //                v9 = (a3->ObjectBase.__vftable[1].PostReenable)(a3);
    //                CGInputControl::SendUIVehicleAngleUpdate(v9);
    //            }
    //        } else if ((v6 & 0x100000) == 0 || (m_flags & 0x40) != 0) {
    //            CGUnit_C::OnPitchStartLocal(a3, a2, 0);
    //            this->m_flags |= 0x100000u;
    //        }
    //    } else if ((v6 & 0x100000) == 0 || (m_flags & 0x80u) != 0) {
    //        CGUnit_C::OnPitchStartLocal(a3, a2, 1);
    //        this->m_flags |= 0x100000u;
    //    }
    //} else if (v7) {
    //    ActivePlayer = ClntObjMgrGetActivePlayer();
    //    if (ClntObjMgrObjectPtr(ActivePlayer, TYPEMASK_PLAYER))
    //        CGPlayer_C::ClearAFK(0);
    //    if (v7 <= 0) {
    //        if ((this->m_flags & 0x100000) == 0 || (a3->m_passenger->m_flags & 0x400000) != 0) {
    //            CGUnit_C::OnAscendDescendStartLocal(a3, a2, 0);
    //            this->m_flags |= 0x100000u;
    //        }
    //    } else if ((this->m_flags & 0x100000) == 0 || ((&loc_7FFFFF + 1) & a3->m_passenger->m_flags) != 0) {
    //        CGUnit_C::OnAscendDescendStartLocal(a3, a2, 1);
    //        this->m_flags |= 0x100000u;
    //    }
    //} else if ((v6 & 0x100000) != 0) {
    //    CGUnit_C::OnAscendDescendStopLocal(a3, a2);
    //    this->m_flags &= ~0x100000u;
    //}
}

// OFFSET: 0x5FB0B0
void CGInputControl::TurnPlayer(int32_t eventTime, CGUnit_C* unit) {
    int32_t dir = 0;
    if ((this->m_flags & CONTROL_TURNLEFT) != 0)
        dir++;
    if ((this->m_flags & CONTROL_TURNRIGHT) != 0)
        dir--;

    if (((m_flags & (CONTROL_VIRTUAL_TURNORACTION | CONTROL_TURNORACTION)) == 0 || unit->NoStrafe()) && dir) {
        //if ((unit->ObjectBase.ukn78)(a3))
        //    CGUnit_C::TryChangeStandState(0);

        // CGPlayer_C* activePlayer = ClntObjMgrGetActivePlayerObj();
        // if (activePlayer)
        //     activePlayer->ClearAFK(0);

        if (dir <= 0) {
            if ((this->m_flags & CONTROL_TURN_SENT) == 0) {
                unit->OnTurnStartLocal(eventTime, 0);
                this->m_flags |= CONTROL_TURN_SENT;
            }
        } else if ((this->m_flags & CONTROL_TURN_SENT) == 0) {
            unit->OnTurnStartLocal(eventTime, 1);
            this->m_flags |= CONTROL_TURN_SENT;
        }
    } else if ((this->m_flags & CONTROL_TURN_SENT) != 0) {
        unit->OnTurnStopLocal(eventTime);
        this->m_flags &= ~CONTROL_TURN_SENT;
    }
}

// OFFSET: 0x5FB1A0
void CGInputControl::PitchPlayer(int32_t eventTime, CGUnit_C* unit) {
    if ((m_flags & (CONTROL_VIRTUAL_TURNORACTION | CONTROL_TURNORACTION)) != 0)
        return;

    int32_t dir = 0;
    if ((this->m_flags & CONTROL_PITCHUP) != 0)
        dir++;
    if ((this->m_flags & CONTROL_PITCHDOWN) != 0)
        dir--;

    if (dir) {
        if (dir <= 0) {
            if ((this->m_flags & CONTROL_PITCH_SENT) == 0) {
                unit->OnPitchStartLocal(eventTime, 0);
                this->m_flags |= CONTROL_PITCH_SENT;
            }
        } else if ((this->m_flags & CONTROL_PITCH_SENT) == 0) {
            unit->OnPitchStartLocal(eventTime, 1);
            this->m_flags |= CONTROL_PITCH_SENT;
        }
    } else if ((this->m_flags & CONTROL_PITCH_SENT) != 0) {
        unit->OnPitchStopLocal(eventTime);
        this->m_flags &= ~CONTROL_PITCH_SENT;

        //v6 = (a3->ObjectBase.__vftable[1].PostReenable)(a3);
        //CGInputControl::SendUIVehicleAngleUpdate(v6);
    }
}

